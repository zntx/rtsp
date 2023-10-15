#ifndef _RTSP_SESSION_H
#define _RTSP_SESSION_H

#include <iostream>
#include <time.h>
#include "pusher.h"
#include "player.h"
#include "rtsp-server.h"
#include "rtsp-request.h"
#include "rtsp-response.h"
#include "sdp-parser.h"
#include "udp-client.h"
#include "net/net.h"
#include "buff/buff.h"

enum class SessionType {
	SESSION_TYPE_PUSHER,
	SESSEION_TYPE_PLAYER
};

inline
std::string SessionType_String(SessionType st)  
{
	switch (st) {
	case SessionType::SESSION_TYPE_PUSHER:
		return "pusher";
	case SessionType::SESSEION_TYPE_PLAYER:
		return "player";
	}
	return "unknow";
}

enum class RTPType {
	RTP_TYPE_AUDIO,
	RTP_TYPE_VIDEO,
	RTP_TYPE_AUDIOCONTROL,
	RTP_TYPE_VIDEOCONTROL
};

inline
std::string RTPType_String( RTPType rt) 
{
	switch (rt) {
	case RTPType::RTP_TYPE_AUDIO:
		return "audio";
	case RTPType::RTP_TYPE_VIDEO:
		return "video";
	case RTPType::RTP_TYPE_AUDIOCONTROL:
		return "audio control";
	case RTPType::RTP_TYPE_VIDEOCONTROL:
		return "video control";
	}
	return "unknow";
}

enum class TransType {
	TRANS_TYPE_TCP,
	TRANS_TYPE_UDP
};

inline
std::string TransType_String(TransType tt)
{
	switch (tt) {
	case TransType::TRANS_TYPE_TCP:
		return "TCP";
	case TransType::TRANS_TYPE_UDP:
		return "UDP";
	}
	return "unknow";
}

struct RTPPack {
	RTPType Type;   
	Slice<uint8_t> Buffer;
};

class Session {

	std::string ID;
	Server     *server;
	TcpStream   Conn;
	//connRW    *bufio.ReadWriter
	std::mutex  connWLock;
	SessionType Type;     
	TransType   transType; 
	std::string Path;
	std::string URL;
	std::string SDPRaw;
	std::map<std::string, SDPInfo*> SDPMap;

	std::string AControl;
	std::string VControl;
	std::string ACodec;
	std::string VCodec;

	// stats info
	int InBytes;
	int OutBytes;
	time_t StartAt;
	int Timeout;

	bool Stoped; 

	//tcp channels
	int aRTPChannel;
	int aRTPControlChannel;
	int vRTPChannel;
	int vRTPControlChannel;

	Pusher*    pusher;
	Player*    player;
	UDPClient* udpclient;
	//RTPHandles  []func(*RTPPack)
	//StopHandles []func()

public:
    std::string String();

    Session( Server *server, TcpStream conn );

    void Stop() ;

    void Start() ;

    void handleRequest(Request *req);

    void SendRTP(RTPPack *pack);

};

#endif