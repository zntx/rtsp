// package rtsp

// import (
// 	"fmt"
// 	"log"
// 	"net"
// 	"sync"

// 	"github.com/penggy/EasyGoLib/utils"
// )
#include <iostream>
#include <map>
#include <mutex>
#include <thread>
#include <stdint.h>
#include "net/net.h"
#include "rtsp-server.h"
// var Instance *Server = &Server{
// 	Stoped:  true,
// 	TCPPort: utils.Conf().Section("rtsp").Key("port").MustInt(554),
// 	pushers: make(map[string]*Pusher),
// }

// func GetServer() *Server {
// 	return Instance
// }

Result<Server, std::string> Server::NewServer(uint16_t port)
{
    Result<TcpListener,int> tcp_listener = TcpListener::bin("0.0.0.0:554");
    if( tcp_listener.isErr())
        return Err(-1);

    Server server(listener, port);
    return Ok(server);
}


Server::Server(TcpListener listener, uint16_t port)
{
    tcp_listener = listener;
	TCPPort = port;     
	Stoped = true;      
	pushers = std::map<string, Pusher*>;      // Path <-> Pusher
	pushersLock = std::mutex(); 
}

bool Server::Start()
{
	this->Stoped = false;
	this->tcp_listener = listener;
	printf("rtsp server start on %d\n", this->TCPPort);
	//networkBuffer := utils.Conf().Section("rtsp").Key("network_buffer").MustInt(1048576)
	for( !this->Stoped ){
		//conn, err := server.tcp_listener.AcceptTCP()
        uint32_t msecond = 3;
        Result<TcpStream,int> stream_result = this->tcp_listener.AccepT_timeout(msecond);

		if (stream_result.isErr() ){
			//log.Println(err)
			continue;
		}
		// if err := conn.SetReadBuffer(networkBuffer); err != nil {
		// 	log.Printf("rtsp server conn set read buffer error, %v", err)
		// }
		// if err := conn.SetWriteBuffer(networkBuffer); err != nil {
		// 	log.Printf("rtsp server conn set write buffer error, %v", err)
		// }
		//session := NewSession(server, conn)
		//go session.Start()
        TcpStream stream = stream_result.unwrap();
        Session* session = new(this, stream);

        std::thread run(Session::start(), session);
        run.detach();
	}
	return true;
}

void Server::Stop() 
{
	printf("rtsp server stop on", this->TCPPort);
	this->Stoped = true
	if (this->tcp_listener != nullptr) {
		this->tcp_listener.Close();
		this->tcp_listener = nullptr;
	}
	//server.pushersLock.Lock()
	//server.pushers = make(map[string]*Pusher)
	//server.pushersLock.Unlock()
}

void Server::AddPusher(Pusher *pusher ) 
{
    this->pushersLock.lock();
	// if _, ok := server.pushers[pusher.Path]; !ok {
	// 	server.pushers[pusher.Path] = pusher
	// 	go pusher.Start()
	// 	log.Printf("%v start, now pusher size[%d]", pusher, len(server.pushers))
	// }
    std::map<std::string, Pusher*>::iterator it = this->pushers.find(pusher->path);
    if( it == this->pushers.end()) {
        this->pushers.insert(std::map<string, Pusher*>::value_type(pusher->path, pusher));

        std::thread run(Pusher::start(), pusher);
        run.detach();
    }
	this->pushersLock.unlock();
}

void Server::RemovePusher(Pusher *pusher) 
{
	this->pushersLock.lock();
	// if _pusher, ok := server.pushers[pusher.Path]; ok && pusher.ID == _pusher.ID {
	// 	delete(server.pushers, pusher.Path)
	// 	log.Printf("%v end, now pusher size[%d]\n", pusher, len(server.pushers))
	// }
    std::map<std::string, Pusher*>::iterator it = this->pushers.find(pusher->path);
    if( it != this->pushers.end() && it->second.ID == pusher->ID)
        this->pushers.erase(it);

	this->pushersLock.unlock();
}

Pusher* Server::GetPusher(std::string path)
{
	this->pushersLock.lock();
	//pusher = server.pushers[path]
    std::map<std::string, Pusher*>::iterator it = this->pushers.find(path);
	this->pushersLock.lock();
    if( it != this->pushers.end())
        return it->second;
	return nullptr;
}

std::map<std::string, Pusher*> Server::GetPushers()
{
	std::map<std::string, Pusher*> pushers;
	this->pushersLock.lock();
    for(auto it = this->pushers.begin(); it != this->pushers.end(); it++) {
        //cout<<it->first<<" "<<itr->second<<endl;//输出key 和value值
        pushers[it->first] = it->second;
    }
	this->pushersLock.unlock();
	return pushers;
}

std::size_t Server::GetPusherSize()
{
	this->pushersLock.lock();
	std_size_t size = this->pushers.size();
	this->pushersLock.unlock();
	return size;
}
