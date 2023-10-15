#ifndef RTSP_RESPONSE_H



#include <iostream>
#include <map>
struct Response{
	std::string Version;
	int StatusCode;
	std::string Status;
	std::map<std::string,std::string> Header;     
	std::string Body;


    Response(int statusCode, std::string  status, std::string  cSeq, std::string sid, std::string body);

    std::string String();
    void SetBody(std::string body) ;
};



#endif