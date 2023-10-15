#ifndef RTSP_PUSH_H_H
#define RTSP_PUSH_H_H

#include <iostream>
#include <map>
#include <mutex>
#include <list>
#include "player.h"
#include "net/net.h"
#include "rtsp-session.h"
#include "rtsp-server.h"
#include "udp-server.h"

class Pusher{
    
	Session *session;

	std::map<std::string, Player*>  players;        //SessionID <-> Player
	std::mutex playersLock;
	bool        gopCacheEnable;
	std::list<RTPPack>  gopCache;
	std::mutex         gopCacheLoc;
	UDPServer *udpserver;

	TcpStream cond;
	std::list<RTPPack> queue;

public:
    Pusher* NewPusher(Session *session);
    Pusher* QueueRTP(RTPPack *pack ) ;

    void Start() ;

    Pusher* BroadcastRTP(RTPPack *pack );

    std::map<std::string,Player*> GetPlayers();

    Pusher* AddPlayer(Player* player) ;

    Pusher* RemovePlayer(Player* player);
};

#endif
