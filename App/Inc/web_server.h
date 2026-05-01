#ifndef __WEB_SERVER_H
#define __WEB_SERVER_H

#include <stdint.h>

/**
 * @brief  Initialize web server
 */
void WebServer_Init(void);

/**
 * @brief  Non-blocking web server process (call in main loop)
 */
void WebServer_Process(void);

#endif /* __WEB_SERVER_H */
