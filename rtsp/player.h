#ifndef RTSP_PLAY_H
#define RTSP_PLAY_H

#include <iostream>
#include <list>
#include "rtsp-server.h"
#include "rtsp-session.h"
#include "pusher.h"
#include "net/net.h"

class Player
{
    Session *session;
    Pusher *pusher;
    TcpStream cond;
    std::list<RTPPack> queue;
public:
    Player NewPlayer(Session *session, Pusher *pusher);

    Player(Session *session, Pusher *pusher);

    Player* QueueRTP(RTPPack *pack);

    void Start();
};


#endif