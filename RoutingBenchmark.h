/* ------------------------------------------------------------------------------
 * Copyright 2018 ANDRO Computational Solutions, LLC.  All Rights Reserved
 * ------------------------------------------------------------------------------
*/
 
/* 
 * Header file for benchmark network layer (RoutingBenchmark is a
 * subclass / impl for RoutingApi) 
 * 
 * */ 

#ifndef ROUTING_BENCHMARK_H
#define ROUTING_BENCHMARK_H

#include "RoutingApi.h" 

#include <queue> 

class RoutingBenchmark : public RoutingAPI
{
public: 
    //send packet
    int send_msg(HelperPacket &hp); 
    //receive packets
    int local_packets_available(void); 
    HelperPacket rx_local_packet(void); 
    
    //cross-layer functionality 
    void write_beacon_data(HelperPacket &hp); 
    int process_beacon_data(HelperPacket &hp);  
    
    int mac_recv(HelperPacket &hp); 
    int packets_to_route(void); 
    int route(void);
    uint8_t get_destination_id(void); 
    HelperPacket pop_routed_packet(void);  
    
    void send_beacon_data_to_app(void);  

private: 
    std::queue<HelperPacket> tx_q; 
    std::queue<HelperPacket> rx_q; 
    uint8_t dest_id; 
}; 

#endif 
