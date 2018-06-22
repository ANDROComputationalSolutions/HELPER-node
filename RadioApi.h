/* ------------------------------------------------------------------------------
 * Copyright 2018 ANDRO Computational Solutions, LLC.  All Rights Reserved
 * ------------------------------------------------------------------------------
*/
 
/*
 * Defines an interface for different physical layer / radios 
 * 
 * */ 

#ifndef RADIO_API_H
#define RADIO_API_H

#include <stdint.h>
#include <vector>

class RadioAPI { 
public:
    virtual ~RadioAPI() {} 
    
    virtual int recv(std::vector<uint8_t> &msg) = 0;
	virtual void send(std::vector<uint8_t> payload) = 0; 
    virtual int8_t get_last_rssi(void) = 0;
    virtual uint16_t get_max_packet_size(void) = 0; 
    
}; 

class RadioFactory {
public:
    RadioAPI * create_radio(uint8_t nodeid); 
};

#endif 
