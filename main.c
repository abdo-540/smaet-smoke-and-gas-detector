#include "lwip/apps/httpd.h"
// #include "lwip/netdb.h"
// #include "lwip/api.h"
#include "lwip/tcp.h"
#include "lwip/netif.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include "lwipopts.h"
#include "ssi.h"
#include "cgi.h"
#include <stdio.h>
#include "main.h"
#include "hardware/adc.h"
#include "hardware/uart.h"
#include "FreeRTOS.h"
#include "task.h"

/*
#include "lwip/dns.h"       // Hostname resolution
#include "lwip/altcp_tls.h" // TCP + TLS (+ HTTP == HTTPS)
#include "altcp_tls_mbedtls_structs.h"
#include "lwip/prot/iana.h" // HTTPS port number

#include "mbedtls/ssl.h" // Server Name Indication TLS extension
#ifdef MBEDTLS_DEBUG_C
#include "mbedtls/debug.h" // Mbed TLS debugging
#endif                     // MBEDTLS_DEBUG_C
#include "mbedtls/check_config.h"
*/
// #include "picohttps.h"

const char WIFI_SSID[] = "WE8EC2DF";     // iPhone
const char WIFI_PASSWORD[] = "kb387544"; // basant2003
bool smoke_alarm = 0;
bool gas_alarm = 0;
const uint LED_PIN = 25U;

TaskHandle_t gasTaskHandle;
TaskHandle_t smokeTaskHandle;
TaskHandle_t lcdTaskHandle;
TaskHandle_t alertTaskHandle;
TaskHandle_t serverTaskHandle;

// char message[] = "";

/*bool getIPAddress(uint8_t *ip)
{
    memcpy(ip, netif_ip4_addr(&cyw43_state.netif[0]), 4);
    return true;
}

bool getIPAddressStr(char *ips)
{
    char *s = ipaddr_ntoa(netif_ip4_addr(&cyw43_state.netif[0]));
    strcpy(ips, s);
    return true;
}*/

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
    // gpio_put(D6_PIN, 1); //
    // gpio_put(D5_PIN, 0);
    // gpio_put(D4_PIN, 0);

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

    // u
    // gpio_put(D7_PIN, 0);
    // gpio_put(D6_PIN, 1); //
    /*gpio_put(D5_PIN, 0);
    gpio_put(D4_PIN, 1);

    gpio_put(E_PIN, 1);
    sleep_us(350);
    gpio_put(E_PIN, 0);
    sleep_us(350);

    gpio_put(E_PIN, 1);
    sleep_us(350);
    gpio_put(E_PIN, 0);
    sleep_us(350);*/
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
    /*int a[10] = {0}, rem, i;
    while (num)
    {
        rem = num % 10;
        a[rem] = a[rem] + 1;
        num = num / 10;
    }

    for (i = 0; i < 10; i++)
    {
        if (a[i] != 0)
        {
        }
    }*/

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
    /*
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
        sleep_us(340);*/

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
    /*
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
        sleep_us(340);*/

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
    /*
        // space
        gpio_put(D7_PIN, 0);
        gpio_put(D6_PIN, 0);
        gpio_put(D5_PIN, 1);
        gpio_put(D4_PIN, 0);

        gpio_put(E_PIN, 1);
        sleep_us(350);
        gpio_put(E_PIN, 0);
        sleep_us(350);

        gpio_put(D5_PIN, 0);

        gpio_put(E_PIN, 1);
        sleep_us(350);
        gpio_put(E_PIN, 0);
        sleep_us(350);*/

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
    // vTaskDelay(pdMS_TO_TICKS(1));
    return adc_read();
}

uint16_t read_gas_sensor(void)
{
    adc_select_input(2);
    sleep_us(10);
    // vTaskDelay(pdMS_TO_TICKS(1));
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

    /*char ipStr[20];
getIPAddressStr(ipStr);
printf("Listening on: %d\n", ipStr);*/

    gpio_put(LED_PIN, 1);
    /*while (1)
    {
        // ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        // code
    }*/
    /*

    //

    // Resolve server hostname
    ip_addr_t ipaddr;
    char *char_ipaddr;
    printf("Resolving %s\n", PICOHTTPS_HOSTNAME);
    if (!resolve_hostname(&ipaddr))
    {
        printf("Failed to resolve %s\n", PICOHTTPS_HOSTNAME);
        // TODO: Disconnect from network
        cyw43_arch_deinit(); // Deinit Pico W wireless hardware
        return;
    }
    cyw43_arch_lwip_begin();
    char_ipaddr = ipaddr_ntoa(&ipaddr);
    cyw43_arch_lwip_end();
    printf("Resolved %s (%s)\n", PICOHTTPS_HOSTNAME, char_ipaddr);

// Establish TCP + TLS connection with server
#ifdef MBEDTLS_DEBUG_C
    mbedtls_debug_set_threshold(PICOHTTPS_MBEDTLS_DEBUG_LEVEL);
#endif // MBEDTLS_DEBUG_C
    struct altcp_pcb *pcb = NULL;
    printf("Connecting to https://%s:%d\n", char_ipaddr, LWIP_IANA_PORT_HTTPS);
    if (!connect_to_host(&ipaddr, &pcb))
    {
        printf("Failed to connect to https://%s:%d\n", char_ipaddr, LWIP_IANA_PORT_HTTPS);
        // TODO: Disconnect from network
        cyw43_arch_deinit(); // Deinit Pico W wireless hardware
        return;
    }
    printf("Connected to https://%s:%d\n", char_ipaddr, LWIP_IANA_PORT_HTTPS);

    // Send HTTP request to server
    printf("Sending request\n");
    if (!send_request(pcb))
    { /*
         printf("Failed to send request\n");
         /altcp_free_config( // Free connection configuration
             ((struct altcp_callback_arg *)(pcb->arg))->config);
         altcp_free_arg( // Free connection callback argument
             (struct altcp_callback_arg *)(pcb->arg));
         altcp_free_pcb(pcb); // Free connection PCB
                              // TODO: Disconnect from network
         cyw43_arch_deinit(); // Deinit Pico W wireless hardware
         return;
   }*/
    /*
    printf("Request sent\n");

    // Await HTTP response
    printf("Awaiting response\n");
    sleep_ms(5000);
    printf("Awaited response\n");
    */

    while (1)
    {
        // ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        // code
    }
}

/*void send_telegram_message(const char *message)
{
 char url[512];
 snprintf(url, sizeof(url),
          "https://api.telegram.org/bot<7554206618:AAFAkT5Z3J_ka1REfExJjpucHAs6ZARAuwY>/sendMessage?chat_id=<8028211435>&text=%s",
          message);

 struct netconn *conn = netconn_new(NETCONN_TCP);
 struct ip4_addr ip;
 IP4_ADDR(&ip, 149, 154, 167, 220); // Telegram API's IP address (api.telegram.org)

 if (conn != NULL)
 {
     if (netconn_connect(conn, &ip, 443) == ERR_OK)
     {
         char request[512];
         snprintf(request, sizeof(request),
                  "GET %s HTTP/1.1\r\n"
                  "Host: api.telegram.org\r\n"
                  "Connection: close\r\n\r\n",
                  url);

         netconn_write(conn, request, strlen(request), NETCONN_COPY);
     }
     netconn_delete(conn);
 }
}*/

void smoke_task(void *pvParameters)
{
    // TickType_t xLastWakeTime = xTaskGetTickCount();
    // const TickType_t xFrequency = pdMS_TO_TICKS(500);
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
        // xTaskNotifyGive(alertTaskHandle);
        // xTaskNotifyGive(lcdTaskHandle);
        // vTaskDelayUntil(&xLastWakeTime, xFrequency);
    }
}

void gas_task(void *pvParameters)
{
    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xFrequency = pdMS_TO_TICKS(500);

    while (1)
    {
        // ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
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
    // send_telegram_message("Smoke, gas, or light detected!");
    while (1)
    {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        if (gas_alarm || smoke_alarm && !gpio_get(LIGHT_SENSOR_PIN))
        {
            gpio_put(ALERT_PIN, 1);
            // send_telegram_message("Smoke, gas, or light detected!");
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
        // ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
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

    /* server / wifi shit
    // Connect to the WiFI network - loop until connected
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
    */

    /* the loop
    write_lcd(read_smoke_sensor(), read_gas_sensor(), gpio_get(LIGHT_SENSOR_PIN));
    printf("Light: %d\n", gpio_get(LIGHT_SENSOR_PIN));
    printf("Gas: %d\n", read_gas_sensor());
    printf("Smoke: %d\n", read_smoke_sensor());
    if (read_gas_sensor() > GAS_SENSITIVITY || read_smoke_sensor() > SMOKE_SENSITIVITY && !gpio_get(LIGHT_SENSOR_PIN))
        {
            gpio_put(ALERT_PIN, 1);
        }
        else
        {
            gpio_put(ALERT_PIN, 0);
        }
        sleep_ms(450);
    */

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