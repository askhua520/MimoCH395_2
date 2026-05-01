/**
 * @file    isapi_client.c
 * @brief   ISAPI OSD update client - non-blocking HTTP POST via CH395
 */

#include "isapi_client.h"
#include "config.h"
#include "config_types.h"
#include "config_storage.h"
#include "data_pool.h"
#include "utils.h"
#include "log_uart.h"
#include "ch395_drv.h"
#include <string.h>
#include <stdio.h>

/* State machine */
typedef enum {
    ISTATE_IDLE,
    ISTATE_CHECK_UPDATE,
    ISTATE_BUILD_REQUEST,
    ISTATE_SEND_HTTP,
    ISTATE_WAIT_RESPONSE,
    ISTATE_DONE
} IsapiState_t;

static IsapiState_t s_state;
static uint8_t  s_cur_cam;
static uint32_t s_last_update_tick;
static int32_t  s_prev_values[CFG_MAX_SENSORS][CFG_MAX_DATA_POINTS];
static uint32_t s_cam_update_tick[CFG_MAX_CAMERAS];  /* Per-camera last update time */

/* HTTP send buffer */
#define ISAPI_TX_BUF_SIZE  1536
#define ISAPI_RX_BUF_SIZE  512
#define ISAPI_SOCK         3   /* Use socket 3 for ISAPI */
#define ISAPI_HTTP_TIMEOUT 3000

static char    s_tx_buf[ISAPI_TX_BUF_SIZE];
static uint8_t s_rx_buf[ISAPI_RX_BUF_SIZE];

/* Base64 encoder */
static const char b64_table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static uint16_t base64_encode(const uint8_t *src, uint16_t src_len, char *dst)
{
    uint16_t i = 0;
    uint16_t j = 0;
    while (i < src_len) {
        uint32_t a = i < src_len ? src[i++] : 0;
        uint32_t b = i < src_len ? src[i++] : 0;
        uint32_t c = i < src_len ? src[i++] : 0;
        uint32_t triple = (a << 16) | (b << 8) | c;

        dst[j++] = b64_table[(triple >> 18) & 0x3F];
        dst[j++] = b64_table[(triple >> 12) & 0x3F];
        dst[j++] = (i > src_len + 1) ? '=' : b64_table[(triple >> 6) & 0x3F];
        dst[j++] = (i > src_len) ? '=' : b64_table[triple & 0x3F];
    }
    dst[j] = '\0';
    return j;
}

void IsapiClient_Init(void)
{
    s_state = ISTATE_IDLE;
    s_cur_cam = 0;
    s_last_update_tick = 0;
    memset(s_prev_values, 0, sizeof(s_prev_values));
    memset(s_cam_update_tick, 0, sizeof(s_cam_update_tick));
}

/**
 * @brief  Build OSD text string from sensor data
 *         Format: "smoke:0,water:1,temp:23.5,humi:65.0"
 */
static uint16_t build_osd_text(uint8_t sensor_idx, char *buf, uint16_t max_len)
{
    const SystemConfig_t *cfg = ConfigStorage_GetConfig();
    if (cfg == NULL || sensor_idx >= cfg->sensor_count) { buf[0] = '\0'; return 0; }

    char *p = buf;
    char *end = buf + max_len - 1;
    const SensorConfig_t *sns = &cfg->sensors[sensor_idx];

    for (uint8_t di = 0; di < sns->dp_count && di < CFG_MAX_DATA_POINTS; di++) {
        if (p >= end) break;

        /* Append name */
        p = Utils_StrAppend(p, sns->dp[di].name);
        if (p >= end) break;
        *p++ = ':';

        /* Append value based on data type */
        int32_t raw = DataPool_GetValue(sensor_idx, di);
        uint16_t div = sns->dp[di].scale_divisor;
        if (div == 0) div = 1;

        if (div == 1) {
            /* Integer display */
            p = Utils_Int16ToStr(p, (int16_t)raw);
        } else {
            /* Float display */
            float val = (float)raw / (float)div;
            p = Utils_FloatToStr(p, val, 1);
        }

        if (p >= end) break;

        /* Comma separator (not after last) */
        if (di < sns->dp_count - 1) {
            *p++ = ',';
        }
    }
    *p = '\0';
    return (uint16_t)(p - buf);
}

void IsapiClient_Process(void)
{
    const SystemConfig_t *cfg = ConfigStorage_GetConfig();
    if (cfg == NULL) return;

    switch (s_state) {
    case ISTATE_IDLE:
        /* Check periodically */
        if (Utils_IsElapsed(s_last_update_tick, CFG_OSD_UPDATE_MIN_MS)) {
            s_state = ISTATE_CHECK_UPDATE;
        }
        break;

    case ISTATE_CHECK_UPDATE:
        /* Find next camera that needs update */
        s_cur_cam = 0xFF;
        for (uint8_t si = 0; si < cfg->sensor_count && si < CFG_MAX_CAMERAS; si++) {
            /* Check if this sensor data changed */
            uint8_t changed = 0;
            for (uint8_t di = 0; di < cfg->sensors[si].dp_count && di < CFG_MAX_DATA_POINTS; di++) {
                int32_t cur = DataPool_GetValue(si, di);
                if (cur != s_prev_values[si][di]) {
                    changed = 1;
                    s_prev_values[si][di] = cur;
                }
            }

            if (changed && Utils_IsElapsed(s_cam_update_tick[si], CFG_OSD_UPDATE_MIN_MS)) {
                s_cur_cam = si;
                break;
            }
        }

        if (s_cur_cam < CFG_MAX_CAMERAS) {
            s_state = ISTATE_BUILD_REQUEST;
        } else {
            s_state = ISTATE_IDLE;
        }
        break;

    case ISTATE_BUILD_REQUEST: {
        if (s_cur_cam >= cfg->sensor_count || s_cur_cam >= CFG_MAX_CAMERAS) {
            s_state = ISTATE_DONE;
            break;
        }

        const CameraConfig_t *cam = &cfg->cameras[s_cur_cam];
        if (cam->ip[0] == '\0') {
            s_state = ISTATE_DONE;
            break;
        }

        /* Build OSD text for this sensor */
        char osd_text[256];
        build_osd_text(s_cur_cam, osd_text, sizeof(osd_text));

        /* Build XML body for ISAPI OSD */
        char body[512];
        int body_len = snprintf(body, sizeof(body),
            "<TextOverlay><id>1</id><enabled>true</enabled>"
            "<positionX>0</positionX><positionY>0</positionY>"
            "<displayText>%s</displayText></TextOverlay>", osd_text);

        /* Base64 encode credentials */
        char cred_plain[64];
        int cred_len = snprintf(cred_plain, sizeof(cred_plain), "%s:%s", cam->user, cam->pass);
        char cred_b64[128];
        base64_encode((uint8_t *)cred_plain, (uint16_t)cred_len, cred_b64);

        /* Build HTTP POST request */
        int tx_len = snprintf(s_tx_buf, ISAPI_TX_BUF_SIZE,
            "POST /ISAPI/System/Video/inputs/channels/%d/overlays/text HTTP/1.1\r\n"
            "Host: %s\r\n"
            "Content-Type: application/xml\r\n"
            "Authorization: Basic %s\r\n"
            "Content-Length: %d\r\n"
            "Connection: close\r\n"
            "\r\n"
            "%s",
            cam->channel, cam->ip, cred_b64, body_len, body);

        /* Connect to camera and send */
        if (CH395_TcpConnect(ISAPI_SOCK, (uint8_t *)cam->ip, 80) == 0) {
            CH395_TcpSend(ISAPI_SOCK, (uint8_t *)s_tx_buf, (uint16_t)tx_len);
            s_cam_update_tick[s_cur_cam] = Utils_GetTick();
            s_state = ISTATE_WAIT_RESPONSE;
        } else {
            Log_Printf("ISAPI: Connect failed cam%d\r\n", s_cur_cam);
            s_state = ISTATE_DONE;
        }
        break;
    }

    case ISTATE_WAIT_RESPONSE: {
        int16_t rlen = CH395_TcpRecv(ISAPI_SOCK, s_rx_buf, sizeof(s_rx_buf));
        if (rlen > 0) {
            /* Check for 200 OK */
            if (rlen >= 15 && memcmp(s_rx_buf, "HTTP/1.1 200", 12) == 0) {
                Log_Printf("ISAPI: cam%d OSD updated\r\n", s_cur_cam);
            } else {
                Log_Printf("ISAPI: cam%d resp error\r\n", s_cur_cam);
            }
            s_state = ISTATE_DONE;
        } else if (Utils_IsElapsed(s_cam_update_tick[s_cur_cam], ISAPI_HTTP_TIMEOUT)) {
            Log_Printf("ISAPI: cam%d timeout\r\n", s_cur_cam);
            s_state = ISTATE_DONE;
        }
        break;
    }

    case ISTATE_DONE:
        s_last_update_tick = Utils_GetTick();
        s_state = ISTATE_IDLE;
        break;
    }
}
