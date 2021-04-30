#ifndef __LCD_HPP__
#define __LCD_HPP__

#include "../lib/TextLCD/TextLCD.h"

namespace drivers
{
    class LCD_driver
    {
        private:
            TextLCD* lcd_object;
        public:

            LCD_driver(PinName rs, PinName e, PinName d4, PinName d5, PinName d6, PinName d7)

            {
                lcd_object = new TextLCD(rs,e,d4,d5,d6,d7);
            }

            int write(const char* text)
            {
                int a = lcd_object->printf(text);
                wait_ms(10);
                return a;
            }

            void cls()
            {
                lcd_object->cls();
            }
    };
}

#endif