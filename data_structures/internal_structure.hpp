#ifndef __INTERNAL_STRUCTURE_HPP__
#define __INTERNAL_STRUCTURE_HPP__

#include <memory.h>
#include <iostream>

struct internal_msg
{
    unsigned int   id;    
    unsigned char  len;            
    unsigned char  data[8];          
  

    internal_msg(unsigned int _id, const unsigned char *_data, unsigned char _len)
    {
        id = _id;
        len = _len;
        memcpy(data, _data, len);
    }

    internal_msg()
    {
        id = 0;
        len = (unsigned char)8;
        memset(data, 0, 8);
    }

};

istream& operator>> (istream& is, internal_msg& msg)  //overloading operators for cater for our model
{
    is >> msg.id;
    is >> msg.len;
    for (int i = 0; i<msg.len ; i++)
    {
        is >> msg.data[i];
    }
    return is;
}

ostream& operator<<(ostream& os, const internal_msg& msg)
{
    os << msg.id << " " << (unsigned int) msg.len;
    for (int i = 0; i<msg.len ; i++)
    {
        os << " " << (unsigned int) msg.data[i];
    }
    return os; 
}
#endif 