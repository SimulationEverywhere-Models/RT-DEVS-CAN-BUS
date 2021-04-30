#ifndef __CAN_STRUCTURE_HPP__
#define __CAN_STRUCTURE_HPP__

#ifndef RT_ARM_MBED
#include <memory.h>
#include <iostream>

enum CANFormat {
    CANStandard = 0,
    CANExtended = 1,
    CANAny = 2
};
typedef enum CANFormat CANFormat;

enum CANType {
    CANData = 0,
    CANRemote = 1
};
typedef enum CANType CANType;

struct CAN_Message
{
    unsigned int   id;               
    unsigned char  data[8];          
    unsigned char  len;               
    CANFormat      format;            
    CANType        type;              
};

class CANMessage : public CAN_Message
{
    public :
    
    CANMessage()
    {
        id = 0U;
        memset(data, 0,8);
        len = 8U;
        format = CANStandard;
        type = CANRemote;
    }

    CANMessage(unsigned int _id, const unsigned char *_data, unsigned char _len = 8)
    {
        id = _id;
        memcpy(data,_data,_len);
        len = _len;
        format = CANStandard;
        type = CANRemote;
    }
    
};

istream& operator>> (istream& is, CANMessage& msg)  //overloading operators for cater for our model
{
    is >> msg.id;
    is >> msg.len;
    for (int i = 0; i<msg.len ; i++)
    {
        is >> msg.data[i];
    }
    return is;
}

ostream& operator<<(ostream& os, const CANMessage& msg)
{
    os << msg.id << " " << (unsigned int) msg.len;
    for (int i = 0; i<msg.len ; i++)
    {
        os << " " << (unsigned int) msg.data[i];
    }
    os << " " << msg.format;
    os << " " << msg.type;
    return os; 
}
#endif
#endif