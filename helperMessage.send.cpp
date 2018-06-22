/* ------------------------------------------------------------------------------
 * Copyright 2018 ANDRO Computational Solutions, LLC.  All Rights Reserved
 * ------------------------------------------------------------------------------
*/
 
/*
 * Implementation of service layer HelperSend class
 * */ 

#include "helperMessage.h"
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

HelperSend::HelperSend(
    const string dst,
    const string type,
    const int seconds,
    const string bytes
) : destinationId(dst),
    messageType(type),
    timeToLive(seconds),
    payload(bytes)
{}

string HelperSend::toJson() {
    using namespace rapidjson;
    Document document;
    document.SetObject();
    Document::AllocatorType& alloc = document.GetAllocator();
    StringBuffer buffer;
    Writer<StringBuffer> writer (buffer);

    document.AddMember("destinationId", StringRef(destinationId.c_str()), alloc);
    document.AddMember("messageType", StringRef(messageType.c_str()), alloc);
    document.AddMember("timeToLive", timeToLive, alloc);
    document.AddMember("payload", StringRef(payload.c_str()), alloc);

    document.Accept(writer);
    
    // NOTE: this json is in utf-8 
    return std::string(buffer.GetString()); 
}

HelperSendBuilder& HelperSendBuilder::setDestinationId(const string dst) {
    destinationId = dst;
    return *this;
}

HelperSendBuilder& HelperSendBuilder::setMessageType(const string type) {
    messageType = type;
    return *this;
}

HelperSendBuilder& HelperSendBuilder::setTimeToLive(const int seconds) {
    timeToLive = seconds;
    return *this;
}

HelperSendBuilder& HelperSendBuilder::setPayload(const string bytes) {
    payload = bytes;
    return *this;
}

HelperSendBuilder& HelperSendBuilder::fromJson(const string json) {
    using namespace rapidjson;
    Document document;
    document.Parse(json.c_str());

    // TODO: error check and exception handling
    (*this)
        .setDestinationId(string(document["destinationId"].GetString()))
        .setMessageType(string(document["messageType"].GetString()))
        .setTimeToLive(document["timeToLive"].GetInt())
        .setPayload(string(document["payload"].GetString()));

    return *this;
}

HelperSend HelperSendBuilder::build() {
    return HelperSend(
        destinationId, messageType, timeToLive, payload
    );
}
