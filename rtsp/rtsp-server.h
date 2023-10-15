#ifndef RTSP_SERVER_H
#define RTSP_SERVER_H

#include <iostream>
#include <map>
#include <mutex>
#include <thread>
#include "pusher.h"
#include "net/net.h"

class Server {
	TcpListener tcp_listener;
	int TCPPort;     
	bool Stoped;      
	std::map<std::string, Pusher*> pushers;      // Path <-> Pusher
	std::mutex pushersLock; 
public:
    static Result<Server, std::string> NewServer(uint16_t port);

    Server(TcpListener listener, uint16_t port);
    bool Start();
    void Stop(); 
    void AddPusher(Pusher *pusher ); 
    void RemovePusher(Pusher *pusher); 
    Pusher* GetPusher(std::string path);
    std::map<std::string, Pusher*> GetPushers();
    std::size_t GetPusherSize();
};


#endif