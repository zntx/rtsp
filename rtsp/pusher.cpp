
#include <iostream>
#include <map>
#include <mutex>
#include <list>
#include "net/net.h"
#include "rtsp-server.h"

Pusher* Pusher::NewPusher(Session *session)
{
	pusher = &Pusher{
		Session:        session,
		players:        make(map[string]*Player),
		gopCacheEnable: utils.Conf().Section("rtsp").Key("gop_cache_enable").MustBool(true),
		gopCache:       make([]*RTPPack, 0),

		cond:  sync.NewCond(&sync.Mutex{}),
		queue: make([]*RTPPack, 0),
	}
	session->RTPHandles = append(session.RTPHandles, func(pack *RTPPack) {
		this->QueueRTP(pack)
	})
	session->StopHandles = append(session.StopHandles, func() {
		this->server.RemovePusher(pusher)
		this->cond.Broadcast()
		if this->UDPServer != nil {
			this->UDPServer.Stop()
			this->UDPServer = nil
		}
	})
	return
}

Pusher* Pusher::QueueRTP(pack *RTPPack) 
{
	this->cond.L.lock()
	this->queue.push_back(pack);
	this->cond.Signal()
	this->cond.L.unlock()
	return this;
}

void Pusher::Start() 
{
	while( !this->Stoped ) {
		RTPPack *pack = nullptr;
		this->cond.L.lock();
		if len(this->queue) == 0 {
			this->cond.Wait();
		}
		if (len(this->queue) > 0 ){
			pack = this->queue[0]
			this->queue = this->queue[1:]
		}
		this->cond.L.unlock()
		if pack == nil {
			if !this->Stoped {
				printf("pusher not stoped, but queue take out nil pack")
			}
			continue
		}

		if this->gopCacheEnable {
			this->gopCacheLock.lock()
			if strings.EqualFold(this->VCodec, "h264") {
				if rtp := ParseRTP(pack.Buffer.Bytes()); rtp != nil && rtp.IsKeyframeStart() {
					this->gopCache = make([]*RTPPack, 0)
				}
				this->gopCache = append(this->gopCache, pack)
			} else if strings.EqualFold(this->VCodec, "h265") {
				if rtp := ParseRTP(pack.Buffer.Bytes()); rtp != nil && rtp.IsKeyframeStartH265() {
					this->gopCache = make([]*RTPPack, 0)
				}
				this->gopCache = append(this->gopCache, pack)
			}
			this->gopCacheLock.unlock()
		}

		this->BroadcastRTP(pack)
	}
}

Pusher* Pusher::BroadcastRTP(pack *RTPPack)
{
	for _, player := range this->GetPlayers() {
		player.QueueRTP(pack)
		this->OutBytes += pack.Buffer.Len()
	}
	return this;
}

std::map<std::string,*Player> Pusher::GetPlayers()
{
	players = make(map[string]*Player)
	this->playersLock.lock();
	for k, v := range this->players {
		players[k] = v
	}
	this->playersLock.unlock();
	return
}

Pusher* Pusher::AddPlayer(Player* player) 
{
	if (this->gopCacheEnable) {
		this->gopCacheLock.lock();
		for _, pack := range this->gopCache {
			player->QueueRTP(pack);
			this->OutBytes += pack.Buffer.Len();
		}
		this->gopCacheLock.unlock();
	}

	this->playersLock.lock();
	if _, ok := this->players[player.ID]; !ok {
		this->players[player.ID] = player
		go player.Start()
		printf("%v start, now player size[%d]", player, this->players.size());
	}
	this->playersLock.unlock();
	return this;
}

Pusher* Pusher::RemovePlayer(Player* player)  
{
	this->playersLock.lock()
	this->players.erase( player->ID);
	printf("%v end, now player size[%d]\n", player, this->players.size());
	this->playersLock.unlock()
	return this;
}
