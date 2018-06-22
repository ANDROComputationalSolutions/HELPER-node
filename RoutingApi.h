/* ------------------------------------------------------------------------------
 * Copyright 2018 ANDRO Computational Solutions, LLC.  All Rights Reserved
 * ------------------------------------------------------------------------------
*/
 
/*
 * Defines interface for different network layers 
 * 
 * */ 

#ifndef ROUTING_API_H
#define ROUTING_API_H

#include <stdint.h>
#include "helper_packet.h" 

class RoutingAPI { 
public:
    virtual ~RoutingAPI() {} 
    
    //send a Helper Packet
    virtual int send_msg(HelperPacket &hp) = 0; 
    //receive packets
    virtual int local_packets_available(void) = 0; 
    virtual HelperPacket rx_local_packet(void) = 0; 
    
    //cross-layer functionality 
    virtual void write_beacon_data(HelperPacket &hp)= 0; 
    virtual int process_beacon_data(HelperPacket &hp)= 0;  
    
    //mac layer interfaces 
    virtual int mac_recv(HelperPacket &hp) = 0; 
    virtual int packets_to_route(void) = 0; 
    virtual int route(void) = 0; 
    virtual uint8_t get_destination_id(void) = 0; 
    virtual HelperPacket pop_routed_packet(void) = 0;  
    virtual void send_beacon_data_to_app(void) = 0;  
}; 

class RoutingFactory {
public:
    RoutingAPI * create_routing(uint8_t nodeid); 
};

#endif 
