/* ------------------------------------------------------------------------------
 * Copyright 2018 ANDRO Computational Solutions, LLC.  All Rights Reserved
 * ------------------------------------------------------------------------------
*/
 
/* 
 * Defines Service Layer message types
 * 
 * */ 

#ifndef HELPER_MESSAGE_H_
#define HELPER_MESSAGE_H_

#include <string>

using std::string;

class HelperSend {

	public:
		friend class HelperSendBuilder;
		string toJson();

		string destinationId;
		string messageType;
		int timeToLive;
		string payload;
	private:	
		HelperSend(
			const string,
			const string,
			const int,
			const string
		);

};

class Location {

	public:
		friend class LocationBuilder;
		Location();
		double getLongitude();
		double getLatitude();
		double getAltitude();

	private:
		double longitude;
		double latitude;
		double altitude;

		Location(
			const double,
			const double,
			const double
		);
};

class HelperRecv {

	public:
		friend class HelperRecvBuilder;
		string toJson();

	//private:
		Location location;
		string sourceId;
		string destinationId;
		string messageType;
		int timeToLive;
		string payload;
    private:
		HelperRecv(
			const Location,
			const string,
			const string,
			const string,
			const int,
			const string
		);

};

class LocationBuilder {

	public:
		LocationBuilder& setLongitude(const double);
		LocationBuilder& setLatitude(const double);
		LocationBuilder& setAltitude(const double);
		Location build();

	private:
		double longitude;
		double latitude;
		double altitude;

};

class HelperRecvBuilder {

	public:
		HelperRecvBuilder& setLocation(const Location);
		HelperRecvBuilder& setSourceId(const string);
		HelperRecvBuilder& setDestinationId(const string);
		HelperRecvBuilder& setMessageType(const string);
		HelperRecvBuilder& setTimeToLive(const int);
		HelperRecvBuilder& setPayload(const string);
		HelperRecvBuilder& fromJson(const string);
		HelperRecv build();

	private:
		Location location;
		string sourceId;
		string destinationId;
		string messageType;
		int timeToLive;
		string payload;

};

class HelperSendBuilder {

	public:
		HelperSendBuilder& setDestinationId(const string);
		HelperSendBuilder& setMessageType(const string);
		HelperSendBuilder& setTimeToLive(int);
		HelperSendBuilder& setPayload(const string);
		HelperSendBuilder& fromJson(const string);
		HelperSend build();

	private:
		string destinationId;
		string messageType;
		int timeToLive;
		string payload;

};

#endif
