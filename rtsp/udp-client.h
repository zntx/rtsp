#ifndef UDP_CLIENT_H
#define UDP_CLIENT_H

#include <iostream>
#include <map>
#include <mutex>
#include <thread>
#include <stdint.h>
#include "fmt/fmt.h"
#include "net/net.h"
#include "rtsp-server.h"
#include "rtsp-session.h"

class UDPClient{
	
	Session*  session;
	int       APort;        
	UdpSocket AConn;
	int       AControlPort;
	UdpSocket AControlConn;
	int       VPort ;       
	UdpSocket VConn;
	int       VControlPort;
	UdpSocket VControlConn;
	bool      Stoped;
public:
    void Stop();
    void SetupAudio();
    void SetupVideo();
    void SendRTP(RTPPack *pack);

};


#endif