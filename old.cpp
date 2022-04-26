#include "mbed.h"
#include "board_freedom.h"
#include "adc.h"
#include "oled_ssd1322.h"
#include <cstdint>
#include <cstdio>
 
#define MESSAGE_MAX_SIZE 50
 
// main() runs in its own thread in the OS
 
int main()
{
    board_init();
    oled_splash();

    u8g2_ClearBuffer(&oled);
    u8g2_SetFont(&oled, u8g2_font_6x12_mr);
    u8g2_SendBuffer(&oled);
 
    // Ready a single reusable buffer for writing text to.
    char message[MESSAGE_MAX_SIZE + 1];
    message[MESSAGE_MAX_SIZE] = '\0';

    PwmOut heater_power(PTC2);
    heater_power = 1;

    while (true) {
        uint16_t temp_plate = adc_read(0);
        float voltage = 3.0 / 65535 * temp_plate;
        float temp = ( voltage * 1000 - 400) / 19.5;

        snprintf(message, MESSAGE_MAX_SIZE, "Value is %-5d, temperature is %5.02f", temp_plate , temp);

        // Clear screen and write a message.
        u8g2_ClearBuffer(&oled);
        u8g2_DrawUTF8(&oled, 10, 10, message);
        u8g2_SendBuffer(&oled);
        
        // Also write the message over USB to the serial monitor
        printf("%s\n", message);
 
        // Wait a bit
        ThisThread::sleep_for(100ms);
    }
}


