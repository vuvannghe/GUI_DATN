/* Esptouch example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

#include "sdkconfig.h"
#include "esp_wifi.h"
#include "esp_wpa2.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_smartconfig.h"
#include "esp_http_client.h"
#include "esp_tls.h"

#include "driver/gpio.h"
#include "driver/spi_common.h"
#include "driver/spi_master.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/timers.h"
#include "freertos/queue.h"
#include "freertos/ringbuf.h"
#include "freertos/event_groups.h"

#include "esp_lcd_ili9341.h"
#include "extended_ili9341.h"
#include "ui/ui.h"
#include "sdcard.h"
#include "main_common.h"

static const char *TAG = "Main";

// DEFINE SECTION
#define WIFI_connect_max_attemp_number CONFIG_MAX_RECONNECT_ATTEMP_NUMBER
#define SW_BUILT_IN 0           // Boot button (NEGATIVE EDGE)
#define SC_SW 32                // Switch to turn into smart config mode  (NEGATIVE EDGE)
#define IMG_DOWNLOAD_SW 35      // Switch to connect to server, send request, receive response from server and save the image to SD card (NEGATIVE EDGE)
#define IMG_FORWARD_SHOW_SW 21  // Switch to display images from SD card one by one (NEGATIVE EDGE)
#define IMG_BACKWARD_SHOW_SW 34 // Switch to display images from SD card one by one (NEGATIVE EDGE)

// Event group
EventGroupHandle_t wifi_control_eventGroup;
EventGroupHandle_t measure_control_eventGroup;

// Semaphore
SemaphoreHandle_t xWifiControlSemaphore = NULL;
SemaphoreHandle_t xImageDownloadSemaphore = NULL;
SemaphoreHandle_t xImageShowSemaphore = NULL;
SemaphoreHandle_t SyncSemaphore = NULL;

// Task handle
TaskHandle_t image_download_task_handle = NULL;
static const char *SD_TAG = "SD CARD";
static const char *SC_TAG = "SMART CONFIG";
static void smartconfig_itr_handler(void *parm);
static smartconfig_start_config_t smart_cfg = SMARTCONFIG_START_CONFIG_DEFAULT();
static uint8_t attemp_count = 0;
bool change_AP = false;
bool press_to_change_AP = false;
bool is_connected = false;
static uint8_t attemp_count_1 = 0;
static wifi_config_t wifi_config;
uint8_t old_ssid[32] = CONFIG_DEFAULT_AP_SSID;
uint8_t old_password[64] = CONFIG_DEFAULT_AP_PASSWORD;
uint8_t old_bssid[6] = {0x34, 0xE8, 0x94, 0xF6, 0xAA, 0xD7};

lv_disp_t *lcd_mainscreen;
char list_img[100][64];
int16_t image_index = -1;
uint16_t image_count = 0;
bool forward = false;
bool backward = false;

#define MAX_HTTP_OUTPUT_BUFFER 2048
#define MAX_HTTP_RECV_BUFFER 512
#define MAX_HTTP_OUTPUT_BUFFER 2048
static const char *IMG_TAG = "HTTP_CLIENT";

#define IMG_ALREADY_EXSISTS 1
#define IMG_FAILED_TO_WRITE 2
#define IMG_FILE_FAILED_TO_OPEN_TO_WRITE 3
#define IMG_FILE_DOWNLOADED_SUCCESSFULLY 4
#define FAILED_TO_CONNECT_TO_SERVER 5
#define IMG_DOWNLOADING 6

/*------------------------------------ WIFI ------------------------------------ */

/**
 * @brief Interrupt handler for the smart config button.
 *
 * This function is triggered when the smart config button is pressed.
 * It checks if the system is connected to a network, and if so, it
 * gives the xWifiControlSemaphore from an interrupt service routine
 * to signal the smart config process, yielding from the ISR if a
 * higher priority task was woken.
 *
 * @param arg Not used in this function.
 */

/**
 * @brief Smart config task
 *
 * This task is an infinite loop that waits for the xWifiControlSemaphore to be given.
 * When it is given, it changes the AP by disconnecting the current connection, stopping
 * the previous smart config process, and starting a new smart config process.
 *
 * @param parm Not used in this function.
 */
static void wifi_control_task(void *parm)
{
    wifi_control_eventGroup = xEventGroupCreate();
    for (;;)
    {
        EventBits_t uxBits = xEventGroupWaitBits(
            wifi_control_eventGroup,
            WIFI_OFF_BIT | WIFI_ON_BIT,
            pdTRUE,
            pdFALSE,
            portMAX_DELAY);

        if (uxBits & WIFI_OFF_BIT)
        {
            press_to_change_AP = true;
            esp_wifi_disconnect();
            esp_smartconfig_stop();
            esp_wifi_stop();
        }
        else
        {
            if (uxBits & WIFI_ON_BIT)
            {
                ESP_ERROR_CHECK_WITHOUT_ABORT(esp_wifi_start());
                press_to_change_AP = false;
            }
        }
    }
}

static void Wifi_event_handler(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data)
{
    if (event_base == WIFI_EVENT)
    {
        switch (event_id)
        {
        case WIFI_EVENT_STA_START:
        {
            ui_wifi_setting_label_state_change(WIFI_START, "Start connecting");
            ESP_ERROR_CHECK(esp_smartconfig_start(&smart_cfg));
            break;
        }
        case WIFI_EVENT_STA_STOP:
        {
            ui_wifi_setting_label_state_change(WIFI_NOT_CONNECTED, "Not connected");
            break;
        }
        case WIFI_EVENT_STA_DISCONNECTED:
        {
            // wifi_state_display(false);
            if (is_connected == true)
            {
                is_connected = false;
                ui_update_device_icon_state(wifi_icon, false);
                char *ssid_str = (char *)malloc(128 * sizeof(char));
                sprintf(ssid_str, "Reconnecting to: %s", wifi_config.sta.ssid);
                ui_wifi_setting_label_state_change(WIFI_RECONNECT, ssid_str);
                free(ssid_str);
            }
            if (press_to_change_AP == false)
            {
                if ((change_AP == true) && ((wifi_config.sta.ssid != old_ssid) || (wifi_config.sta.password != old_password) || (wifi_config.sta.bssid != old_bssid)))
                {
                    if (attemp_count < WIFI_connect_max_attemp_number)
                    {
                        esp_wifi_connect();
                        attemp_count += 1;
                        ESP_LOGE(__func__, "Wi-Fi disconnected: Retrying connect to AP SSID:%s password:%s", wifi_config.sta.ssid, wifi_config.sta.password);
                    }
                    else
                    {
                        memcpy(wifi_config.sta.ssid, old_ssid, sizeof(old_ssid));
                        memcpy(wifi_config.sta.password, old_password, sizeof(old_password));
                        ESP_LOGI(TAG, "Reconfigurating the previous AP: SSID: %s PASSWORDS: %s", wifi_config.sta.ssid, wifi_config.sta.password);
                        memcpy(wifi_config.sta.bssid, old_bssid, sizeof(old_bssid));
                        ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
                        attemp_count += 1;
                    }
                    ESP_LOGI(TAG, "Connect to the new AP unsuccessfully");
                }
                else
                {
                    if (change_AP == true)
                    {
                        ESP_LOGI(TAG, "The new AP is same to the previous AP");
                        esp_wifi_disconnect();
                        change_AP = false;
                        memcpy(wifi_config.sta.ssid, old_ssid, sizeof(old_ssid));
                        memcpy(wifi_config.sta.password, old_password, sizeof(old_password));
                        memcpy(wifi_config.sta.bssid, old_bssid, sizeof(old_bssid));
                        ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
                    }
                }
                if ((change_AP == false) || (attemp_count > WIFI_connect_max_attemp_number))
                {
                    if (change_AP == true)
                    {
                        ESP_LOGI(TAG, "Trying to reconnect to the previous AP");
                        change_AP = false;
                    }

                    if (attemp_count_1 < WIFI_connect_max_attemp_number)
                    {
                        esp_wifi_connect();
                        attemp_count_1 += 1;
                        ESP_LOGE(TAG, "Wi-Fi disconnected: Retrying connect to AP SSID:%s password:%s", wifi_config.sta.ssid, wifi_config.sta.password);
                    }
                    else
                    {
                        ESP_LOGI(TAG, "Connect to the AP unsuccessfully. Smart Config again");
                        if (esp_smartconfig_stop() == ESP_OK)
                        {
                            esp_smartconfig_start(&smart_cfg);
                        }
                    }
                }
            }
            break;
        }
        default:
            break;
        }
    }
    else
    {
        if (event_base == IP_EVENT)
        {
            if (event_id == IP_EVENT_STA_GOT_IP)
            {
                // wifi_state_display(true);
                ESP_LOGI(TAG, "The Device got an IP successfully");
                attemp_count = 0;
                attemp_count_1 = 0;
                is_connected = true;
                memcpy(old_ssid, wifi_config.sta.ssid, sizeof(wifi_config.sta.ssid));
                memcpy(old_password, wifi_config.sta.password, sizeof(wifi_config.sta.password));
                memcpy(old_bssid, wifi_config.sta.bssid, sizeof(wifi_config.sta.bssid));
                wifi_ap_record_t ap_info;
                esp_wifi_sta_get_ap_info(&ap_info);
                if (lv_obj_is_valid(ui_continueLabel) == true)
                {
                    start_continue_Animation(ui_continueLabel, 0);
                }
                char *ssid_str = (char *)malloc(128 * sizeof(char));
                sprintf(ssid_str, "Connected to: %s", wifi_config.sta.ssid);
                ui_wifi_setting_label_state_change(WIFI_GOT_IP, ssid_str);
                free(ssid_str);
                ESP_LOGI(TAG, "AP MAC ADDRESS: %02X:%02X:%02X:%02X:%02X:%02X", ap_info.bssid[0], ap_info.bssid[1], ap_info.bssid[2], ap_info.bssid[3], ap_info.bssid[4], ap_info.bssid[5]);
                ui_update_device_icon_state(wifi_icon, true);

                //_ui_flag_modify(ui_error_notify, LV_OBJ_FLAG_HIDDEN, _UI_MODIFY_FLAG_REMOVE);
            }
        }
        else
        {
            if (event_base == SC_EVENT)
            {
                switch (event_id)
                {
                case SC_EVENT_SCAN_DONE:
                {
                    ui_wifi_setting_label_state_change(WIFI_SCAN_DONE, "Finding Access Point...");
                    ESP_LOGI(SC_TAG, "Scan done");
                    break;
                }
                case SC_EVENT_FOUND_CHANNEL:
                {
                    attemp_count = 0;
                    attemp_count_1 = 0;
                    change_AP = true;
                    ui_wifi_setting_label_state_change(WIFI_FOUND_CHANNEL, "Found Access Point");
                    ESP_LOGI(SC_TAG, "Found channel");
                    break;
                }
                case SC_EVENT_GOT_SSID_PSWD:
                {
                    ESP_LOGI(SC_TAG, "Got SSID and password");

                    smartconfig_event_got_ssid_pswd_t *evt = (smartconfig_event_got_ssid_pswd_t *)event_data;

                    uint8_t ssid[33] = {0};
                    uint8_t password[65] = {0};
                    uint8_t bssid[6] = {0};
                    uint8_t rvd_data[33] = {0};

                    bzero(&wifi_config, sizeof(wifi_config_t));
                    memcpy(wifi_config.sta.ssid, evt->ssid, sizeof(wifi_config.sta.ssid));
                    memcpy(wifi_config.sta.password, evt->password, sizeof(wifi_config.sta.password));
                    wifi_config.sta.bssid_set = evt->bssid_set;
                    if (wifi_config.sta.bssid_set == true)
                    {
                        memcpy(wifi_config.sta.bssid, evt->bssid, sizeof(wifi_config.sta.bssid));
                        memcpy(bssid, evt->bssid, sizeof(evt->bssid));
                        ESP_LOGI(SC_TAG, "AP MAC ADDRESS: %02X:%02X:%02X:%02X:%02X:%02X", bssid[0], bssid[1], bssid[2], bssid[3], bssid[4], bssid[5]);
                    }

                    memcpy(ssid, evt->ssid, sizeof(evt->ssid));
                    memcpy(password, evt->password, sizeof(evt->password));

                    ESP_LOGI(SC_TAG, "SSID:%s", ssid);
                    ESP_LOGI(SC_TAG, "PASSWORD:%s", password);
                    char *ssid_str = (char *)malloc(128 * sizeof(char));
                    sprintf(ssid_str, "Connecting to: %s", ssid);
                    ui_wifi_setting_label_state_change(WIFI_GOT_SSID_PWD, ssid_str);
                    free(ssid_str);
                    if (evt->type == SC_TYPE_ESPTOUCH_V2)
                    {
                        ESP_ERROR_CHECK(esp_smartconfig_get_rvd_data(rvd_data, sizeof(rvd_data)));
                        ESP_LOGI(SC_TAG, "RVD_DATA:");
                        for (int i = 0; i < 33; i++)
                        {
                            printf("%02x ", rvd_data[i]);
                        }
                        printf("\n");
                    }

                    ESP_ERROR_CHECK(esp_wifi_disconnect());
                    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
                    esp_wifi_connect();
                    break;
                }
                case SC_EVENT_SEND_ACK_DONE:
                {
                    esp_smartconfig_stop();
                    change_AP = false;

                    break;
                }
                default:
                    break;
                }
            }
        }
    }
}

static void WIFI_init(void)
{
    esp_netif_t *sta_netif = esp_netif_create_default_wifi_sta();
    assert(sta_netif);

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK_WITHOUT_ABORT(esp_wifi_init(&cfg));

    esp_event_handler_instance_t instance_any_id_Wifi;
    esp_event_handler_instance_t instance_got_ip;
    esp_event_handler_instance_t instance_any_id_SmartConfig;

    ESP_ERROR_CHECK_WITHOUT_ABORT(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &Wifi_event_handler, &instance_any_id_Wifi));
    ESP_ERROR_CHECK_WITHOUT_ABORT(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &Wifi_event_handler, &instance_got_ip));
    ESP_ERROR_CHECK_WITHOUT_ABORT(esp_event_handler_register(SC_EVENT, ESP_EVENT_ANY_ID, &Wifi_event_handler, &instance_any_id_SmartConfig));

    ESP_ERROR_CHECK_WITHOUT_ABORT(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK_WITHOUT_ABORT(esp_smartconfig_set_type(SC_TYPE_ESPTOUCH));
    // ESP_ERROR_CHECK_WITHOUT_ABORT(esp_wifi_start());
}
/*---------------------------------------------------------------------------- */
/*-------------------------------------SD CARD--------------------------------------- */

/**
 * @brief Scanning base directory mounted to File system to take all .bmp image names.
 *
 * @param[in] base_path Base directory.
 * @return esp_err_t
 *
 * @retval  - ESP_OK on success.
 * @retval  - ESP_FAIL on fail.
 */
esp_err_t scan_bmp_images(const char *base_path)
{
    DIR *dir = opendir(base_path);
    if (!dir)
    {
        ESP_LOGE(SD_TAG, "Failed to open directory: %s\n to scan", base_path);
        return ESP_FAIL;
    }
    struct dirent *entry;
    char *full_path = (char *)malloc(320 * sizeof(char));
    while ((entry = readdir(dir)) != NULL)
    {
        sprintf(full_path, "%s/%s", base_path, entry->d_name);
        struct stat st;
        if (stat(full_path, &st) == 0 && S_ISREG(st.st_mode))
        {
            char *ext = strrchr(entry->d_name, '.');
            if ((ext != NULL) && (strcmp(ext, ".BMP") == 0))
            {
                *ext = '\0';
                image_index += 1;
                image_count++;
                strcpy(list_img[image_index], entry->d_name);
            }
        }
        memset(full_path, 0, strlen(full_path));
    }
    free(full_path);
    ESP_LOGI(SD_TAG, "Scaning file system to find .bmp images successfully. %" PRIu16 " image found", image_count);
    closedir(dir);
    return ESP_OK;
}
void readSenorChamberTemperature_task(void *parameters)
{
    int i = 0;
    char str[20] = {0};
    for (;;)
    {
        sprintf(str, "%d", i);
        _ui_label_set_property(ui_tempValue, _UI_LABEL_PROPERTY_TEXT, str);
        i++;
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
}

// nvs flash init
static void initialize_nvs(void)
{
    esp_err_t error = nvs_flash_init();
    if (error == ESP_ERR_NVS_NO_FREE_PAGES || error == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK_WITHOUT_ABORT(nvs_flash_erase());
        error = nvs_flash_init();
    }
    ESP_ERROR_CHECK_WITHOUT_ABORT(error);
}

void app_main(void)
{

    initialize_nvs(); // nvs flash init
    ESP_ERROR_CHECK_WITHOUT_ABORT(esp_netif_init());
    ESP_ERROR_CHECK_WITHOUT_ABORT(esp_event_loop_create_default());

    // Initialize SPI bus and mount SD card to file system
    ESP_LOGI(__func__, "Initialize SD card with SPI interface.");
    esp_vfs_fat_mount_config_t mount_config_t = MOUNT_CONFIG_DEFAULT();
    spi_bus_config_t spi_bus_config_t_1 = SPI_BUS_CONFIG_DEFAULT();
    sdmmc_host_t host_t = SDSPI_HOST_DEFAULT();
    sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
    slot_config.gpio_cs = CONFIG_SD_PIN_NUM_CS;
    slot_config.host_id = host_t.slot;
    sdmmc_card_t SDCARD;
    esp_err_t sd_err = sdcard_initialize(&mount_config_t, &SDCARD, &host_t, &spi_bus_config_t_1, &slot_config);

    // scan_bmp_images(mount_point);

    // Initialize LCD TFT (LVGL drivers)
#if CONFIG_USING_LCD_TFT
    ili9341_init();
    xTaskCreate(&lvgl_timer_handle_task, "LVGL timer handle task", 10 * 1024, NULL, 5, NULL);
    ui_init();
#endif // CONFIG_USING_LCD_TFT
    if (sd_err == ESP_OK)
    {
        ui_update_device_icon_state(sdcard_icon, true);
    }

    // xTaskCreate(&image_show_handler, "Image display task", 10 * 1024, NULL, 10, NULL);
    // vTaskDelay(1000 / portTICK_PERIOD_MS);

    ESP_ERROR_CHECK_WITHOUT_ABORT(sd_err);
    xTaskCreate(&wifi_control_task, "wifi control task", 10 * 1024, NULL, 10, NULL);
    xTaskCreate(&readSenorChamberTemperature_task, "temperature monitor task", 10 * 1024, NULL, 11, NULL);
    WIFI_init(); // Initialize Wifi in station mode with Smart Config
}
