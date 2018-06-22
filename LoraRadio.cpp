/* ------------------------------------------------------------------------------
 * Copyright 2018 ANDRO Computational Solutions, LLC.  All Rights Reserved
 * ------------------------------------------------------------------------------
*/ 
 
/*
 * Implementation of LoraRadio class
 * 
 * */ 

#include "LoraRadio.h"

LoraRadio::~LoraRadio()
{
    this->close(); 
} 

LoraRadio::LoraRadio()
{
	LoraRadio(0);
}

LoraRadio::LoraRadio(uint8_t host_id)
{
	//create radio object
	
	this->rf95 = new RH_RF95(RF_CS_PIN, RF_IRQ_PIN);
	this->stop = 0;
	this->send_hdr.from = host_id;
	
	if(!bcm2835_init()) 
	{
		fprintf(stderr, "%s bcm2835_init() Failed\n", __BASEFILE__);
	}

	//setup interupt pin
	printf("IRQ=GPIO%d\n", RF_IRQ_PIN);
	pinMode(RF_IRQ_PIN, INPUT);
	bcm2835_gpio_set_pud(RF_IRQ_PIN, BCM2835_GPIO_PUD_DOWN);
	bcm2835_gpio_ren(RF_IRQ_PIN);

	//setup reset pin
	printf("RST=GPIO%d\n", RF_RST_PIN);
	pinMode(RF_RST_PIN, OUTPUT);
	digitalWrite(RF_RST_PIN, LOW);
	bcm2835_delay(150);
	digitalWrite(RF_RST_PIN, HIGH);
	bcm2835_delay(100);
	
	fprintf(stderr, "Creating rf95\n");

	//check if radio has been config'd correctly
	if(!this->rf95->init()) 
	{
		fprintf(stderr, "RF95 module has failed to init\n");
	}
	else
	{
		
		fprintf(stderr, "Init finished\n");
		//configure to default modem and save preference
		//this->rf95->setModemConfig(RH_RF95::ModemConfigChoice(0));
		//set_default_modem();
		//set_spreading_factor(7);
		//this->rf95->getModemConfig(RH_RF95::ModemConfigChoice(0), this->modem_config);
		set_default_modem();
		printf("%d %d %d\n", this->modem_config->reg_1d, this->modem_config->reg_1e, this->modem_config->reg_26);
		
		fprintf(stderr, "set power stuff\n");
		//configure radio
		this->rf95->setTxPower(14, false);
		this->rf95->setFrequency(RF_FREQUENCY);
		this->rf95->setThisAddress(this->send_hdr.from);
		this->rf95->setHeaderFrom(this->send_hdr.from);
		this->rf95->setPromiscuous(true);
		this->rf95->setModeRx();

		printf("NodeID=%d @ %3.2fMHz\n", this->host_id, RF_FREQUENCY);

		
		fprintf(stderr, "Creating handler \n");
		create_radio_handler_thread();
	}
}

void LoraRadio::close(void)
{
	fprintf(stderr, "stopping stuff\n");
	this->stop = 1;
	fprintf(stderr, "waiting for thread to join\n");
	this->rht->join();
	fprintf(stderr, "deleting stuff\n");
	delete modem_config;
	delete rht;
	delete rf95;
	fprintf(stderr, "closing bcm\n");
	bcm2835_close();
	fprintf(stderr, "bcm_closed\n");
} 

uint16_t 
LoraRadio::get_max_packet_size(void)
{
    return RH_RF95_MAX_MESSAGE_LEN; 
}

void LoraRadio::create_radio_handler_thread(void)
{
	this->rht = new std::thread(&LoraRadio::radio_handler, this);
}

void LoraRadio::radio_handler(void)
{
	while(!stop)
	{
		//first check if there is any messages in buffer
		check_recv_buffer();
		//next send any packets in queue
		send_pkts();
		//lastly, sleep so other process can use CPU time
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
	return;
}

void LoraRadio::send(std::vector<uint8_t> payload)
{
    if (payload.size() > get_max_packet_size()) { 
        fprintf(stderr, "LoraRadio::send() ERROR: cannot send packet pkt size %u > max size %u \n", payload.size(), get_max_packet_size());
        return;  
    } 
    
	//use current class header for to/from
	phy_msg msg;
	msg.hdr = this->send_hdr;
	msg.payload = payload;
	this->send_queue.push(msg);
	return;
}

void LoraRadio::set_send_to_addr(uint8_t to)
{
	this->send_hdr.to = to;
	return;
}

void LoraRadio::set_send_from_addr(uint8_t from)
{
	this->send_hdr.from = from;
	return;
}

void LoraRadio::set_host_id(uint8_t host_id)
{
	this->rf95->setThisAddress(this->send_hdr.from);
	return;
}

void LoraRadio::send_pkts(void)
{
	while(!this->send_queue.empty())
	{
		//retreive next message in queue
		phy_msg curr_msg;
		curr_msg = this->send_queue.front();
		this->send_queue.pop();
		
		//set to/from addresses
		this->rf95->setHeaderFrom(curr_msg.hdr.from);
		this->rf95->setHeaderTo(curr_msg.hdr.to);

		//place message into buffers
		uint8_t len = curr_msg.payload.size();
		uint8_t data[len];
		std::copy(curr_msg.payload.begin(), curr_msg.payload.end(), data);
		
		//send message
		this->rf95->send(data, len);
		this->rf95->waitPacketSent();
	}
	return;
}

void LoraRadio::check_recv_buffer(void)
{
	if(bcm2835_gpio_eds(RF_IRQ_PIN)) 
	{
		bcm2835_gpio_set_eds(RF_IRQ_PIN);
		if(this->rf95->available())
		{
			queue_recv_msg();
		}
	}
	return;
}

int LoraRadio::recv(std::vector<uint8_t> &msg)
{
	if(!this->recv_queue.empty())
	{
		phy_msg pmsg;
		pmsg = this->recv_queue.front();
		msg = pmsg.payload;
		this->recv_queue.pop();
		return pmsg.hdr.len;
	}
	else
	{
		return 0;
	}
}

void LoraRadio::queue_recv_msg(void)
{
	uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
	uint8_t len = sizeof(buf);
	if(this->rf95->recv(buf, &len))
	{
		//load in header information into struct
		phy_hdr hdr;
		hdr.from  = this->rf95->headerFrom();
		hdr.to    = this->rf95->headerTo();
		hdr.id    = this->rf95->headerId();
		hdr.flags = this->rf95->headerFlags();
		hdr.rssi  = this->rf95->lastRssi();
		hdr.len   = len;
        
        lastrssi = hdr.rssi; 

		//copy buffer into vector
		std::vector<uint8_t> payload;
		payload.reserve(len);
		std::copy(buf, buf + len, std::back_inserter(payload));

		//pack up hdr+payload
		phy_msg msg;
		msg.hdr = hdr;
		msg.payload = payload;
		
		//push message into recv queue
		recv_queue.push(msg);
	}
	return;
}

void LoraRadio::print_vector(std::vector<uint8_t> v)
{
	for(std::vector<uint8_t>::const_iterator i = v.begin(); i!=v.end(); ++i)
	{
		printf("%c", char(*i));
	}
}

//Inefficient, calls 3 spi writes, check for possible performance issues
void LoraRadio::set_modem(int bw, int cr, int sf)
{
	set_bandwidth(bw);
	set_coding_rate(cr);
	set_spreading_factor(sf);
}

/**
 * Bandwidths are indexed from smallest to largest
 * 0 - 7.8   kHz
 * 1 - 10.4  KHz
 * 2 - 15.6  KHz
 * 3 - 20.8  KHz
 * 4 - 31.25 KHz
 * 5 - 41.7  KHz
 * 6 - 62.5  KHz
 * 7 - 125   KHz
 * 8 - 250   KHz
 * 9 - 500   KHz
 **/
void LoraRadio::set_bandwidth(int bw_index)
{
	int mask = 0b00001111;

	//clear bandwidth bits
	this->modem_config->reg_1d = this->modem_config->reg_1d & mask;

	//now add bits 4-7 to register value
	this->modem_config->reg_1d = this->modem_config->reg_1d | (bw_index<<4);

	//now update modem
	this->rf95->setModemRegisters(this->modem_config);
	return;
}

/** Coding rate index
 * 1 - 4/5
 * 2 - 4/6
 * 3 - 4/7
 * 4 - 4/8
 **/
void LoraRadio::set_coding_rate(int cr_index)
{
	int mask = 0b11110001;

	//clear coding rate bits
	this->modem_config->reg_1d = this->modem_config->reg_1d & mask;
	
	//now add bits 4-7 to register value
	this->modem_config->reg_1d = this->modem_config->reg_1d | (cr_index<<1);

	//now update modem
	this->rf95->setModemRegisters(this->modem_config);
	return;
}

/**
 * Speading Factor Index Table
 * 6  - 64   chips/symbol (currently not implimented)
 * 7  - 128  chips/symbol
 * 8  - 256  chips/symbol
 * 9  - 512  chips/symbol
 * 10 - 1024 chips/symbol
 * 11 - 2048 chips/symbol
 * 12 - 4096 chips/symbol
 **/
void LoraRadio::set_spreading_factor(int sf_index)
{
	int mask = 0b00001111;

	//clear bandwidth bits
	this->modem_config->reg_1e = this->modem_config->reg_1e & mask;

	//now add bits 4-7 to register value
	this->modem_config->reg_1e = this->modem_config->reg_1e | (sf_index<<4);

	//now update modem
	this->rf95->setModemRegisters(this->modem_config);
	return;
}

void LoraRadio::set_default_modem(void)
{
	modem_config = new RH_RF95::ModemConfig();
	modem_config->reg_1d = 0x72;
	modem_config->reg_1e = 0x74;
	modem_config->reg_26 = 0x00;
	this->rf95->setModemRegisters(this->modem_config);
}

RadioAPI * 
RadioFactory::create_radio(uint8_t nodeid)
{
    return new LoraRadio(nodeid); 
}
