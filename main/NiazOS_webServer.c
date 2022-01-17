#include "NiazOS_webServer.h"
#include "esp_system.h"
#include <driver/adc.h>

extern uint8_t temprature_sens_read();

static const char *TAG = "wifi station";

static esp_err_t adder_get_handler(httpd_req_t * req)
{
    unsigned *visitors = (unsigned *)req->user_ctx;
    char outbuf[512];
    ESP_LOGI("webserver", "get REQUEST");
    snprintf(outbuf, sizeof(outbuf), "%s \n %d", "Hello!!!", *visitors);
    httpd_resp_send(req, outbuf, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}
static esp_err_t adder_post_handler(httpd_req_t * req)
{
    char outbuf[50];
    snprintf(outbuf, sizeof(outbuf), "%s", "Hello!!!");
    httpd_resp_send(req, outbuf, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}
static esp_err_t adder_put_handler(httpd_req_t * req)
{
    char outbuf[50];
    snprintf(outbuf, sizeof(outbuf), "%s", "Hello!!!");
    httpd_resp_send(req, outbuf, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}
esp_chip_info_t chip_info;
static esp_err_t info_get_handler(httpd_req_t * req)
{
    adc1_config_width(ADC_WIDTH_BIT_12);
    char outbuf[512];
    uint8_t mac[6];
    esp_base_mac_addr_get(mac);
    snprintf(outbuf,sizeof(outbuf),
            "info:\n"
            " esp chip info:\n"
            "  Config IDF: %s\n"
            "  model: %d\n"
            "  silicon revision: %d\n"
            "  CPU core(s): %d\n"
            "  features: %s%s%s%s\n"
            "mac: %02X:%02X:%02X:%02X:%02X:%02X\n"
            "flash: %dMB %s flash\n"
            "Minimum free heap size: %d bytes\n"
            "temp: %.2f C\n"
            "hall: %d \n",
        CONFIG_IDF_TARGET,
        chip_info.model,
        chip_info.revision,
        chip_info.cores,
        (chip_info.features & CHIP_FEATURE_WIFI_BGN) ? "/WIFI" : "",
        (chip_info.features & CHIP_FEATURE_BT) ? "/BT" : "",
        (chip_info.features & CHIP_FEATURE_BLE) ? "/BLE" : "",
        (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "/EMB_FLASH" : "",
        mac[0],mac[1],mac[2],mac[3],mac[4],mac[5],
        spi_flash_get_chip_size() / (1024 * 1024),
        (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external",
        esp_get_minimum_free_heap_size(),
        (temprature_sens_read() - 32) / 1.8,
        hall_sensor_read()
        );

    httpd_resp_set_type(req,"text/plain");  //if needed a html support, u can use "text/html" context type
    httpd_resp_send(req,outbuf,HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

/* Maintain a variable which stores the number of times
 * the "/adder" URI has been visited */
static unsigned visitors = 0;

static const httpd_uri_t adder_post = {
    .uri      = "/adder",
    .method   = HTTP_POST,
    .handler  = adder_post_handler,
    .user_ctx = &visitors
};

static const httpd_uri_t adder_get = {
    .uri      = "/adder",
    .method   = HTTP_GET,
    .handler  = adder_get_handler,
    .user_ctx = &visitors
};

static const httpd_uri_t adder_put = {
    .uri      = "/adder",
    .method   = HTTP_PUT,
    .handler  = adder_put_handler,
    .user_ctx = &visitors
};

static const httpd_uri_t info_get = {
    .uri      = "/info",
    .method   = HTTP_GET,
    .handler  = info_get_handler
    //.user_ctx = &visitors
};


httpd_handle_t start_webserver(void)
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    // Start the httpd server
    ESP_LOGI("webserver", "Starting server on port: '%d'", config.server_port);
    httpd_handle_t server;
    esp_chip_info(&chip_info);
    if (httpd_start(&server, &config) == ESP_OK) {
        // Set URI handlers
        ESP_LOGI("webserver", "Registering URI handlers");
        httpd_register_uri_handler(server, &adder_get);
        httpd_register_uri_handler(server, &adder_put);
        httpd_register_uri_handler(server, &adder_post);
        httpd_register_uri_handler(server, &info_get);
        return server;
    }

    ESP_LOGI("webserver", "Error starting server!");
    return NULL;
}

static void stop_webserver(httpd_handle_t server)
{
    // Stop the httpd server
    httpd_stop(server);
}

void connect_handler(void* arg, esp_event_base_t event_base,
                            int32_t event_id, void* event_data)
{
    httpd_handle_t* server = (httpd_handle_t*) arg;
    if (*server == NULL) {
        ESP_LOGI(TAG, "Starting webserver");
        *server = start_webserver();
    }
}

void disconnect_handler(void* arg, esp_event_base_t event_base,
                               int32_t event_id, void* event_data)
{
    httpd_handle_t* server = (httpd_handle_t*) arg;
    if (*server) {
        ESP_LOGI(TAG, "Stopping webserver");
        stop_webserver(*server);
        *server = NULL;
    }
    ESP_ERROR_CHECK(esp_wifi_start());
}
