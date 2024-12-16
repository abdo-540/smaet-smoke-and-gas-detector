#ifndef SMOKE_DETECTOR_H
#define SMOKE_DETECTOR_H

#include <stdint.h>

// Define hardware pins
// #define M_LED_PIN 25
#define TEMP_PIN 4
#define SMOKE_ADC_PIN 26    // GPIO pin for smoke sensor (ADC channel 0)
#define GAS_ADC_PIN 27      // GPIO pin for gas sensor (ADC channel 2)
#define LIGHT_SENSOR_PIN 22 // GPIO pin for light sensor (digital input)
#define ALERT_PIN 15        // GPIO pin for buzzer/LED
#define E_PIN 8
#define RS_PIN 14
#define D4_PIN 9
#define D5_PIN 10
#define D6_PIN 11
#define D7_PIN 12

#define configTICK_RATE_HZ 1000 // 1ms tick

// Define sensitivity thresholds
#define SMOKE_SENSITIVITY 750 // 850
#define GAS_SENSITIVITY 1700
// #define LIGHT_SENSITIVITY 1000

// Task priorities
#define SMOKE_TASK_PRIORITY 1
#define GAS_TASK_PRIORITY 1
#define ALERT_TASK_PRIORITY 1
#define SERVER_TASK_PRIORITY 2
#define LCD_TASK_PRIORITY 3

// Function prototypes
void init_hardware(void);
uint16_t read_smoke_sensor(void);
uint16_t read_gas_sensor(void);
uint16_t read_light_sensor(void);
void gas_task(void *pvParameters);
void smoke_task(void *pvParameters);
void lcd_task(void *pvParameters);
void alert_task(void *pvParameters);
void server_task(void *pvParameters);

#endif