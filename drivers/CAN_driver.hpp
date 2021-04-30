#ifndef __CAN_DRIVER_HPP__
#define __CAN_DRIVER_HPP__

#include <mbed.h>
#include <cadmium/real_time/arm_mbed/embedded_error.hpp>


namespace drivers{

    //driver for the CAN controllers.


    class CAN_driver {
        private:
            CAN can_object;

        public:

            CAN_driver(PinName rd, PinName td) : can_object(rd, td)
            {
                //default frequency to 100 Khz
                can_object.frequency(100000);
            }

            int write(CANMessage msg)
            {
                int operation_success = 0;
                operation_success = can_object.write(msg);
                wait_ms(10);
                return operation_success;
            }

            int read(CANMessage &msg, int filter=0)
            {
                int operation_success = 0;
                operation_success = can_object.read(msg, filter);
                wait_ms(10);
                return operation_success;
            }


    };


}

#endif