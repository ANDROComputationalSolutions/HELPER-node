/* ------------------------------------------------------------------------------
 * Copyright 2018 ANDRO Computational Solutions, LLC.  All Rights Reserved
 * ------------------------------------------------------------------------------
*/

/*
 * Header file for Lora Radio interface (implementation class of RadioApi)
 *  
 * */ 

#ifndef LORARADIO_H
#define LORARADIO_H

#include <bcm2835.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <iostream>
#include <RH_RF95.h>
#include <cstring>
#include <thread>
#include <chrono>
#include <vector>
#include <queue>

#define BOARD_LORASPI

#include "RadioApi.h" 
#include "examples/raspi/RasPiBoards.h"
#include "LoraCommon.h"

class LoraRadio : public RadioAPI
{
public:
	LoraRadio();
	LoraRadio(uint8_t host_id);
    ~LoraRadio(); 

	int recv(std::vector<uint8_t> &msg);
	void send(std::vector<uint8_t> payload);
	void close(void);
    uint16_t get_max_packet_size(void); 

    int8_t get_last_rssi(void) {return lastrssi;}

	//public setters
	void set_send_to_addr(uint8_t to);
	void set_send_from_addr(uint8_t from);
	void set_host_id(uint8_t host_id);

	//Modem Setters
	void set_modem(int bw, int cr, int sf);
	void set_bandwidth(int bw_index);
	void set_coding_rate(int cr_index);
	void set_spreading_factor(int sf_index);

private:
	void create_radio_handler_thread(void);
	void radio_handler(void);
	void check_recv_buffer(void);
	void queue_recv_msg(void);
	void print_vector(std::vector<uint8_t> v);
	void send_pkts(void);


	RH_RF95* rf95;
	uint8_t host_id;
    int8_t lastrssi; 
	bool stop;
	std::thread *rht;
	std::queue<phy_msg> recv_queue;
	std::queue<phy_msg> send_queue;
	
	phy_hdr send_hdr;

	RH_RF95::ModemConfig *modem_config;
	void set_default_modem(void);
};

#endif
