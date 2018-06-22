/* ------------------------------------------------------------------------------
 * Copyright 2018 ANDRO Computational Solutions, LLC.  All Rights Reserved
 * ------------------------------------------------------------------------------
*/
 
/*
 * Common header file for defining 
 * constants, message types, etc.
 * 
 * */ 

#ifndef LORACOMMON_H
#define LORACOMMON_H

#include <vector>
#include <cstdint> 
#include <cstdlib> 

#define RF_FREQUENCY  915.00

//traffic classes 
#define PRIORITY 1 
#define BEST_EFFORT 2 

#define BROADCAST_ADDR 255
#define RESPONDER_STATION_ADDR 254


#define MAX_TTL 1
#define NEIGHBOR_TTL 1 
#define HELP_TTL (MAX_TTL)
#define HELP_BCAST_TTL 2 
#define RESOURCE_TTL (MAX_TTL)
#define RESOURCE_UPDATE_TTL (MAX_TTL) 
#define RS_INIT_TTL (MAX_TTL)
#define RS_UPDATE_TTL (MAX_TTL)
#define NODE_UPDATE_TTL (MAX_TTL)
#define UNICAST_TTL 2

//beacon, rts, cts, data, ack, broadcast_data are used at the MAC/NW layer
//the rest are used at APP/NW layer 
enum MSG_ID { 
    //MAC/NW layer
    BEACON, 
    RTS, 
    CTS, 
    DATA, 
    ACK, 
    BROADCAST_DATA,
    // APP/NW layer 
    NEIGHBORS, 
    HELP,
    RESOURCE,
    RESOURCE_UPDATE,
    RS_INIT, 
    RS_UPDATE, 
    NODE_UPDATE, 
    UNICAST 
}; 

struct phy_hdr {
	uint8_t from;
	uint8_t to;
	uint8_t id;
	uint8_t flags;
	int8_t rssi;
	uint8_t len;
} __attribute__((__packed__));

struct phy_msg {
	phy_hdr hdr;
	std::vector<uint8_t> payload;
};

#endif
