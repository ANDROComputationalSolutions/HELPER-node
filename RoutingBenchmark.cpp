/* ------------------------------------------------------------------------------
 * Copyright 2018 ANDRO Computational Solutions, LLC.  All Rights Reserved
 * ------------------------------------------------------------------------------
*/
 
/*
 * Implementation of Benchmark Network Layer
 *  
 * */ 
 
#include "RoutingApi.h" 
#include "RoutingBenchmark.h"

int 
RoutingBenchmark::send_msg(HelperPacket &hp)
{
    tx_q.push(hp); 
    return 0; 
}

int 
RoutingBenchmark::local_packets_available(void)
{
    return rx_q.size(); 
}

HelperPacket 
RoutingBenchmark::rx_local_packet(void) 
{
    HelperPacket pkt = rx_q.front(); 
    rx_q.pop(); 
    return pkt; 
}

//cross-layer functionality 
void 
RoutingBenchmark::write_beacon_data(HelperPacket &hp) 
{
}

int 
RoutingBenchmark::process_beacon_data(HelperPacket &hp)  
{
}

int 
RoutingBenchmark::mac_recv(HelperPacket &hp) 
{
    rx_q.push(hp); 
}
int 
RoutingBenchmark::packets_to_route(void) 
{
    return tx_q.size(); 
}

int 
RoutingBenchmark::route(void)
{
    int ret = -1; 
    if (tx_q.size()) {
        HelperPacket pkt = tx_q.front(); 
        dest_id = pkt.get_header().finaldst; 
        ret = 0; 
    }
    return ret; 
}

uint8_t 
RoutingBenchmark::get_destination_id(void) 
{
    return dest_id; 
}

HelperPacket 
RoutingBenchmark::pop_routed_packet(void) 
{
    HelperPacket pkt = tx_q.front(); 
    tx_q.pop();
    return pkt;  
}

void 
RoutingBenchmark::send_beacon_data_to_app(void)
{
}


RoutingAPI * 
RoutingFactory::create_routing(uint8_t nodeid)
{
    return new RoutingBenchmark; 
}
