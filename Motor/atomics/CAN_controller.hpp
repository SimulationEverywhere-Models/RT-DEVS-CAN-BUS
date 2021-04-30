#ifndef __CAN_controller_HPP__
#define __CAN_controller_HPP__





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

#ifdef RT_ARM_MBED

#include "../../mbed-os/mbed.h"
#include <cadmium/real_time/arm_mbed/embedded_error.hpp>
#include "../../mbed-os/drivers/CAN.h"
#include "../../mbed-os/hal/can_helper.h"
#include "../../drivers/CAN_driver.hpp"


using namespace mbed;

#else

#include "../../data_structures/can_structure.hpp"

#endif

struct canController_defs
{
    struct Rx : public in_port<CANMessage>
    {

    } ;
    struct DataIn : public in_port<internal_msg>
    {

    };

    struct Tx : public out_port<CANMessage>
    {

    };

    struct DataOut : public out_port<internal_msg>
    {

    };
};


#ifdef RT_ARM_MBED

template <typename TIME> 

class CAN_controller
{
    private:

    internal_msg datamsg;
    CANMessage canmsg;
    TIME polling_rate; 
    public:
    CAN_controller() noexcept 
    {
        cadmium::embedded::embedded_error::hard_fault("CAN controller needs input pin definition");
    }

    CAN_controller(PinName rx, PinName td, unsigned int _mode)
    {
        cout<< "contructur CAN_controller\n ";
        state.transmit = false;
        state.recieve = false;
        state.internal_done = true;
        state.CAN_object = new drivers::CAN_driver(rx,td);
        state.mode = _mode;
        polling_rate = TIME("00:00:00:000");
        cout<< "contructur CAN_controller done\n ";
    }

    struct state_type
    {
        bool transmit;
        bool recieve;
        bool internal_done;
        drivers::CAN_driver* CAN_object;
        unsigned int mode;
    };
    struct state_type state;

    using defs = struct canController_defs;

    using input_ports = std::tuple<typename defs::Rx, typename defs::DataIn>;
    using output_ports = std::tuple<typename defs::Tx, typename defs::DataOut>;

    // internal transition
    void internal_transition()
    {
        if (state.mode == 0)
        {
            CANMessage msg;
            state.recieve  = state.CAN_object->read(msg);
            if (state.recieve)
            {
                this->datamsg.id = msg.id;
                this->datamsg.len = msg.len;
                for(int i = 0; i < this->datamsg.len ; i++)
                {
                this->datamsg.data[i] = msg.data[i];
                }
            }
            state.transmit = false;
        }
        else if(state.mode == 2)
        {
           if (state.internal_done)
            {
            CANMessage msg;
            state.recieve  = state.CAN_object->read(msg);
            if (state.recieve)
            {
                this->datamsg.id = msg.id;
                this->datamsg.len = msg.len;
                for(int i = 0; i < this->datamsg.len ; i++)
                {
                this->datamsg.data[i] = msg.data[i];
                }
                state.internal_done = false;
            }
            state.transmit = false;
            }
            else if (state.recieve == true)
            {
                state.recieve = false;
            }
        }
        else
        {
            state.transmit = false;
            state.recieve = false;
        }
    }

    // external transition
    void external_transition(TIME e, typename make_message_bags<input_ports>::type mbs)
    {

        if (state.mode == 1 || state.mode ==2)
        {        // if (get_messages<typename defs::Rx>(mbs).size() > 1)
        // { // Multiple messages in RX port not allowed at same time
        //     assert(true && "Muliple Messages received at Tx. Not allowed!");
        // }

        // if (get_messages<typename defs::Rx>(mbs).size() > 0)
        // { // Did we receive external input?
        //     // state.recieve = true;
        //     // CANMessage msg;
        //     // state.CAN_object->read(msg);
        //     // // vector<CANMessage> msg = get_messages<typename defs::Rx>(mbs);
        //     // this->datamsg.id = msg.id;
        //     // this->datamsg.len = msg.len;
        //     // for(int i = 0; i < this->datamsg.len ; i++)
        //     // {
        //     // this->datamsg.data[i] = msg.data[i];
        //     // }

        // }

        if (get_messages<typename defs::DataIn>(mbs).size() > 0)
        {
            state.transmit = true;
            vector<internal_msg> msg = get_messages<typename defs::DataIn>(mbs);
            this->canmsg.id = msg[0].id;
            this->canmsg.len = msg[0].len;
            for(int i = 0; i < this->canmsg.len ; i++)
            {
            this->canmsg.data[i] = msg[0].data[i];             
            }
            state.internal_done = true;
        }
        }
        else
        {
        cadmium::embedded::embedded_error::hard_fault("Input recieved in read-only mode.");
        }
    }

    // confluence transition
    void confluence_transition(TIME e, typename make_message_bags<input_ports>::type mbs)
    {
        if (state.mode == 2)    
    {    
        external_transition(TIME(), std::move(mbs));
        internal_transition();
    }  
    else
    {
        internal_transition();
        external_transition(TIME(), std::move(mbs));
    }  
    }

    // output function
    typename make_message_bags<output_ports>::type output() const
    {
        typename make_message_bags<output_ports>::type bags;
        if (state.recieve)
        {
            get_messages<typename defs::DataOut>(bags).push_back(datamsg);
        }

        if (state.transmit)
        {
            state.CAN_object->write(canmsg);
            get_messages<typename defs::Tx>(bags).push_back(canmsg);
        }
        return bags;
    }

    // time_advance function
    TIME time_advance() const
    {
        if (state.transmit || state.recieve && state.mode == 1)
        {
            return TIME("00:00:00:010");
        }
        else if (state.mode == 1)
        {
            return numeric_limits<TIME>::infinity();
        }
        else if (state.mode == 0)
        {
            if(state.recieve)
            {
                return TIME("00:00:00:010");
            }
            else
            {
                return polling_rate;
            }
        }
        else if (state.mode == 2)
        {
            if(state.transmit || state.recieve)
            {
                return TIME("00:00:00:020");
            }
            else if (!state.internal_done)
            {
                return numeric_limits<TIME>::infinity();
            }
            else
            {
                return polling_rate;
            }

        }
    }

    friend std::ostringstream &operator<<(std::ostringstream &os, const typename CAN_controller<TIME>::state_type &i)
    {
        os << i.recieve << " :Recieve " << i.transmit << " :Transmit " << i.mode << " : mode";
        return os;
    }

};

#else



template <typename TIME> 

class CAN_controller
{
    private:

    internal_msg datamsg;
    CANMessage canmsg;
    TIME polling_rate; 
    public:
    CAN_controller() noexcept 
    {
        assert(true && "CAN_controller requires pin definition");
    }

    CAN_controller(const char *rx, const char *td, unsigned int _mode)
    {
        state.transmit = false;
        state.recieve = false;
        state.mode = _mode;
        state.internal_done = true;
        polling_rate = TIME("00:00:00:100"); 
    }

    struct state_type
    {
        bool transmit;
        bool recieve;
        int mode;
        int internal_done;
    };
    struct state_type state;

    using defs = struct canController_defs;

    using input_ports = std::tuple<typename defs::Rx, typename defs::DataIn>;
    using output_ports = std::tuple<typename defs::Tx, typename defs::DataOut>;

    // internal transition
    void internal_transition()
    {
        state.transmit = false;
        state.recieve = false;
    }

    // external transition
    void external_transition(TIME e, typename make_message_bags<input_ports>::type mbs)
    {

        if (get_messages<typename defs::Rx>(mbs).size() > 1)
        { // Multiple messages in RX port not allowed at same time
            assert(true && "Muliple Messages received at rx. Not allowed!");
        }

        if (get_messages<typename defs::Rx>(mbs).size() > 0)
        { // Did we receive external input?
            state.recieve = true;
            vector<CANMessage> msg = get_messages<typename defs::Rx>(mbs);
            this->datamsg.id = msg[0].id;
            this->datamsg.len = msg[0].len;
            for(int i = 0; i < this->datamsg.len ; i++)
            {
            this->datamsg.data[i] = msg[0].data[i];
            }
            state.internal_done = false;
        }

        else if (get_messages<typename defs::DataIn>(mbs).size() > 0)
        {
            state.transmit = true;
            vector<internal_msg> msg = get_messages<typename defs::DataIn>(mbs);
            this->canmsg.id = msg[0].id;
            this->canmsg.len = msg[0].len;
            for(int i = 0; i < this->canmsg.len ; i++)
            {
            this->canmsg.data[i] = msg[0].data[i];        
            }
            state.internal_done = true;
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
        if (state.recieve)
        {
            get_messages<typename defs::DataOut>(bags).push_back(datamsg);
        }

        if (state.transmit)
        {
            get_messages<typename defs::Tx>(bags).push_back(canmsg);
        }
        return bags;
    }

    // time_advance function
    TIME time_advance() const
    {
        if (state.transmit || state.recieve && state.mode == 1)
        {
            return TIME("00:00:00:010");
        }
        else if (state.mode == 1)
        {
            return numeric_limits<TIME>::infinity();
        }
        else if (state.mode == 0)
        {
            if(state.recieve)
            {
                return TIME("00:00:00:010");
            }
            else
            {
                return polling_rate;
            }
        }
        else if (state.mode == 2)
        {
            if(state.transmit || state.recieve)
            {
                return TIME("00:00:00:020");
            }
            else if (!state.internal_done)
            {
                return numeric_limits<TIME>::infinity();
            }
            else
            {
                return polling_rate;
            }

        }
    }

    friend std::ostringstream &operator<<(std::ostringstream &os, const typename CAN_controller<TIME>::state_type &i)
    {
        os << i.recieve << " :Recieve " << i.transmit << " :Transmit";
        return os;
    }

};


#endif
#endif