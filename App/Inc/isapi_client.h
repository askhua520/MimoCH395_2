#ifndef __ISAPI_CLIENT_H
#define __ISAPI_CLIENT_H

#include <stdint.h>

/**
 * @brief  Initialize ISAPI OSD client
 */
void IsapiClient_Init(void);

/**
 * @brief  Non-blocking ISAPI OSD update process (call in main loop)
 */
void IsapiClient_Process(void);

#endif /* __ISAPI_CLIENT_H */
