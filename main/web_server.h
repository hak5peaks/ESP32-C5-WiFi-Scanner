#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include <esp_http_server.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 
 * 
 * @return 
 */
httpd_handle_t start_webserver(void);

#ifdef __cplusplus
}
#endif

#endif
