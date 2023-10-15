#ifndef RTSP_REQUEST_H
#define RTSP_REQUEST_H

#include <iostream>
#include <map>


#define RTSP_VERSION "RTSP/1.0"

	// Client to server for presentation and stream objects; recommended
#define DESCRIBE = "DESCRIBE"
	// Bidirectional for client and stream objects; optional
#define ANNOUNCE = "ANNOUNCE"
	// Bidirectional for client and stream objects; optional
#define GET_PARAMETER = "GET_PARAMETER"
	// Bidirectional for client and stream objects; required for Client to server, optional for server to client
#define OPTIONS = "OPTIONS"
	// Client to server for presentation and stream objects; recommended
#define PAUSE = "PAUSE"
	// Client to server for presentation and stream objects; required
#define PLAY = "PLAY"
	// Client to server for presentation and stream objects; optional
#define RECORD = "RECORD"
	// Server to client for presentation and stream objects; optional
#define REDIRECT = "REDIRECT"
	// Client to server for stream objects; required
#define SETUP = "SETUP"
	// Bidirectional for presentation and stream objects; optional
#define SET_PARAMETER = "SET_PARAMETER"
	// Client to server for presentation and stream objects; required
#define TEARDOWN = "TEARDOWN"
#define DATA     = "DATA"


class Request{
	std::string Method;
	std::string URL;
	std::string Version;
	std::map<std::string, std::string> Header;  
	std::string Conten;
	std::string Body;

public :
    static Request* NewRequest(std::string content);
    
    Request(std::string method, std::string url, std::string version, std::string heard, std::string count );
    std::string String();
    int GetContentLength();
};


#endif