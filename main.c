#include "lwip/apps/httpd.h"
#include "lwip/tcp.h"
#include "lwip/netif.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "lwipopts.h"
#include "ssi.h"
#include <stdio.h>
#include "main.h"
#include "hardware/adc.h"
#include "hardware/uart.h"
#include "FreeRTOS.h"
#include "task.h"

const char WIFI_SSID[] = "";
const char WIFI_PASSWORD[] = "";
bool smoke_alarm = 0;
bool gas_alarm = 0;
const uint LED_PIN = 25U;

TaskHandle_t gasTaskHandle;
TaskHandle_t smokeTaskHandle;
TaskHandle_t lcdTaskHandle;
TaskHandle_t alertTaskHandle;
TaskHandle_t serverTaskHandle;

void init_lcd()
{
    sleep_ms(500);
    gpio_init(RS_PIN);
    gpio_set_dir(RS_PIN, GPIO_OUT);
    gpio_init(E_PIN);
    gpio_set_dir(E_PIN, GPIO_OUT);
    gpio_init(D4_PIN);
    gpio_set_dir(D4_PIN, GPIO_OUT);
    gpio_init(D5_PIN);
    gpio_set_dir(D5_PIN, GPIO_OUT);
    gpio_init(D6_PIN);
    gpio_set_dir(D6_PIN, GPIO_OUT);
    gpio_init(D7_PIN);
    gpio_set_dir(D7_PIN, GPIO_OUT);

    gpio_put(RS_PIN, 0);
    gpio_put(D4_PIN, 0);
    gpio_put(D5_PIN, 1);
    gpio_put(D6_PIN, 0);
    gpio_put(D7_PIN, 0);

    gpio_put(E_PIN, 1);
    sleep_us(350);
    gpio_put(E_PIN, 0);
    sleep_us(350);

    gpio_put(D5_PIN, 0);

    gpio_put(E_PIN, 1);
    sleep_us(350);
    gpio_put(E_PIN, 0);
    sleep_us(350);

    gpio_put(D4_PIN, 1);

    gpio_put(E_PIN, 1);
    sleep_us(350);
    gpio_put(E_PIN, 0);
    sleep_us(350);

    gpio_put(D4_PIN, 0);

    gpio_put(E_PIN, 1);
    sleep_us(350);
    gpio_put(E_PIN, 0);
    sleep_us(350);

    gpio_put(D5_PIN, 1);

    gpio_put(E_PIN, 1);
    sleep_us(350);
    gpio_put(E_PIN, 0);
    sleep_us(350);

    gpio_put(D5_PIN, 0);

    gpio_put(E_PIN, 1);
    sleep_us(350);
    gpio_put(E_PIN, 0);
    sleep_us(350);

    gpio_put(D6_PIN, 1);
    gpio_put(D7_PIN, 1);

    gpio_put(E_PIN, 1);
    sleep_us(350);
    gpio_put(E_PIN, 0);
    sleep_us(350);

    // writing
    gpio_put(RS_PIN, 1);

    // l
    gpio_put(D7_PIN, 0);

    gpio_put(E_PIN, 1);
    sleep_us(350);
    gpio_put(E_PIN, 0);
    sleep_us(350);

    gpio_put(D7_PIN, 1);
    gpio_put(D6_PIN, 1);

    gpio_put(E_PIN, 1);
    sleep_us(350);
    gpio_put(E_PIN, 0);
    sleep_us(350);
}

void clear_lcd()
{
    gpio_put(D4_PIN, 0);
    gpio_put(D6_PIN, 0);
    gpio_put(D5_PIN, 0);
    gpio_put(D7_PIN, 0);
    gpio_put(RS_PIN, 0);

    gpio_put(E_PIN, 1);
    vTaskDelay(2);
    gpio_put(E_PIN, 0);
    vTaskDelay(2);

    gpio_put(D4_PIN, 1);

    gpio_put(E_PIN, 1);
    vTaskDelay(2);
    gpio_put(E_PIN, 0);
    vTaskDelay(2);

    // home
    gpio_put(D4_PIN, 0);

    gpio_put(E_PIN, 1);
    vTaskDelay(2);
    gpio_put(E_PIN, 0);
    vTaskDelay(2);

    gpio_put(D5_PIN, 1);

    gpio_put(E_PIN, 1);
    vTaskDelay(2);
    gpio_put(E_PIN, 0);
    vTaskDelay(2);

    gpio_put(RS_PIN, 1);
}

void number_lcd(int num)
{
    int x = num;
    int c = 0;
    while (x != 0)
    {
        ++c;
        x /= 10;
    }
    int v = c;
    int j[4]; // c
    while (num != 0)
    {
        j[--c] = num % 10;
        num /= 10;
    }
    for (int i = 0; i < v; i++)
    {
        if (j[i] == 0)
        {
            gpio_put(D7_PIN, 0);
            gpio_put(D6_PIN, 0);
            gpio_put(D5_PIN, 1);
            gpio_put(D4_PIN, 1);

            gpio_put(E_PIN, 1);
            sleep_us(340);
            gpio_put(E_PIN, 0);
            sleep_us(340);

            gpio_put(D5_PIN, 0);
            gpio_put(D4_PIN, 0);

            gpio_put(E_PIN, 1);
            sleep_us(340);
            gpio_put(E_PIN, 0);
            sleep_us(340);
        }
        else if (j[i] == 1)
        {
            gpio_put(D7_PIN, 0);
            gpio_put(D6_PIN, 0);
            gpio_put(D5_PIN, 1);
            gpio_put(D4_PIN, 1);

            gpio_put(E_PIN, 1);
            sleep_us(340);
            gpio_put(E_PIN, 0);
            sleep_us(340);

            gpio_put(D5_PIN, 0);

            gpio_put(E_PIN, 1);
            sleep_us(340);
            gpio_put(E_PIN, 0);
            sleep_us(340);
        }
        else if (j[i] == 2)
        {
            gpio_put(D7_PIN, 0);
            gpio_put(D6_PIN, 0);
            gpio_put(D5_PIN, 1);
            gpio_put(D4_PIN, 1);

            gpio_put(E_PIN, 1);
            sleep_us(340);
            gpio_put(E_PIN, 0);
            sleep_us(340);

            gpio_put(D4_PIN, 0);

            gpio_put(E_PIN, 1);
            sleep_us(340);
            gpio_put(E_PIN, 0);
            sleep_us(340);
        }
        else if (j[i] == 3)
        {
            gpio_put(D7_PIN, 0);
            gpio_put(D6_PIN, 0);
            gpio_put(D5_PIN, 1);
            gpio_put(D4_PIN, 1);

            gpio_put(E_PIN, 1);
            sleep_us(340);
            gpio_put(E_PIN, 0);
            sleep_us(340);

            gpio_put(E_PIN, 1);
            sleep_us(340);
            gpio_put(E_PIN, 0);
            sleep_us(340);
        }
        else if (j[i] == 4)
        {
            gpio_put(D7_PIN, 0);
            gpio_put(D6_PIN, 0);
            gpio_put(D5_PIN, 1);
            gpio_put(D4_PIN, 1);

            gpio_put(E_PIN, 1);
            sleep_us(340);
            gpio_put(E_PIN, 0);
            sleep_us(340);

            gpio_put(D6_PIN, 1);
            gpio_put(D5_PIN, 0);
            gpio_put(D4_PIN, 0);

            gpio_put(E_PIN, 1);
            sleep_us(340);
            gpio_put(E_PIN, 0);
            sleep_us(340);
        }
        else if (j[i] == 5)
        {
            gpio_put(D7_PIN, 0);
            gpio_put(D6_PIN, 0);
            gpio_put(D5_PIN, 1);
            gpio_put(D4_PIN, 1);

            gpio_put(E_PIN, 1);
            sleep_us(340);
            gpio_put(E_PIN, 0);
            sleep_us(340);

            gpio_put(D6_PIN, 1);
            gpio_put(D5_PIN, 0);

            gpio_put(E_PIN, 1);
            sleep_us(340);
            gpio_put(E_PIN, 0);
            sleep_us(340);
        }
        else if (j[i] == 6)
        {
            gpio_put(D7_PIN, 0);
            gpio_put(D6_PIN, 0);
            gpio_put(D5_PIN, 1);
            gpio_put(D4_PIN, 1);

            gpio_put(E_PIN, 1);
            sleep_us(340);
            gpio_put(E_PIN, 0);
            sleep_us(340);

            gpio_put(D6_PIN, 1);
            gpio_put(D4_PIN, 0);

            gpio_put(E_PIN, 1);
            sleep_us(340);
            gpio_put(E_PIN, 0);
            sleep_us(340);
        }
        else if (j[i] == 7)
        {
            gpio_put(D7_PIN, 0);
            gpio_put(D6_PIN, 0);
            gpio_put(D5_PIN, 1);
            gpio_put(D4_PIN, 1);

            gpio_put(E_PIN, 1);
            sleep_us(340);
            gpio_put(E_PIN, 0);
            sleep_us(340);

            gpio_put(D6_PIN, 1);

            gpio_put(E_PIN, 1);
            sleep_us(340);
            gpio_put(E_PIN, 0);
            sleep_us(340);
        }
        else if (j[i] == 8)
        {
            gpio_put(D7_PIN, 0);
            gpio_put(D6_PIN, 0);
            gpio_put(D5_PIN, 1);
            gpio_put(D4_PIN, 1);

            gpio_put(E_PIN, 1);
            sleep_us(340);
            gpio_put(E_PIN, 0);
            sleep_us(340);

            gpio_put(D7_PIN, 1);
            gpio_put(D5_PIN, 0);
            gpio_put(D4_PIN, 0);

            gpio_put(E_PIN, 1);
            sleep_us(340);
            gpio_put(E_PIN, 0);
            sleep_us(340);
        }
        else if (j[i] == 9)
        {
            gpio_put(D7_PIN, 0);
            gpio_put(D6_PIN, 0);
            gpio_put(D5_PIN, 1);
            gpio_put(D4_PIN, 1);

            gpio_put(E_PIN, 1);
            sleep_us(340);
            gpio_put(E_PIN, 0);
            sleep_us(340);

            gpio_put(D7_PIN, 1);
            gpio_put(D5_PIN, 0);

            gpio_put(E_PIN, 1);
            sleep_us(340);
            gpio_put(E_PIN, 0);
            sleep_us(340);
        }
    }
}

void write_lcd(int smk, int gas, bool lig)
{
    clear_lcd();
    // S
    gpio_put(D7_PIN, 0);
    gpio_put(D6_PIN, 1);
    gpio_put(D5_PIN, 0);
    gpio_put(D4_PIN, 1);

    gpio_put(E_PIN, 1);
    sleep_us(340);
    gpio_put(E_PIN, 0);
    sleep_us(340);

    gpio_put(D6_PIN, 0);
    gpio_put(D5_PIN, 1);

    gpio_put(E_PIN, 1);
    sleep_us(340);
    gpio_put(E_PIN, 0);
    sleep_us(340);

    // :
    gpio_put(D7_PIN, 0);
    gpio_put(D6_PIN, 0);
    gpio_put(D5_PIN, 1);
    gpio_put(D4_PIN, 1);

    gpio_put(E_PIN, 1);
    sleep_us(340);
    gpio_put(E_PIN, 0);
    sleep_us(340);

    gpio_put(D7_PIN, 1);
    gpio_put(D4_PIN, 0);

    gpio_put(E_PIN, 1);
    sleep_us(340);
    gpio_put(E_PIN, 0);
    sleep_us(340);

    // smoke value
    number_lcd(smk);

    // space
    gpio_put(D7_PIN, 0);
    gpio_put(D6_PIN, 0);
    gpio_put(D5_PIN, 1);
    gpio_put(D4_PIN, 0);

    gpio_put(E_PIN, 1);
    sleep_us(340);
    gpio_put(E_PIN, 0);
    sleep_us(340);

    gpio_put(D5_PIN, 0);

    gpio_put(E_PIN, 1);
    sleep_us(340);
    gpio_put(E_PIN, 0);
    sleep_us(340);

    // G

    gpio_put(D6_PIN, 1);

    gpio_put(E_PIN, 1);
    sleep_us(340);
    gpio_put(E_PIN, 0);
    sleep_us(340);

    gpio_put(D5_PIN, 1);
    gpio_put(D4_PIN, 1);

    gpio_put(E_PIN, 1);
    sleep_us(340);
    gpio_put(E_PIN, 0);
    sleep_us(340);

    // :
    gpio_put(D7_PIN, 0);
    gpio_put(D6_PIN, 0);
    gpio_put(D5_PIN, 1);
    gpio_put(D4_PIN, 1);

    gpio_put(E_PIN, 1);
    sleep_us(340);
    gpio_put(E_PIN, 0);
    sleep_us(340);

    gpio_put(D7_PIN, 1);
    gpio_put(D4_PIN, 0);

    gpio_put(E_PIN, 1);
    sleep_us(340);
    gpio_put(E_PIN, 0);
    sleep_us(340);

    // gas value
    number_lcd(gas);

    // space
    gpio_put(D7_PIN, 0);
    gpio_put(D6_PIN, 0);
    gpio_put(D5_PIN, 1);
    gpio_put(D4_PIN, 0);

    gpio_put(E_PIN, 1);
    sleep_us(340);
    gpio_put(E_PIN, 0);
    sleep_us(340);

    gpio_put(D5_PIN, 0);

    gpio_put(E_PIN, 1);
    sleep_us(340);
    gpio_put(E_PIN, 0);
    sleep_us(340);

    // L
    gpio_put(D7_PIN, 0);
    gpio_put(D6_PIN, 1);
    gpio_put(D5_PIN, 0);
    gpio_put(D4_PIN, 0);

    gpio_put(E_PIN, 1);
    sleep_us(340);
    gpio_put(E_PIN, 0);
    sleep_us(340);

    gpio_put(D7_PIN, 1);

    gpio_put(E_PIN, 1);
    sleep_us(340);
    gpio_put(E_PIN, 0);
    sleep_us(340);

    // :
    gpio_put(D7_PIN, 0);
    gpio_put(D6_PIN, 0);
    gpio_put(D5_PIN, 1);
    gpio_put(D4_PIN, 1);

    gpio_put(E_PIN, 1);
    sleep_us(340);
    gpio_put(E_PIN, 0);
    sleep_us(340);

    gpio_put(D7_PIN, 1);
    gpio_put(D4_PIN, 0);

    gpio_put(E_PIN, 1);
    sleep_us(340);
    gpio_put(E_PIN, 0);
    sleep_us(340);

    // light value
    if (lig)
    {
        gpio_put(D7_PIN, 0);
        gpio_put(D6_PIN, 0);
        gpio_put(D5_PIN, 1);
        gpio_put(D4_PIN, 1);

        gpio_put(E_PIN, 1);
        sleep_us(340);
        gpio_put(E_PIN, 0);
        sleep_us(340);

        gpio_put(D5_PIN, 0);

        gpio_put(E_PIN, 1);
        sleep_us(340);
        gpio_put(E_PIN, 0);
        sleep_us(340);
    }
    else
    {
        gpio_put(D7_PIN, 0);
        gpio_put(D6_PIN, 0);
        gpio_put(D5_PIN, 1);
        gpio_put(D4_PIN, 1);

        gpio_put(E_PIN, 1);
        sleep_us(340);
        gpio_put(E_PIN, 0);
        sleep_us(340);

        gpio_put(D5_PIN, 0);
        gpio_put(D4_PIN, 0);

        gpio_put(E_PIN, 1);
        sleep_us(340);
        gpio_put(E_PIN, 0);
        sleep_us(340);
    }
}

void init_hardware(void)
{
    stdio_init_all();
    adc_gpio_init(SMOKE_ADC_PIN);
    adc_gpio_init(GAS_ADC_PIN);
    gpio_init(LIGHT_SENSOR_PIN);
    gpio_set_dir(LIGHT_SENSOR_PIN, GPIO_IN);
    gpio_init(ALERT_PIN);
    gpio_set_dir(ALERT_PIN, GPIO_OUT);
    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    init_lcd();
    printf("Hardware Initialized for Smoke Sensor Test\n");
}

uint16_t read_smoke_sensor(void)
{
    adc_select_input(0); // Channel 0 for SMOKE_ADC_PIN
    sleep_us(10);        // Stabilize ADC
    return adc_read();
}

uint16_t read_gas_sensor(void)
{
    adc_select_input(2);
    sleep_us(10);
    return adc_read();
}

void server_task(void *pvParameters)
{
    // Connect to the WiFI network - loop until connected
    printf("aaa");
    while (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 30000) != 0)
    {
        printf("Attempting to connect...\n");
    }
    // Print a success message once connected
    printf("Connected! \n");

    // Initialise web server
    httpd_init();
    printf("Http server initialised\n");

    // Configure SSI and CGI handler
    ssi_init();
    printf("SSI Handler initialised\n");
    cgi_init();
    printf("CGI Handler initialised\n");

    gpio_put(LED_PIN, 1);

    while (1)
    {
        //
    }
}

void smoke_task(void *pvParameters)
{
    while (1)
    {
        if (read_smoke_sensor() > SMOKE_SENSITIVITY)
        {
            smoke_alarm = 1;
        }
        else
        {
            smoke_alarm = 0;
        }
    }
}

void gas_task(void *pvParameters)
{
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xFrequency = pdMS_TO_TICKS(500);

    while (1)
    {
        if (read_gas_sensor() > GAS_SENSITIVITY)
        {
            gas_alarm = 1;
        }
        else
        {
            gas_alarm = 0;
        }
        xTaskNotifyGive(alertTaskHandle);
        xTaskNotifyGive(lcdTaskHandle);
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}

void alert_task(void *pvParameters)
{
    while (1)
    {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        if (gas_alarm || smoke_alarm && !gpio_get(LIGHT_SENSOR_PIN))
        {
            gpio_put(ALERT_PIN, 1);
        }
        else
        {
            gpio_put(ALERT_PIN, 0);
        }
    }
}

void lcd_task(void *pvParameters)
{
    while (1)
    {
        vTaskDelay(500);
        write_lcd(read_smoke_sensor(), read_gas_sensor(), gpio_get(LIGHT_SENSOR_PIN));
        printf("Light: %d\n", gpio_get(LIGHT_SENSOR_PIN));
        printf("Gas: %d\n", read_gas_sensor());
        printf("Smoke: %d\n", read_smoke_sensor());
    }
}

int main()
{
    adc_init();

    init_hardware();

    cyw43_arch_init();

    cyw43_arch_enable_sta_mode();

    xTaskCreate(server_task, "Server_Task", 1024, NULL, SERVER_TASK_PRIORITY, &serverTaskHandle); // 256
    xTaskCreate(gas_task, "Gas_Task", 1024, NULL, GAS_TASK_PRIORITY, &gasTaskHandle);
    xTaskCreate(smoke_task, "Smoke_Task", 1024, NULL, SMOKE_TASK_PRIORITY, &smokeTaskHandle);
    xTaskCreate(alert_task, "Alert_Task", 1024, NULL, ALERT_TASK_PRIORITY, &alertTaskHandle);
    xTaskCreate(lcd_task, "LCD_Task", 1024, NULL, LCD_TASK_PRIORITY, &lcdTaskHandle);

    vTaskStartScheduler();

    // Infinite loop
    while (true)
    {
    }
}