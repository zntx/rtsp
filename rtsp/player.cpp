#include "player.h"
#include "rtsp-server.h"
#include "pusher.h"
#include "net/net.h"


Player* Player::NewPlayer(Session* session , Pusher *pusher) 
{
	Player* player = new Player(session, pusher );

	session->StopHandles = append(session->StopHandles, []() {
		pusher->RemovePlayer(player);
		player->cond.Broadcast();
	});
	return
}

Player::Player(Session* session , Pusher* pusher)
{
	this->session = session;
	this->pusher = pusher;
	//cond:    sync.NewCond(&sync.Mutex{}),
	//queue:   make([]*RTPPack, 0),
}

Player* Player::QueueRTP(pack *RTPPack) 
{
	if pack == nil {
		log.Printf("player queue enter nil pack, drop it")
		return player
	}
	player.cond.L.Lock()
	player.queue = append(player.queue, pack)
	player.cond.Signal()
	player.cond.L.Unlock()
	return player
}

void Player::Start() 
{
	for !player.Stoped {
		var pack *RTPPack
		player.cond.L.Lock()
		if len(player.queue) == 0 {
			player.cond.Wait()
		}
		if len(player.queue) > 0 {
			pack = player.queue[0]
			player.queue = player.queue[1:]
		}
		player.cond.L.Unlock()
		if pack == nil {
			if !player.Stoped {
				log.Printf("player not stoped, but queue take out nil pack")
			}
			continue
		}
		if err := player.SendRTP(pack); err != nil {
			log.Println(err)
		}
	}
}
