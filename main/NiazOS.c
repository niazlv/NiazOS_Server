#include "NiazOS.h"
#include "NiazOS_WiFi.h"
#include "NiazOS_webServer.h"

//#include "sdkconfig.h"
#define BLINK_GPIO 13

#define EXAMPLE_ESP_WIFI_SSID      CONFIG_ESP_WIFI_SSID
#define EXAMPLE_ESP_WIFI_PASS      CONFIG_ESP_WIFI_PASSWORD


void app_main(void)
{
    static httpd_handle_t server = NULL;
	printf("Hello world!");
    //Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    //init wifi config 


    wifi_config_t wifi_config = {
         .sta = {
             .ssid = EXAMPLE_ESP_WIFI_SSID,
             .password = EXAMPLE_ESP_WIFI_PASS,
             .threshold.authmode = WIFI_AUTH_WPA2_PSK,
             .pmf_cfg = {
                 .capable = true,
                 .required = false
             },
         },
     };


    /*
    wifi_config.sta.ssid = (unsigned char *)"Home";//(uint8_t *)EXAMPLE_ESP_WIFI_SSID;
    wifi_config.sta.password = (unsigned char *)"34ValI45";//(uint8_t *)EXAMPLE_ESP_WIFI_PASS;
    wifi_config.sta.threshold.authmode = WIFI_AUTH_WPA2_PSK;
    wifi_config.sta.pmf_cfg.capable = true;
    wifi_config.sta.pmf_cfg.required = false;
	*/
    //if NVS init, then call wifi connect
    if (NiazOS_wifi_init_sta(&wifi_config))
        printf("wifi do not be connected.");
    else
    {
        server = start_webserver();

        ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &connect_handler, &server));
        ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_STA_DISCONNECTED, &disconnect_handler, &server));
    }
	gpio_reset_pin(BLINK_GPIO);
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);
    //xTaskCreate(&http_get_task, "http_get_task", 4096, NULL, 5, NULL);
    int32_t c = 0;
    while(1) {
        // wifi_ap_record_t wifidata;
        // if (esp_wifi_sta_get_ap_info(&wifidata)==0)
        //     printf("rssi:%d\r\n", wifidata.rssi);
        /* Blink off (output low) */
        //printf((c%2==0)?"Turning off the LED\n":"Turning on the LED\n");
        gpio_set_level(BLINK_GPIO, c%2);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        c++;
    }
}