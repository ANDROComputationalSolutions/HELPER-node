/* ------------------------------------------------------------------------------
 * Copyright 2018 ANDRO Computational Solutions, LLC.  All Rights Reserved
 * ------------------------------------------------------------------------------
*/ 

#ifndef LORAMAC_H
#define LORAMAC_H

#include "LoraCommon.h"
#include "LoraApi.h"
#include "RadioApi.h" 
#include "RoutingApi.h" 
#include "helper_packet.h" 

#include <thread>

class LoraMac : public LoraApi
{	
public:
	LoraMac(uint8_t host_id);

    int recv_ready(void); 
    HelperPacket recv(void);
    void send(HelperPacket pkt); 
    
	void close(void);
	
	void print_vector(std::vector<uint8_t> v);

private:
	enum MAC_STATE {IDLE, CTS_WAIT, DATA_WAIT, ACK_WAIT};
    RadioAPI *LR; 
	bool stop = 0;
	uint8_t host_id;
	MAC_STATE state;
	std::thread *mac_thread;
	void create_mac_thread(void);

	void mac_handler(void);

    RoutingAPI *routing; 

	int CTS_timer;
	int DATA_timer;
	int ACK_timer;
};

#endif
