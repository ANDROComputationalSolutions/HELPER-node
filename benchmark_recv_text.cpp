/* ------------------------------------------------------------------------------
 * Copyright 2018 ANDRO Computational Solutions, LLC.  All Rights Reserved
 * ------------------------------------------------------------------------------
*/


/* 
 * Benchmark application for receiving text messages 
 * Messages are received from helperd_benchmark (service layer) over ZMQ
 * in HelperRecv (JSON) format
 * Message is printed in JSON format and payload is printed separately. 
 * 
 * Usage: ./benchmark_recv_text
 * 
 * */ 


#include <signal.h>
#include <iostream> 
#include <algorithm> 
#include <zmq.hpp>
 
#include "helperMessage.h" 

bool stop = 0;
void sighandler(int sig);

void sighandler(int sig){stop = 1;}

int main (int argc, char **argv) 
{ 
    zmq::context_t context(1);
    zmq::socket_t socket(context, ZMQ_SUB);
    socket.bind("ipc:///tmp/to_app.ipc");
    socket.setsockopt(ZMQ_SUBSCRIBE, "", 0); 
    
    int rxcount = 0; 
    while (!stop) { 
        zmq::message_t msg; 
        try {
            socket.recv(&msg); 
        } catch (zmq::error_t& e) { 
            exit(1); 
        }
        std::vector<uint8_t> msg_buf(msg.size()); 
        std::memcpy(msg_buf.data(), msg.data(), msg.size()); 
        
        std::string msg_str(msg_buf.begin(), msg_buf.end()); 
        
        HelperRecv recvable =
		HelperRecvBuilder()
			.fromJson(msg_str)
			.build();
        
        rxcount++; 
        printf("recv'd message [rx count = %d] \n", rxcount); 
        printf("JSON: %s \n", msg_str.c_str()); 
        printf("message payload: %s \n", recvable.payload.c_str()); 
	    printf("\n"); 
    } 
    
    return 0; 
} 
