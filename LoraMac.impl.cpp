/* ------------------------------------------------------------------------------
 * Copyright 2018 ANDRO Computational Solutions, LLC.  All Rights Reserved
 * ------------------------------------------------------------------------------
*/
 
/*
 * Implementation of LoraMac class
 * 
 * */ 

#include "LoraMac.h"

LoraMac::LoraMac(uint8_t host_id)
{
	fprintf(stderr, "Creating loramac\n");
    RadioFactory radio_factory; 
    this->LR = radio_factory.create_radio(host_id); 
    this->state = IDLE;
    RoutingFactory rf; 
    this->routing = rf.create_routing(host_id); 
    this->host_id = host_id;  
	create_mac_thread();
}

void LoraMac::send(HelperPacket pkt)
{
    routing->send_msg(pkt); 
    return; 
}

int LoraMac::recv_ready(void)
{
    return routing->local_packets_available(); 
}

HelperPacket LoraMac::recv(void)
{
    return routing->rx_local_packet();
}

void LoraMac::close(void)
{
	printf("Closing\n");
	this->stop = 1;
	this->mac_thread->join();
    delete this->LR; 
    delete this->routing; 
	return;
}

void LoraMac::create_mac_thread(void)
{
	this->mac_thread = new std::thread(&LoraMac::mac_handler, this);
}

void LoraMac::mac_handler(void)
{
    auto last_ctrl_sent = std::chrono::steady_clock::now();
    auto last_app_update = std::chrono::steady_clock::now();
	while(!stop)
	{
		switch(state)
		{
			case IDLE: 
			{
				//first check if there is packets received
				std::vector<uint8_t> recv_buf;
                int retval; 
                int8_t rssi; 
                retval = this->LR->recv(recv_buf); 
                rssi = this->LR->get_last_rssi(); 
                if (retval)
				{
                    HelperPacket rhp(recv_buf, rssi); 
                    struct HelperHeader hh = rhp.get_header(); 
					//check if message is RTS
                    if (hh.macmsgtype == RTS && hh.macdst == host_id) 
					{
						printf("Received RTS message\n");
						//RTS received, construct CTS
                
                        HelperPacket cts_pkt(host_id, hh.macsrc, uint8_t(CTS));  
                        routing->write_beacon_data(cts_pkt); 
						
                        std::vector<std::uint8_t> cts_pkt_phy = cts_pkt.serialize(); 
                        
                        //Call send to radio
						printf("Sending CTS\n");
                        this->LR->send(cts_pkt_phy); 
                        last_ctrl_sent = std::chrono::steady_clock::now();

						//set state to waiting for DATA
						this->state = DATA_WAIT;
						DATA_timer = 0;
                        
                        routing->process_beacon_data(rhp); 
						
					} else if (hh.macmsgtype == BEACON) { 
                        printf("received BEACON \n"); 
                        routing->process_beacon_data(rhp); 
                    } else if (hh.macmsgtype == BROADCAST_DATA) { 
                        printf("received BROADCAST DATA packet \n"); 
                        routing->mac_recv(rhp); 
                    } 
				}
                
				//else check if there is a message to send
                else if (routing->packets_to_route()) { 
                    if (routing->route() == 0) { 
                        
                        printf("Sucessful Route \n"); 
                        if (routing->get_destination_id() != BROADCAST_ADDR) { 
                            //successful route, send RTS 
                            printf("RTS destination %hhu \n", routing->get_destination_id()); 
                            
                            HelperPacket rts(host_id, routing->get_destination_id(), uint8_t(RTS)); 
                            routing->write_beacon_data(rts); 
                            
                            std::vector<uint8_t> rts_phy = rts.serialize(); 
                            this->LR->send(rts_phy); 
                            printf("Sent RTS \n"); 
                            last_ctrl_sent = std::chrono::steady_clock::now();
                            CTS_timer = 0; 
                            state = CTS_WAIT; 
                        } else { 
                            
                            printf("Sending Broadcast DATA packet \n"); 
                            //routed packet is a broadcast 
                            HelperPacket data_pkt = routing->pop_routed_packet(); 
                            data_pkt.write_mac_header(host_id, uint8_t(BROADCAST_ADDR), uint8_t(BROADCAST_DATA)); 
                        
                            std::vector<uint8_t> data_pkt_phy = data_pkt.serialize(); 

                            //send pkt 
                            this->LR->send(data_pkt_phy); 
                        } 
                        
                                
                    }
                     
                } else { 
                    auto current_time = std::chrono::steady_clock::now();
                    int elapsed_seconds = std::chrono::duration_cast<std::chrono::seconds>(current_time - last_ctrl_sent).count();
                    if (elapsed_seconds >= 5) { 
                      
                        HelperPacket beacon(host_id, 255, uint8_t(BEACON)); 

                        routing->write_beacon_data(beacon); 
            
                        std::vector<uint8_t> beacon_phy = beacon.serialize(); 
                        this->LR->send(beacon_phy); 
                        printf("sent beacon \n"); 
                            
                        last_ctrl_sent = std::chrono::steady_clock::now();
                    } 
                }
                
				break;
			}
			case CTS_WAIT:
			{
				//first check if there is packets received
				std::vector<uint8_t> recv_buf;
				int retval; 
                int8_t rssi; 
                retval = this->LR->recv(recv_buf);
                rssi = this->LR->get_last_rssi();
                if (retval) { 
                    HelperPacket rhp(recv_buf, rssi); 
                    struct HelperHeader hh = rhp.get_header(); 
                    if (hh.macmsgtype == CTS && hh.macsrc == routing->get_destination_id() && hh.macdst == host_id) {
                        printf("Received CTS \n"); 
            
                        //CTS received, construct DATA pkt
                        //get the DATA packet and write its MAC header 
                        
                        HelperPacket data_pkt = routing->pop_routed_packet(); 
                        data_pkt.write_mac_header(host_id, routing->get_destination_id(), uint8_t(DATA)); 
                        
                        std::vector<uint8_t> data_pkt_phy = data_pkt.serialize(); 
                        
                        //send pkt 
                        this->LR->send(data_pkt_phy); 
                        //transition to ACK_WAIT
                        state = ACK_WAIT; 
                        ACK_timer = 0; 
                        
                        routing->process_beacon_data(rhp); 
                    } else if (hh.macmsgtype == BEACON) { 
                        printf("received BEACON \n"); 
                        routing->process_beacon_data(rhp); 
                    } else if (hh.macmsgtype == BROADCAST_DATA) { 
                        printf("received BROADCAST DATA packet \n"); 
                        routing->mac_recv(rhp); 
                    } 
                }    
                    
				else if(CTS_timer==100)
				{
					printf("CTS Timeout\n");
					state = IDLE;
				}
				else
				{
					CTS_timer++;
				}
				break;
			}
			case DATA_WAIT:
			{
				//first check if there is packets received
				std::vector<uint8_t> recv_buf;
				int retval; 
                int8_t rssi; 
                retval = this->LR->recv(recv_buf);
                rssi = this->LR->get_last_rssi();
                if (retval) { 
                    HelperPacket rhp(recv_buf, rssi); 
                    struct HelperHeader hh = rhp.get_header(); 
                    if (hh.macmsgtype == DATA && hh.macdst == host_id) { 
                        uint8_t src = hh.macsrc; 
                        routing->mac_recv(rhp); 
            
                        HelperPacket ack(host_id, src, uint8_t(ACK)); 
                        
                        std::vector<uint8_t> ack_pkt = ack.serialize(); 
            
                        printf("received DATA pushed pkt to rx q and sending ACK \n"); 
            
                        this->LR->send(ack_pkt); 
                        this->state = IDLE; 
                        
                    } else if (hh.macmsgtype == BEACON) { 
                        printf("received BEACON \n"); 
                        routing->process_beacon_data(rhp); 
                    } else if (hh.macmsgtype == BROADCAST_DATA) { 
                        printf("received BROADCAST DATA packet \n"); 
                        routing->mac_recv(rhp); 
                    } 		 
                } 
				else if(DATA_timer==100)
				{
					printf("DATA Timeout\n");
					state = IDLE;
				}
				else
				{
					DATA_timer++;
				}	
				break;
			}
			case ACK_WAIT:
			{
				//first check if there is packets received
				std::vector<uint8_t> recv_buf;
				int retval; 
                int8_t rssi; 
                retval = this->LR->recv(recv_buf);
                rssi = this->LR->get_last_rssi();
				if(retval)
				{
                    HelperPacket rhp(recv_buf, rssi); 
                    struct HelperHeader hh = rhp.get_header(); 
                
					//check if message is ACK
					if (hh.macmsgtype == ACK && hh.macdst == host_id)
					{
						printf("Received ACK message\n");
						//ACK message received, move back to IDLE
						this->state = IDLE;
                    } else if (hh.macmsgtype == BEACON) { 
                        printf("received BEACON \n"); 
                        routing->process_beacon_data(rhp); 
                    } else if (hh.macmsgtype == BROADCAST_DATA) { 
                        printf("received BROADCAST DATA packet \n"); 
                        routing->mac_recv(rhp); 
                    } 		 
                                        
				}
				//placeholder for now, would be retransmit logic
				else if(ACK_timer==100)
				{
					printf("ACK Timeout\n");
					state = IDLE;
				}
				else
				{
					ACK_timer++;
				}	
				break;
			}
			default:
			{
				state = IDLE;
				break;
			}
		}
        
        auto curr_time = std::chrono::steady_clock::now();
        int elapsed_sec = std::chrono::duration_cast<std::chrono::seconds>(curr_time - last_app_update).count();
        if (elapsed_sec >= 5) { 
            last_app_update = curr_time; 
            routing->send_beacon_data_to_app(); 
        
        }
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
	return;
}

void LoraMac::print_vector(std::vector<uint8_t> v)
{
	for(std::vector<uint8_t>::const_iterator i = v.begin(); i!=v.end(); ++i)
	{
		printf("%c", char(*i));
	}
}
