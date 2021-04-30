#ifndef __LCD_DISPLAY_HPP__
#define __LCD_DISPLAY_HPP__


#include <cadmium/modeling/ports.hpp>
#include <cadmium/modeling/message_bag.hpp>
#include <limits>
#include <math.h>
#include <assert.h>
#include <memory>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <algorithm>
#include <limits>
#include <random>
#include <tuple>
#include "../../data_structures/internal_structure.hpp"


using namespace std;

struct lcd_out
{
    char txt[32];
    lcd_out(const char * arr)
    {
        memcpy(txt,arr,32);
    }
};

ostream& operator<<(ostream& os, const lcd_out& msg)
{
    for (int i = 0; i < 32 ; i++)
    {
    os << msg.txt[i];
    }
    return os; 
}

struct LCD_defs
{
struct input : public in_port<internal_msg> {};
struct output : public out_port<lcd_out>{};
};
#ifdef RT_ARM_MBED

#include "../../mbed-os/mbed.h"
#include <cadmium/real_time/arm_mbed/embedded_error.hpp>

#include "../../drivers/LCD.hpp"


using namespace mbed;

template <typename TIME>

class LCD_display
{
    private:
    char txt[32];
    unsigned int brake;
    unsigned int accel;
    unsigned int speed;

   public:
    LCD_display() noexcept 
    {
        cadmium::embedded::embedded_error::hard_fault("LCD display needs pin definition");
    }

    LCD_display(PinName rs, PinName e, PinName d4, PinName d5, PinName d6, PinName d7)
    {
        cout<<"Start\n";
        state.write = false;
        state.lcd_object = new drivers::LCD_driver(rs,e,d4,d5,d6,d7);
        brake = 0;
        accel = 0;
        speed = 0;
        state.lcd_object->cls(); 
    }

    struct state_type
    {
        bool write;
        drivers::LCD_driver* lcd_object;
    };
    struct state_type state;

    using defs = struct LCD_defs;

    using input_ports = std::tuple<typename defs::input>;
    using output_ports = std::tuple<typename defs::output>;
    // internal transition
    void internal_transition()
    {
        state.write = false;
    }

    // external transition
    void external_transition(TIME e, typename make_message_bags<input_ports>::type mbs)
    {
        if (get_messages<typename defs::input>(mbs).size() > 0)
        {
            vector<internal_msg> msg = get_messages<typename defs::input>(mbs);
            unsigned int value = msg[0].data[0];
            value = (value*100)/255; //convert to percentage.
            if(msg[0].id == 1100)
            {
                brake = value;
                sprintf(txt,"Speed:%d\nAcl:%d Brk:%d",speed,accel,brake);
            }
            else if (msg[0].id == 1200)
            {
                accel = value;
                sprintf(txt,"Speed:%d\nAcl:%d Brk:%d",speed,accel,brake);
            }
            else if(msg[0].id == 1300)
            {
                speed = value;
                sprintf(txt,"Speed:%d\nAcl:%d Brk:%d",speed,accel,brake);
            }
            state.write = true;
        }
    }

    // confluence transition
    void confluence_transition(TIME e, typename make_message_bags<input_ports>::type mbs)
    {
        internal_transition();
        external_transition(TIME(), std::move(mbs));
    }

    // output function
    typename make_message_bags<output_ports>::type output() const
    {
        typename make_message_bags<output_ports>::type bags;
        if( state.write)
        {
            state.lcd_object->cls();
            state.lcd_object->write(txt);
        }
        return bags;
    }

    // time_advance function
    TIME time_advance() const
    {
        if (state.write)
        {
            return TIME("00:00:00:010");
        }
        else
        {
            return numeric_limits<TIME>::infinity();
        }

    }

    friend std::ostringstream &operator<<(std::ostringstream &os, const typename LCD_display<TIME>::state_type &i)
    {
        os << i.write << " :Write " ;
        return os;
    }
  
};

#else

template <typename TIME>

class LCD_display
{
    private:
    char txt[32];
    unsigned int brake;
    unsigned int accel;
    unsigned int speed;

   public:
    LCD_display() noexcept 
    {
       assert(true && "LCD display needs pin definition");
    }

    LCD_display(const char * rs, const char * e, const char * d4, const char * d5, const char * d6, const char * d7)
    {
        state.write = false;
        brake = 0;
        accel = 0;
        speed = 0;
    }

    struct state_type
    {
        bool write;
    };
    struct state_type state;

    using defs = struct LCD_defs;

    using input_ports = std::tuple<typename defs::input>;
    using output_ports = std::tuple<typename defs::output>;
    // internal transition
    void internal_transition()
    {
        state.write = false;
    }

    // external transition
    void external_transition(TIME e, typename make_message_bags<input_ports>::type mbs)
    {
        if (get_messages<typename defs::input>(mbs).size() > 0)
        {
            vector<internal_msg> msg = get_messages<typename defs::input>(mbs);
            unsigned int value = msg[0].data[0];
            value = (value*100)/255; //convert to percentage.
            if(msg[0].id == 1100)
            {
                brake = value;
                sprintf(txt,"Speed:%d\nAcl:%d Brk:%d",speed,accel,brake);
            }
            else if (msg[0].id == 1200)
            {
                accel = value;
                sprintf(txt,"Speed:%d\nAcl:%d Brk:%d",speed,accel,brake);
            }
            else if(msg[0].id == 1300)
            {
                speed = value;
                sprintf(txt,"Speed:%d\nAcl:%d Brk:%d",speed,accel,brake);
            }
            state.write = true;
        }
    }

    // confluence transition
    void confluence_transition(TIME e, typename make_message_bags<input_ports>::type mbs)
    {
        internal_transition();
        external_transition(TIME(), std::move(mbs));
    }

    // output function
    typename make_message_bags<output_ports>::type output() const
    {
        typename make_message_bags<output_ports>::type bags;
        if( state.write)
        {
            lcd_out temp(txt);
            get_messages<typename defs::output>(bags).push_back(temp);
        }
        return bags;
    }

    // time_advance function
    TIME time_advance() const
    {
        if (state.write)
        {
            return TIME("00:00:00:010");
        }
        else
        {
            return numeric_limits<TIME>::infinity();
        }

    }

    friend std::ostringstream &operator<<(std::ostringstream &os, const typename LCD_display<TIME>::state_type &i)
    {
        os << i.write << " :Write " ;
        return os;
    }
  
};


#endif

#endif


