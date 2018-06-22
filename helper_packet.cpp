/* ------------------------------------------------------------------------------
 * Copyright 2018 ANDRO Computational Solutions, LLC.  All Rights Reserved
 * ------------------------------------------------------------------------------
*/
 
/* 
 * Implementation of HelperPacket class
 * 
 * */ 

#include "helper_packet.h" 

#include <string> 
#include <iostream> 

HelperPacket::HelperPacket(std::uint8_t finaldst, std::uint8_t msgtype, std::uint8_t ttl, std::vector<std::uint8_t> payload)
{
    //printf("Enter HelperPacket app constructor \n"); 
    std::memset(&header, 0, sizeof(header)); 
    payload_.clear(); 
    
    header.finaldst = finaldst;
    header.msgtype = msgtype;
    header.timetolive = ttl;
    
    payload_ = payload; 
}

HelperPacket::HelperPacket(std::vector<std::uint8_t> serial, std::int8_t rssi) 
{
    //printf("Enter HelperPacket PHY constructor \n"); 
    std::memset(&header, 0, sizeof(header)); 
    payload_.clear(); 
    
    std::memcpy(&header, serial.data(), sizeof(struct HelperHeader)); 
    header.rssi = rssi; 
    
    std::size_t payload_size = serial.size() - sizeof(struct HelperHeader); 
    if (payload_size) {
        payload_ = std::vector<std::uint8_t>(serial.begin() + sizeof(struct HelperHeader), serial.end()); 
    }
}

HelperPacket::HelperPacket(std::vector<std::uint8_t> serial)
{
    //printf("Enter HelperPacket serial constructor \n"); 
    std::memset(&header, 0, sizeof(header)); 
    payload_.clear(); 
    
    std::memcpy(&header, serial.data(), sizeof(struct HelperHeader)); 
    std::size_t payload_size = serial.size() - sizeof(struct HelperHeader); 
    if (payload_size) {
        payload_ = std::vector<std::uint8_t>(serial.begin() + sizeof(struct HelperHeader), serial.end()); 
    }
    
}

HelperPacket::HelperPacket(std::uint8_t macsrc, std::uint8_t macdst, std::uint8_t macmsgtype)
{
    //printf("Enter HelperPacket MAC constructor \n"); 
    std::memset(&header, 0, sizeof(header));
    payload_.clear();  
    
    header.macsrc = macsrc; 
    header.macdst = macdst; 
    header.macmsgtype = macmsgtype; 
}

HelperPacket::~HelperPacket(void)
{
    //printf("Enter HelperPacket::~HelperPacket \n"); 
} 

void 
HelperPacket::write_network_header(std::uint8_t origsrc, std::uint8_t pktid, std::uint8_t seqid, std::uint8_t seqtot)
{
    //printf("Enter HelperPacket::write_network_header \n");
    header.origsrc = origsrc; 
    header.pktid = pktid; 
    header.seqid = seqid; 
    header.seqtot = seqtot;  
}
    
void 
HelperPacket::write_mac_header(std::uint8_t macsrc, std::uint8_t macdst, std::uint8_t macmsgtype)
{
    //printf("Enter HelperPacket::write_mac_header \n");
    header.macsrc = macsrc; 
    header.macdst = macdst; 
    header.macmsgtype = macmsgtype; 
}
    
void 
HelperPacket::write_beacon_data(double latitude, double longitude, std::uint8_t utility, std::uint8_t battery, std::uint8_t priority_q_len, std::uint8_t best_effort_q_len)
{
    //printf("Enter HelperPacket::write_beacon_data \n");
    header.latitude = latitude; 
    header.longitude = longitude;
    header.utility = utility; 
    header.battery = battery; 
    header.priority_q_len = priority_q_len;
    header.best_effort_q_len = best_effort_q_len; 
}

void
HelperPacket::decrement_ttl(void)
{
    //printf("Enter HelperPacket::decrement_ttl()\n"); 
    header.timetolive--; 
}

struct HelperHeader
HelperPacket::get_header(void) 
{
    return header; 
}

std::vector<std::uint8_t> 
HelperPacket::get_payload(void)
{
    return payload_; 
}

std::vector<std::uint8_t> 
HelperPacket::serialize(void)
{
    //printf("Enter HelperPacket::serialize \n"); 
    std::vector<std::uint8_t> serial; 
    std::uint8_t *hhp = (std::uint8_t *) &header; 
    
    //insert header 
    serial.insert(serial.begin(), hhp, hhp + sizeof(struct HelperHeader)); 
    //insert payload
    if (payload_.size()) {
        serial.insert(serial.end(), payload_.begin(), payload_.end()); 
    }
    return serial; 
}

void
HelperPacket::print(void)
{
    printf("Enter HelperPacket::print \n"); 
    printf("origsrc %hhu \n", header.origsrc); 
    printf("finaldst %hhu \n", header.finaldst); 
    printf("msgtype %hhu \n", header.msgtype); 
    printf("timetolive %hhu \n", header.timetolive); 
    printf("pktid %hhu \n", header.pktid); 
    printf("seqid %hhu \n", header.seqid); 
    printf("seqtot %hhu \n", header.seqtot); 
    printf("latitude %lf \n", header.latitude); 
    printf("longitude %lf \n", header.longitude); 
    printf("utility %hhu \n", header.utility); 
    printf("battery %hhu \n", header.battery); 
    printf("priority_q_len %hhu \n", header.priority_q_len); 
    printf("best_effort_q_len %hhu \n", header.best_effort_q_len); 
    printf("macsrc %hhu \n", header.macsrc); 
    printf("macdst %hhu \n", header.macdst); 
    printf("macmsgtype %hhu \n", header.macmsgtype); 
    printf("rssi %hhd \n", header.rssi); 
    
    if (payload_.size()) {
        std::string payload (payload_.begin(), payload_.end()); 
        printf("payload bytes %d \n", payload.size()); 
        printf("payload %s \n", payload.c_str()); 
    } else { 
        printf("payload bytes 0 [no payload] \n"); 
    }
}
