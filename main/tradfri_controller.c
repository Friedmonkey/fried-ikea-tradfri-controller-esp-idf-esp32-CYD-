#include "fried_coaps_client.h"
#include "fdata.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include "esp_log.h"

static char* join_url(const char* base, const char* sub) {
    size_t len = strlen(base) + strlen(sub) + 1;
    char* out = (char*)malloc(len);
    if (!out) return NULL;
    strcpy(out, base);
    strcat(out, sub);
    return out;
}

static char* Tradfri_send(coap_pdu_code_t method, bool wait, const char* suburl, const char* data, size_t data_len)
{
    const char* baseurl = "coaps://192.168.2.9:5684";
    char* fullurl = join_url(baseurl, suburl);
    if (!fullurl) return NULL;

    printf("%s\n", fullurl);
    if (data)
        printf("data: %s\n", data);
    char* result = CoAPsSend(method, wait, fullurl, "tradfri_12345", "IaY5AQRXw1awfqEt", data, data_len);
    free(fullurl);
    return result;
}
static char* Tradfri_GET(const char* suburl)
{
    return Tradfri_send(COAP_REQUEST_CODE_GET, true, suburl, NULL, 0);
}
static char* Tradfri_PUT(const char* suburl, const char* data)
{
    size_t len = data ? strlen(data) : 0;
    return Tradfri_send(COAP_REQUEST_CODE_PUT, false, suburl, data, len);
}

static char* GetTradfriDevices()
{
    return Tradfri_GET("/15001");
}

static char* GetTradfriDeviceInfo(const char* deviceId)
{
    const char* basepath = "/15001/";
    char* path = join_url(basepath, deviceId);
    if (!path) return NULL;

    char* result = Tradfri_GET(path);
    free(path);
    return result;
}

static void UpdateTradfriDeviceInfo(const char* deviceId, fdata json)
{
    const char* basepath = "/15001/";
    char* path = join_url(basepath, deviceId);
    if (!path) return;

    char* json_str = f_serialize(json);
    if (!json_str) {
        free(path);
        return;
    }

    Tradfri_PUT(path, json_str);

    free(json_str);
    free(path);
}

static void SetBrightnessTradfriLamp(const char* deviceId, uint8_t brightness)
{
    fdata obj = f_create();
    f_set_int(obj, "5851", brightness);

    fdata json = f_create();
    f_set_arr(json, "3311", &obj, 1); // single object array

    UpdateTradfriDeviceInfo(deviceId, json);

    f_free(json); // frees nested objects too
}
static void SetColorTradfriLamp(const char* deviceId, uint16_t x, uint16_t y)
{
    fdata obj = f_create();
    //f_set_int(obj, "5850", 1);   // power on
    //f_set_int(obj, "5851", 254); // brightness (max)
    f_set_int(obj, "5709", x);   // X color coordinate
    f_set_int(obj, "5710", y);   // Y color coordinate
    f_set_int(obj, "5712", 20);  // 2-second transition

    fdata json = f_create();
    f_set_arr(json, "3311", &obj, 1);

    UpdateTradfriDeviceInfo(deviceId, json);
    f_free(json);
}

static void CycleColorsXY(const char* deviceId)
{
    struct { uint16_t x, y; } colors[] = {
        { 45860, 19660 }, // red
        { 19660, 39321 }, // green
        { 9830,  6554  }, // blue
        { 32768, 32768 }  // white
    };

    const int color_count = sizeof(colors) / sizeof(colors[0]);

    while (1)
    {
        for (int i = 0; i < color_count; i++)
        {
            SetColorTradfriLamp(deviceId, colors[i].x, colors[i].y);
            printf("Set XY to %u %u\n", colors[i].x, colors[i].y);
            vTaskDelay(pdMS_TO_TICKS(3000));
        }
    }
}




static void coap_example_client(void *p)
{
    //fdata data = f_create();
    //f_set_str(data, "key", "value");
    //char* json = f_serialize(data);
    const char* deviceId = "65574";
    char* info = GetTradfriDeviceInfo(deviceId);
    printf("info :\n%s\n", info);

    vTaskDelay(pdMS_TO_TICKS(1000));


    //uint8_t brightness = 0;
    //while(1)
    //{
    //    printf("new request\n");
    //    SetBrightnessTradfriLamp(deviceId, brightness);
    //    printf("request done!\n\n");

    //    brightness += 25;

    //    vTaskDelay(pdMS_TO_TICKS(500)); // sleep for 500 ms (half a second)


    //    //for(int countdown = 3; countdown >= 0; countdown--) {
    //    //    ESP_LOGI(TAG, "%d... ", countdown);
    //    //}
    //    //ESP_LOGI(TAG, "Starting again!");
    //}


    CycleColorsXY(deviceId);

    ESP_LOGI(TAG, "Finished");
    vTaskDelete(NULL);
}

void app_main(void)
{
    coaps_init();

    xTaskCreate(coap_example_client, "coap", 8 * 1024, NULL, 5, NULL);
}
