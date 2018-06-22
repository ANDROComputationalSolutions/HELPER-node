/* ------------------------------------------------------------------------------
 * Copyright 2018 ANDRO Computational Solutions, LLC.  All Rights Reserved
 * ------------------------------------------------------------------------------
*/
 
/* 
 * Implementation of service layer HelperRecv message class
 * 
 * */ 

#include "helperMessage.h"
#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

HelperRecv::HelperRecv(
    const Location coordinate,
    const string src,
    const string dst,
    const string type,
    const int hops,
    const string bytes
) : location(coordinate),
    sourceId(src),
    destinationId(dst),
    messageType(type),
    timeToLive(hops),
    payload(bytes)
{}

string HelperRecv::toJson() {
    rapidjson::Document document;
    document.SetObject();
    rapidjson::Document::AllocatorType& alloc = document.GetAllocator();
    rapidjson::StringBuffer buffer;
    rapidjson::Writer<rapidjson::StringBuffer> writer (buffer);

    rapidjson::Value locationObject(rapidjson::kObjectType);
    locationObject.AddMember("longitude", location.getLongitude(), alloc);
    locationObject.AddMember("latitude", location.getLatitude(), alloc);
    locationObject.AddMember("altitude", location.getAltitude(), alloc);
    document.AddMember("location", locationObject, alloc);
    document.AddMember("sourceId", rapidjson::StringRef(sourceId.c_str()), alloc);
    document.AddMember("destinationId", rapidjson::StringRef(destinationId.c_str()), alloc);
    document.AddMember("messageType", rapidjson::StringRef(messageType.c_str()), alloc);
    document.AddMember("timeToLive", rapidjson::Value(timeToLive), alloc);
    document.AddMember("payload", rapidjson::StringRef(payload.c_str()), alloc);

    document.Accept(writer);
    
    // NOTE: this json is in utf-8 
    return std::string(buffer.GetString()); 
 
}

HelperRecvBuilder& HelperRecvBuilder::setLocation(const Location coordinate) {
    location = coordinate;
    return *this;
}

HelperRecvBuilder& HelperRecvBuilder::setSourceId(const string src) {
    sourceId = src;
    return *this;
}

HelperRecvBuilder& HelperRecvBuilder::setDestinationId(const string dst) {
    destinationId = dst;
    return *this;
}

HelperRecvBuilder& HelperRecvBuilder::setMessageType(const string type) {
    messageType = type;
    return *this;
}

HelperRecvBuilder& HelperRecvBuilder::setTimeToLive(const int hops) {
    timeToLive = hops;
    return *this;
}

HelperRecvBuilder& HelperRecvBuilder::setPayload(const string bytes) {
    payload = bytes;
    return *this;
}

HelperRecvBuilder& HelperRecvBuilder::fromJson(const string json) {
    rapidjson::Document document;
    document.Parse(json.c_str());

    LocationBuilder locationBuilder;

    locationBuilder
        .setLongitude(document["location"]["longitude"].GetDouble())
        .setLatitude(document["location"]["latitude"].GetDouble())
        .setAltitude(document["location"]["altitude"].GetDouble());

    (*this)
        .setLocation(locationBuilder.build())
        .setSourceId(string(document["sourceId"].GetString()))
        .setDestinationId(string(document["destinationId"].GetString()))
        .setMessageType(string(document["messageType"].GetString()))
        .setTimeToLive(document["timeToLive"].GetInt())
        .setPayload(string(document["payload"].GetString()));

    return *this;
}

HelperRecv HelperRecvBuilder::build() {
    return HelperRecv(
        location, sourceId, destinationId,
        messageType, timeToLive, payload
    );
}

Location::Location(
    const double eastWest,
    const double northSouth,
    const double seaLevel
) : longitude(eastWest),
    latitude(northSouth),
    altitude(seaLevel)
{}

Location::Location() : longitude (0), latitude (0), altitude (0) {}

double Location::getLongitude() {
	return longitude;
}

double Location::getLatitude() {
	return latitude;
}

double Location::getAltitude() {
	return altitude;
}

LocationBuilder& LocationBuilder::setLongitude(const double degrees) {
    this->longitude = degrees;
    return *this;
}

LocationBuilder& LocationBuilder::setLatitude(const double degrees) {
    this->latitude = degrees;
    return *this;
}

LocationBuilder& LocationBuilder::setAltitude(const double degrees) {
    this->altitude = degrees;
    return *this;
}

Location LocationBuilder::build() {
    return Location(longitude, latitude, altitude);
}
