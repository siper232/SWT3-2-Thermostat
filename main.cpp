#include "adc.h"
#include "board_freedom.h"
#include "mbed.h"
#include "oled_ssd1322.h"
#include <cstdint>
#include <cstdio>

PwmOut heater_power(PTC2);
DigitalOut redLed(PTB2);
DigitalOut greenLed(PTB3);

#define MESSAGE_MAX_SIZE 50
#define MAX_TEMP 40

int main() {
  board_init();

  u8g2_ClearBuffer(&oled);
  u8g2_SetFont(&oled, u8g2_font_6x12_mr);
  u8g2_SendBuffer(&oled);

  char message[MESSAGE_MAX_SIZE + 1];
  message[MESSAGE_MAX_SIZE] = '\0';

  float prevTemp = 0;

  while (true) {
    uint16_t temp_plate = adc_read(0);
    float voltage = 3.0 / 65535 * temp_plate;

    if (voltage < 0.21) {
      heater_power = 0;
      printf("Sensor not working/unavailable\n");

      snprintf(message, MESSAGE_MAX_SIZE, "Sensor not working/unavailable");

      u8g2_ClearBuffer(&oled);
      u8g2_DrawUTF8(&oled, 10, 10, message);
      u8g2_SendBuffer(&oled);

    } else {
      float temp = (voltage * 1000 - 400) / 19.5;
      float tempDif = (temp - prevTemp);

      // heater controll
      if (temp > MAX_TEMP) {
        heater_power = 0;
      } else {
        if (temp < 31.0) {
          heater_power = 1;
        } else if (temp + (tempDif * 4) > 35.0 && prevTemp != 0) {
          heater_power = 0;
        }
      }

      // led controll
      if (temp > 30.0 && temp < 35.0 && heater_power == 0) {
        redLed = 0;
        greenLed = 1;
      } else if (heater_power == 1) {
        greenLed = 0;
        redLed = 1;
      } else {
        redLed = 0;
        greenLed = 0;
      }

      snprintf(message, MESSAGE_MAX_SIZE,
               "Value is %-5d, temperature is %5.02f", temp_plate, temp);

      u8g2_ClearBuffer(&oled);
      u8g2_DrawUTF8(&oled, 10, 10, message);
      u8g2_SendBuffer(&oled);

      printf("%s\n", message);
    }
    ThisThread::sleep_for(100ms);
  }
}
