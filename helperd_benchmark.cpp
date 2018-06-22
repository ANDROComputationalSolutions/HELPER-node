/* ------------------------------------------------------------------------------
 * Copyright 2018 ANDRO Computational Solutions, LLC.  All Rights Reserved
 * ------------------------------------------------------------------------------
*/

/* 
 * Main program for demonstrating benchmark HELPER network. 
 * 
 * Creates an instance of LoraMac class 
 * Creates two threads: 
 *  lora_handler: Monitors HELPER network stack for received messages and forwards them to application
 *  app_handler: Monitors sending applications for outgoing messages, encapsules them in HelperPacket format
 *  and forwards to network layer (service layer functionality) 
 * 
 * */ 

#include "LoraMac.h"
#include "helperMessage.h" 
#include <signal.h>
#include <zmq.hpp>

void sighandler(int sig);
void app_handler(void);
void lora_handler(void);

bool stop = 0;
LoraMac *lr;
uint8_t node_id;

void sighandler(int sig){stop = 1;}

void lora_handler(void)
{
	zmq::context_t context(1);
	zmq::socket_t send_socket(context, ZMQ_PUB);
	send_socket.connect("ipc:///tmp/to_app.ipc");
	while(!stop) {
		//read from lora, if message, send to web
        if (lr->recv_ready()) 
		{
            fprintf(stderr, "Message Recieved on lora\n");
            
            HelperPacket hp = lr->recv(); 
            
            struct HelperHeader hh = hp.get_header(); 
            double latitude = hh.latitude; 
            double longitude = hh.longitude; 
            std::string srcid = std::to_string(hh.origsrc); //placeholder
            std::string dstid = std::to_string(hh.finaldst); //placeholder
            std::string msgtype = "test"; //set this based off msg type field in HelperHeader
            
            std::uint8_t ttl = hh.timetolive; 
            
            std::vector<std::uint8_t> payload_vec = hp.get_payload(); 
            std::string payload(payload_vec.begin(), payload_vec.end()); 
            
            HelperRecv recvable =
                HelperRecvBuilder()
                .setLocation(
                    LocationBuilder()
                        .setLatitude(latitude)
                        .setLongitude(longitude)
                        .setAltitude(0)
                        .build()
                )
                .setSourceId(srcid)
                .setDestinationId(dstid)
                .setMessageType(msgtype)
                .setTimeToLive(ttl)
                .setPayload(payload)
                .build();
                
            std::string json = recvable.toJson(); 
            
            zmq::message_t send_msg(json.size()); 
			memcpy(send_msg.data(), json.data(), json.size());
			send_socket.send(send_msg);
		}
		else
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}
	}
	fprintf(stderr, "Closing lorahandler\n");
}

void app_handler(void)
{
	zmq::context_t context(1);
	zmq::socket_t recv_socket(context, ZMQ_SUB);
	recv_socket.connect("ipc:///tmp/from_app.ipc");
    recv_socket.setsockopt (ZMQ_SUBSCRIBE, "", 0);
    
    
    while(!stop) {
        //read from app, if message, send to lora
        zmq::message_t msg;
        int rv = recv_socket.recv(&msg,ZMQ_DONTWAIT);
        if(rv == true)
        {
            fprintf(stderr, "Message Received from app\n");
            auto const ptr = reinterpret_cast<uint8_t*>(msg.data());
            std::vector<uint8_t> data(ptr, ptr+msg.size());  
            
            std::string data_str(data.begin(), data.end()); 
            
            HelperSend sendable =
                HelperSendBuilder()
                    .fromJson(data_str)
                    .build();
            
            std::uint8_t finaldst; 
            std::uint8_t msgtype; //set this based on msgtype field in HelperSend 
            std::uint8_t ttl; 
            std::vector<std::uint8_t> payload(sendable.payload.begin(), sendable.payload.end()); 
            
            finaldst = std::atoi(sendable.destinationId.c_str()); 
            ttl = sendable.timeToLive; 
            HelperPacket hp(finaldst, msgtype, ttl, payload); 
            hp.write_network_header(node_id, 0, 0, 0); 
            lr->send(hp); 
        }
        else
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    }
        
   
}

int main(int argc, const char* argv[])
{
	if (argc < 2) { 
		printf("usage: sudo ./helperd_benchmark <node_id> \n"); 
		return -1;
	}
	node_id = std::atoi(argv[1]); 
	signal(SIGABRT, &sighandler);
	signal(SIGTERM, &sighandler);
    signal(SIGINT, &sighandler);	
	fprintf(stderr, "Before creating\n");
	lr = new LoraMac(node_id);

	//start threads
	std::thread *upstream_thread = new std::thread(&lora_handler);
	std::thread *app_thread = new std::thread(&app_handler);

	//wait till finished
	upstream_thread->join();
	app_thread->join();

	//cleanup
	fprintf(stderr, "Closing\n");
	delete upstream_thread;
	delete app_thread;
	lr->close();
	delete lr;
	return 0;
}
