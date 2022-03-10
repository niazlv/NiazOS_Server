#ifndef __NIAZOS_WEBSERVER_H__
# define __NIAZOS_WEBSERVER_H__

# include "NiazOS.h"

httpd_handle_t start_webserver(void);
void disconnect_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);
void connect_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);
#endif//__NIAZOS_WEBSERVER_H__