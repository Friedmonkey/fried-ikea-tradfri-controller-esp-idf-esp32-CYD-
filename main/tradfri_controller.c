#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include "esp_log.h"

#include "fried_tradfri_client.h"
#include "fdata.h"

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


static void controller_main(void *p)
{
    //fdata data = f_create();
    //f_set_str(data, "key", "value");
    //char* json = f_serialize(data);
    const char* deviceId = "65574";
    char* info = GetTradfriDeviceInfo(deviceId);
    printf("info :\n%s\n", info);

    vTaskDelay(pdMS_TO_TICKS(1000));
    CycleColorsXY(deviceId);

    ESP_LOGI(TAG, "Finished");
    vTaskDelete(NULL);
}

void app_main(void)
{
    tradfri_init("192.168.2.9", "tradfri_12345", "IaY5AQRXw1awfqEt");

    xTaskCreate(controller_main, "controller", 8 * 1024, NULL, 5, NULL);
}
