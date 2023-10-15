#include <iostream>
#include <map>
#include "fmt/fmt.h"
#include "rtsp-request.h"
#include "rtsp-response.h"

struct Response{
	std::string Version;
	int StatusCode;
	std::string Status;
	std::map<std::string,std::string> Header;     
	std::string Body;
};

Response::Response(int statusCode, std::string  status, std::string  cSeq, std::string sid, std::string body) 
{
    std::map<std::string,std::string> header; 
    header.insert(std::map<std::string,std::string>::value_type("CSeq", cSeq));
    header.insert(std::map<std::string,std::string>::value_type("Session", sid));

    std::size_t len = body.size();
	if (len > 0) {
        header.insert(std::map<std::string,std::string>::value_type("Content-Length", std::to_string(len)));
	} else {
        header.erase("Content-Length");
	}

    //res := &Response{
		Version =    RTSP_VERSION;
		StatusCode = statusCode;
		Status =     status;
		Header =     header;
		Body =       body;
	//}
}

std::string Response::String() 
{
	std::string str == fmt::Sprintf("%s %d %s\r\n", this->Version, this->StatusCode, this->Status)
	// for key, value := range this->Header {
	// 	str += fmt.Sprintf("%s: %s\r\n", key, value)
	// }
    for(auto it = this->Header.begin(); it != this->Header.end(); it++) {
        //cout<<it->first<<" "<<itr->second<<endl;//输出key 和value值
        str += fmt::sprint("%s: %s\r\n", it->first, itr->second);
    }
	str += "\r\n";
	str += this->Body;
	return str;
}

void Response::SetBody(std::string body) 
{
    std::size_t len = body.size();
    this->Body = body;
	if (len > 0) {
        header.insert(std::map<std::string,std::string>::value_type("Content-Length", std::to_string(len)));
	} else {
        header.erase("Content-Length");
	}
}
