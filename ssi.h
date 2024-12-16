#include "lwip/apps/httpd.h"
#include "pico/cyw43_arch.h"
#include "hardware/adc.h"
#include "main.h"

// SSI tags - tag length limited to 8 bytes by default
const char *ssi_tags[] = {"smoke", "gas", "light", "message"};

u16_t ssi_handler(int iIndex, char *pcInsert, int iInsertLen)
{
  size_t printed;
  adc_select_input(0);
  const float ss = adc_read();
  adc_select_input(2);
  const float gs = adc_read();
  switch (iIndex)
  {
  case 0: // smoke
  {
    adc_select_input(0);
    const float smoke = adc_read();
    printed = snprintf(pcInsert, iInsertLen, "%f", smoke);
  }
  break;
  case 1: // gas
  {
    adc_select_input(2);
    const float gas = adc_read();
    printed = snprintf(pcInsert, iInsertLen, "%f", gas);
  }
  break;
  case 2: // light
  {
    // bool led_status = cyw43_arch_gpio_get(CYW43_WL_GPIO_LED_PIN);
    bool light_status = gpio_get(LIGHT_SENSOR_PIN);
    if (light_status == true)
    {
      printed = snprintf(pcInsert, iInsertLen, "1");
    }
    else
    {
      printed = snprintf(pcInsert, iInsertLen, "0");
    }
  }
  break;
  case 3: // alarm message
  {
    if (ss > SMOKE_SENSITIVITY && !gpio_get(LIGHT_SENSOR_PIN))
    {
      printed = snprintf(pcInsert, iInsertLen, "DARK SMOKE ALERT");
    }
    if (ss > SMOKE_SENSITIVITY && gpio_get(LIGHT_SENSOR_PIN))
    {
      printed = snprintf(pcInsert, iInsertLen, "SMOKE ALERT");
    }
    if (gs > GAS_SENSITIVITY && ss <= SMOKE_SENSITIVITY)
    {
      printed = snprintf(pcInsert, iInsertLen, "GAS LEAK ALERT");
    }
    if (!(ss > SMOKE_SENSITIVITY && !gpio_get(LIGHT_SENSOR_PIN)) && !(ss > SMOKE_SENSITIVITY && gpio_get(LIGHT_SENSOR_PIN)) && !(gs > GAS_SENSITIVITY && ss <= SMOKE_SENSITIVITY))
    {
      printed = snprintf(pcInsert, iInsertLen, "");
    }
  }
  break;
  default:
    printed = 0;
    break;
  }

  return (u16_t)printed;
}

// Initialise the SSI handler
void ssi_init()
{
  // Initialise ADC (internal pin)
  // adc_init();
  adc_set_temp_sensor_enabled(true);
  adc_select_input(2);

  http_set_ssi_handler(ssi_handler, ssi_tags, LWIP_ARRAYSIZE(ssi_tags));
}
