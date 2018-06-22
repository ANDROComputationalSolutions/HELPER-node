/* ------------------------------------------------------------------------------
 * Copyright 2018 ANDRO Computational Solutions, LLC.  All Rights Reserved
 * ------------------------------------------------------------------------------
*/
 
/*
 * Interface definition to HELPER network stack
 * */ 

#ifndef LORAAPI_H
#define LORAAPI_H

#include <vector>
#include <stdint.h>

#include "LoraCommon.h" 
#include "helper_packet.h"

class LoraApi
{	
public:
    virtual int recv_ready(void) = 0; 
	virtual HelperPacket recv(void) = 0; 
	virtual void send(HelperPacket pkt) = 0;
	virtual void close(void) = 0;
	
	virtual void print_vector(std::vector<uint8_t> v) = 0;
};

#endif
