#ifndef __CAN_INPUT_HPP__
#define __CAN_INPUT_HPP__



#ifndef RT_ARM_MBED

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



#include "../../data_structures/can_structure.hpp"
#include "../../data_structures/test_input.hpp"

struct can_test_defs
{
    struct DataIn : public in_port<test_input>
    {

    } ;

    struct DataOut : public out_port<CANMessage>
    {

    };
};


template <typename TIME> 

class can_input
{
    private:

    CANMessage canmsg;

    public:
    can_input() noexcept 
    {
        state.transmit = false;
        canmsg.len = 1;
    }


    struct state_type
    {
        bool transmit;
    };
    struct state_type state;

    using defs = struct can_test_defs;

    using input_ports = std::tuple<typename defs::DataIn>;
    using output_ports = std::tuple<typename defs::DataOut>;

    // internal transition
    void internal_transition()
    {
        state.transmit = false;
    }

    // external transition
    void external_transition(TIME e, typename make_message_bags<input_ports>::type mbs)
    {

        if (get_messages<typename defs::DataIn>(mbs).size() > 0)
        {
            state.transmit = true;
            vector<test_input> msg = get_messages<typename defs::DataIn>(mbs);
            this->canmsg.id = msg[0].id;
            int data = msg[0].data;
            if (data > 255)
            {
                data = 255;
            }
            else if (data < 0)
            {
                data =0;
            }

            this->canmsg.data[0] = (unsigned char) data;
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
        if (state.transmit)
        {
            get_messages<typename defs::DataOut>(bags).push_back(canmsg);
        }
        return bags;
    }

    // time_advance function
    TIME time_advance() const
    {
        if (state.transmit)
        {
            return TIME("00:00:00:001");
        }

        else
        {
            return numeric_limits<TIME>::infinity();
        }
    }

    friend std::ostringstream &operator<<(std::ostringstream &os, const typename can_input<TIME>::state_type &i)
    {
        os << i.transmit;
    }

};

#endif
#endif