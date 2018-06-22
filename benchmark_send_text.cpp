/* ------------------------------------------------------------------------------
 * Copyright 2018 ANDRO Computational Solutions, LLC.  All Rights Reserved
 * ------------------------------------------------------------------------------
*/

/* 
 * Benchmark application for sending text messages 
 * 
 * Provides an example of creating a service layer HelperSend message
 * Sends HelperSend message in JSON format to helperd_benchmark (service layer)
 * Service layer pushes message to network layer
 * 
 * Usage: ./benchmark_send_text <destid> 
 * <destid> is the node id of the destination node
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
    if (argc < 2) { 
        printf("USAGE: ./benchmark_send_text <dst_id> \n"); 
        return -1;
    } 
    std::string dstid(argv[1]); 
    std::string messageType = "test"; 
    std::uint8_t ttl = 0; //this is set by helperd 
    std::string payload; 
    
    zmq::context_t context(1);
    zmq::socket_t socket(context, ZMQ_PUB);
    socket.bind("ipc:///tmp/from_app.ipc");
    
    while (!stop) { 
        std::string msg; 
        std::cout << "enter a message or 'quit' to exit" << std::endl; 
        std::getline(std::cin, msg); 
        msg.erase(std::remove(msg.begin(), msg.end(), '\n'), msg.end()); 
        
        if (msg.find("quit") != -1) { 
            break; 
        } 
        
        payload = msg; 
        
        HelperSend sendable =
		HelperSendBuilder()
			.setDestinationId(dstid)
			.setMessageType(messageType)
			.setTimeToLive(ttl)
			.setPayload(payload)
			.build();
    
        std::string msg_buf = sendable.toJson(); 
        zmq::message_t zmq_msg(msg_buf.size());
        memcpy(zmq_msg.data(), msg_buf.data(), msg_buf.size());
        socket.send(zmq_msg);
    } 
    
    return 0; 
} 

