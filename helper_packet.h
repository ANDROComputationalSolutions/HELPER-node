/* ------------------------------------------------------------------------------
 * Copyright 2018 ANDRO Computational Solutions, LLC.  All Rights Reserved
 * ------------------------------------------------------------------------------
*/

/*
 * Definition of HelperPacket and HelperHeader class/structure
 * */ 

#ifndef HELPER_PACKET_H
#define HELPER_PACKET_H

#include <cstring>
#include <cstdint>
#include <vector> 

struct HelperHeader
{ 
    //app/network layer fields
    std::uint8_t origsrc;
    std::uint8_t finaldst;
    std::uint8_t msgtype;
    std::uint8_t timetolive;
    std::uint8_t pktid; 
    std::uint8_t seqid; 
    std::uint8_t seqtot; 

    //src node control info
    double latitude;
    double longitude; 
    std::uint8_t utility;
    std::uint8_t battery; 
    std::uint8_t priority_q_len;
    std::uint8_t best_effort_q_len;  

    //mac layer fields 
    std::uint8_t macsrc;
    std::uint8_t macdst; 
    std::uint8_t macmsgtype; 

    //phy layer fields
    std::int8_t rssi; 
} __attribute__((__packed__)); 

class HelperPacket { 
public: 
    //app layer constructor 
    HelperPacket(std::uint8_t finaldst, std::uint8_t msgtype, std::uint8_t ttl, std::vector<std::uint8_t> payload); 
    //phy layer constructor
    HelperPacket(std::vector<std::uint8_t> serial, std::int8_t rssi); 
    //mac layer constructor
    HelperPacket(std::uint8_t macsrc, std::uint8_t macdst, std::uint8_t macmsgtype); 
    //serialized constructor
    HelperPacket(std::vector<std::uint8_t> serial); 

    ~HelperPacket(void); 
    
    void write_network_header(std::uint8_t origsrc, std::uint8_t pktid, std::uint8_t seqid, std::uint8_t seqtot); 
    
    void write_mac_header(std::uint8_t macsrc, std::uint8_t macdst, std::uint8_t macmsgtype); 
    
    void write_beacon_data(double latitude, double longitude, std::uint8_t utility, std::uint8_t battery, std::uint8_t priority_q_len, std::uint8_t best_effort_q_len); 
    
    void decrement_ttl(void); 
    
    std::vector<std::uint8_t> serialize(void); 
    
    void print(void); 
    
    std::vector<std::uint8_t> get_payload(void); 
    
    struct HelperHeader get_header(void); 
    
private: 
    struct HelperHeader header; 
    std::vector<std::uint8_t> payload_; 
}; 

#endif 
