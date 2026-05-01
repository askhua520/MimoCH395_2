/**
 * @file    web_server.c
 * @brief   HTTP web server on port 80 via CH395
 */

#include "web_server.h"
#include "config.h"
#include "config_types.h"
#include "config_storage.h"
#include "data_pool.h"
#include "utils.h"
#include "log_uart.h"
#include "ch395_drv.h"
#include "web_resources.h"
#include <string.h>

#define WEB_RX_BUF_SIZE   1024
#define WEB_TX_BUF_SIZE   4096
#define WEB_SOCK          2   /* Socket for web server */

static uint8_t s_rx_buf[WEB_RX_BUF_SIZE];
static char    s_tx_buf[WEB_TX_BUF_SIZE];
static uint8_t s_initialized;

void WebServer_Init(void)
{
    s_initialized = 0;
}

/**
 * @brief  Send HTTP response
 */
static void send_http_response(uint8_t sock, uint16_t status, const char *content_type,
                                const char *body, uint32_t body_len)
{
    int hdr_len = snprintf(s_tx_buf, WEB_TX_BUF_SIZE,
        "HTTP/1.1 %d OK\r\n"
        "Content-Type: %s\r\n"
        "Content-Length: %lu\r\n"
        "Connection: close\r\n"
        "\r\n",
        status, content_type, (unsigned long)body_len);

    if (hdr_len + body_len < WEB_TX_BUF_SIZE) {
        memcpy(s_tx_buf + hdr_len, body, body_len);
        CH395_TcpSend(sock, (uint8_t *)s_tx_buf, (uint16_t)(hdr_len + body_len));
    }
}

/**
 * @brief  Handle GET / -> return main HTML page
 */
static void handle_get_root(uint8_t sock)
{
    send_http_response(sock, 200, "text/html", web_index_html, web_index_html_len);
}

/**
 * @brief  Handle GET /api/status -> return sensor data as JSON
 */
static void handle_get_status(uint8_t sock)
{
    const SystemConfig_t *cfg = ConfigStorage_GetConfig();
    char *p = s_tx_buf;
    char *end = s_tx_buf + WEB_TX_BUF_SIZE - 64;

    p = Utils_StrAppend(p, "{\"sensors\":[");

    if (cfg != NULL) {
        for (uint8_t si = 0; si < cfg->sensor_count && si < CFG_MAX_SENSORS; si++) {
            if (si > 0) *p++ = ',';
            p = Utils_StrAppend(p, "{\"addr\":");
            p = Utils_Uint16ToStr(p, cfg->sensors[si].slave_addr);
            p = Utils_StrAppend(p, ",\"data\":{");

            for (uint8_t di = 0; di < cfg->sensors[si].dp_count && di < CFG_MAX_DATA_POINTS; di++) {
                if (di > 0) *p++ = ',';
                *p++ = '"';
                p = Utils_StrAppend(p, cfg->sensors[si].dp[di].name);
                p = Utils_StrAppend(p, "\":");
                p = Utils_FloatToStr(p, DataPool_GetScaledValue(si, di), 1);
            }
            p = Utils_StrAppend(p, "}}");
            if (p >= end) break;
        }
    }

    p = Utils_StrAppend(p, "],\"totalRegs\":");
    p = Utils_Uint16ToStr(p, DataPool_GetTotalRegs());
    p = Utils_StrAppend(p, "}");

    send_http_response(sock, 200, "application/json", s_tx_buf, (uint32_t)(p - s_tx_buf));
}

/**
 * @brief  Handle GET /api/config -> return config as JSON
 */
static void handle_get_config(uint8_t sock)
{
    const SystemConfig_t *cfg = ConfigStorage_GetConfig();
    char *p = s_tx_buf;
    char *end = s_tx_buf + WEB_TX_BUF_SIZE - 64;

    p = Utils_StrAppend(p, "{\"sensorCount\":");
    if (cfg != NULL) {
        p = Utils_Uint16ToStr(p, cfg->sensor_count);
        p = Utils_StrAppend(p, ",\"slaveAddr\":");
        p = Utils_Uint16ToStr(p, cfg->slave_addr);
        p = Utils_StrAppend(p, ",\"pollInterval\":");
        p = Utils_Uint16ToStr(p, cfg->poll_interval_ms);
        p = Utils_StrAppend(p, ",\"collectBaud\":");
        p = Utils_Uint16ToStr(p, (uint16_t)(cfg->collect_baud / 1000));
        p = Utils_StrAppend(p, "000");
        p = Utils_StrAppend(p, ",\"forwardBaud\":");
        p = Utils_Uint16ToStr(p, (uint16_t)(cfg->forward_baud / 1000));
        p = Utils_StrAppend(p, "000");
        p = Utils_StrAppend(p, ",\"sensors\":[");

        for (uint8_t si = 0; si < cfg->sensor_count && si < CFG_MAX_SENSORS; si++) {
            if (si > 0) *p++ = ',';
            p = Utils_StrAppend(p, "{\"addr\":");
            p = Utils_Uint16ToStr(p, cfg->sensors[si].slave_addr);
            p = Utils_StrAppend(p, ",\"dpCount\":");
            p = Utils_Uint16ToStr(p, cfg->sensors[si].dp_count);
            p = Utils_StrAppend(p, "}");
        }
        p = Utils_StrAppend(p, "],\"cameras\":[");

        for (uint8_t ci = 0; ci < cfg->sensor_count && ci < CFG_MAX_CAMERAS; ci++) {
            if (ci > 0) *p++ = ',';
            p = Utils_StrAppend(p, "{\"ip\":\"");
            p = Utils_StrAppend(p, cfg->cameras[ci].ip);
            p = Utils_StrAppend(p, "\",\"channel\":");
            p = Utils_Uint16ToStr(p, cfg->cameras[ci].channel);
            p = Utils_StrAppend(p, "}");
        }
        p = Utils_StrAppend(p, "]}");
    } else {
        p = Utils_StrAppend(p, "\"error\":\"no config\"}");
    }

    send_http_response(sock, 200, "application/json", s_tx_buf, (uint32_t)(p - s_tx_buf));
}

/**
 * @brief  Handle POST /api/config -> update config
 */
static void handle_post_config(uint8_t sock, char *body)
{
    /* Simple parser: expect JSON body with sensor config */
    /* For simplicity, just save and return success */
    /* In production, use a proper JSON parser */

    ConfigStorage_Save();
    DataPool_RebuildMap();

    const char *resp = "{\"result\":\"ok\"}";
    send_http_response(sock, 200, "application/json", resp, strlen(resp));
}

/**
 * @brief  Parse HTTP request and route to handler
 */
static void route_request(uint8_t sock, uint8_t *data, uint16_t len)
{
    char *req = (char *)data;
    req[len] = '\0';

    /* Find method and path */
    char method[8] = {0};
    char path[128] = {0};
    int i = 0;

    /* Parse method */
    while (i < len && req[i] != ' ' && i < 7) {
        method[i] = req[i];
        i++;
    }
    method[i] = '\0';
    while (i < len && req[i] == ' ') i++;

    /* Parse path */
    int j = 0;
    while (i < len && req[i] != ' ' && req[i] != '?' && j < 127) {
        path[j++] = req[i++];
    }
    path[j] = '\0';

    /* Route */
    if (strcmp(method, "GET") == 0) {
        if (strcmp(path, "/") == 0) {
            handle_get_root(sock);
        } else if (strcmp(path, "/api/status") == 0) {
            handle_get_status(sock);
        } else if (strcmp(path, "/api/config") == 0) {
            handle_get_config(sock);
        } else {
            const char *resp = "<h1>404 Not Found</h1>";
            send_http_response(sock, 404, "text/html", resp, strlen(resp));
        }
    } else if (strcmp(method, "POST") == 0) {
        if (strcmp(path, "/api/config") == 0) {
            /* Find body after \r\n\r\n */
            char *body = strstr(req, "\r\n\r\n");
            if (body) body += 4;
            handle_post_config(sock, body);
        } else {
            const char *resp = "<h1>404 Not Found</h1>";
            send_http_response(sock, 404, "text/html", resp, strlen(resp));
        }
    }
}

void WebServer_Process(void)
{
    if (!s_initialized) {
        if (CH395_TcpListen(WEB_SOCK, CFG_WEB_PORT) == 0) {
            s_initialized = 1;
            Log_Printf("WebServer: Listening on port %d\r\n", CFG_WEB_PORT);
        }
        return;
    }

    int16_t rlen = CH395_TcpRecv(WEB_SOCK, s_rx_buf, sizeof(s_rx_buf) - 1);
    if (rlen > 0) {
        route_request(WEB_SOCK, s_rx_buf, (uint16_t)rlen);
    }
}
