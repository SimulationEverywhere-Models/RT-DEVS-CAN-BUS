#ifndef __Motor_HPP__
#define __Motor_HPP__


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

struct Motor_defs
{
struct input : public in_port<internal_msg> {};
struct output : public out_port<internal_msg>{};
};



template <typename TIME>

class Motor
{
    private:

    internal_msg msg;
    float a_weight = 0.25;
    float b_weight = 0.5;
    float drag_weight = 0.01;
    TIME update_rate;
    public:
    Motor() noexcept 
    {
        state.id = 1300;
        state.speed = 0;
        state.accel = 0;
        state.prev_speed = 0;
        state.update = false;
        update_rate = TIME("00:00:00:60");
        msg.id = state.id;
        msg.len = 1;
        msg.data[0] = 0;
        }
        
    Motor(int _id)
    {
        state.id = _id;
        state.speed = 0;
        state.accel = 0;
        state.prev_speed = 0;
        state.update = false;
        update_rate = TIME("00:00:00:60");
        msg.id = state.id;
        msg.len = 1;
        msg.data[0] = 0;
    }

    struct state_type
    {
        int speed;
        int accel;
        int brake;
        int prev_speed;
        int id;
        bool update;
    };
    struct state_type state;

    using defs = struct Motor_defs;

    using input_ports = std::tuple<typename defs::input>;
    using output_ports = std::tuple<typename defs::output>;
    // internal transition
    void internal_transition()
    {
        state.speed = state.prev_speed + ( a_weight*state.accel - b_weight*state.brake - (state.prev_speed *state.prev_speed * drag_weight)/2);
        if (state.speed >255)
        {
            state.speed = 255;
        }
        else if (state.speed <0)
        {
            state.speed = 0;
        }
        msg.id = state.id;
        msg.len = 1;
        msg.data[0] = (unsigned char) state.speed;
        state.prev_speed = state.speed;
    }

    // external transition
    void external_transition(TIME e, typename make_message_bags<input_ports>::type mbs)
    {  
        vector<internal_msg> msg = get_messages<typename defs::input>(mbs);
        if (msg[0].id == 1100)
        {
            state.brake = msg[0].data[0];
        }
        else if(msg[0].id == 1200)
        {
            state.accel = msg[0].data[0];
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
        get_messages<typename defs::output>(bags).push_back(msg);
        return bags;
    }

    // time_advance function
    TIME time_advance() const
    {
        return update_rate;
    }

    friend std::ostringstream &operator<<(std::ostringstream &os, const typename Motor<TIME>::state_type &i)
    {
        os << i.speed << " :speed " << i.brake << " :brake" << i.accel <<" :acceleration" ;
        return os;
    }
  
};


#endif


