#ifndef __DATA_TO_CAN_HPP__
#define __DATA_TO_CAN_HPP__

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


struct floatToInternal_defs
{

    struct DataIn : public in_port<float>
    {

    };

    struct DataOut : public out_port<internal_msg>
    {

    };
};


template <typename TIME> 

class float_to_internal
{
    private:

    internal_msg datamsg;

    public:
    float_to_internal() noexcept 
    {

        state.transmit = false;
        state.id = 0;
    }

    float_to_internal(unsigned _id)
    {

        state.transmit = false;
        state.id = _id;

    }

    struct state_type
    {
        bool transmit;
        unsigned int id;
    };
    struct state_type state;

    using defs = struct floatToInternal_defs;

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

        if (get_messages<typename defs::DataIn>(mbs).size() > 1)
        { // Multiple messages in RX port not allowed at same time
            assert(true && "Muliple Messages received at DataIn. Not allowed!");
        }
        else
        {
            vector<float> message = get_messages<typename defs::DataIn>(mbs);
            unsigned int temp = (int) (floor(message[0] * 255));
            if(temp > 255)
            {
                temp = 255;
            }
            else if(temp < 0)
            {
                temp = 0;
            }

            datamsg.id = state.id;
            datamsg.data[0] = (unsigned char) temp;
            datamsg.len = 1;
            state.transmit = true;
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


            get_messages<typename defs::DataOut>(bags).push_back(datamsg);
        }     
       

        return bags;
    }

    // time_advance function
    TIME time_advance() const
    {

        if (state.transmit)
        {

            return TIME("00:00:00:050");
        }
        else
        {
            return numeric_limits<TIME>::infinity();
        }
    }

    friend std::ostringstream &operator<<(std::ostringstream &os, const typename float_to_internal<TIME>::state_type &i)
    {
        os << i.transmit << " :Transmit " << i.id << " :id";
        return os;
    }

};



#endif