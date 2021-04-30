#ifndef __TEST_INPUT_HPP__
#define __TEST_INPUT_HPP__

#ifndef RT_ARM_MBED
#include <memory.h>
#include <iostream>

struct test_input
{
    int id;
    int data;
};

istream& operator>> (istream& is, test_input& msg)  //overloading operators for cater for our model
{
    is >> msg.id;
    is >> msg.data;
}

ostream& operator<<(ostream& os, const test_input& msg)
{
    os << msg.id << " " << (unsigned int) msg.data;
    return os; 
}
#endif
#endif