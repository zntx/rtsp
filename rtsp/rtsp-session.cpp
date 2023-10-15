// package rtsp

// import (
// 	"bufio"
// 	"bytes"
// 	"encoding/binary"
// 	"fmt"
// 	"io"
// 	"log"
// 	"net"
// 	"net/url"
// 	"regexp"
// 	"strconv"
// 	"strings"
// 	"sync"
// 	"time"

// 	"github.com/penggy/EasyGoLib/utils"

// 	"github.com/teris-io/shortid"
// )


#include <iostream>
#include "rtsp-session.h"
#include "fmt/fmt.h"

#define UDP_BUF_SIZE 1048576

class Session {
	std::string ID;
	Server*     server;
	TcpStream   Conn;
	//connRW    *bufio.ReadWriter
	std::mutex  connWLock;
	SessionType Type;     
	TransType   TransType; 
	std::string Path;
	std::string URL;
	std::string SDPRaw;
	std::map<string, *SDPInfo> SDPMap

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

	Pusher*    Pusher;
	Player*    Player;
	UDPClient* UDPClient;
	//RTPHandles  []func(*RTPPack)
	//StopHandles []func()
}

std::string Session::String() 
{
	return fmt::sprint("session[%v][%v][%s][%s]", this->Type, this->TransType, this->Path, this->ID);
}

Session::Session(Server* server, TcpStream conn ) 
{
	//networkBuffer := utils.Conf().Section("rtsp").Key("network_buffer").MustInt(1048576)
	//session := &Session{
		ID =      shortid.MustGenerate();
		server =  server;
		Conn =   conn;
		//connRW:  bufio.NewReadWriter(bufio.NewReaderSize(conn, networkBuffer), bufio.NewWriterSize(conn, networkBuffer)),
		StartAt = time(NULL);
		//Timeout: utils.Conf().Section("rtsp").Key("timeout").MustInt(0),

		//RTPHandles:  make([]func(*RTPPack), 0),
		//StopHandles: make([]func(), 0),
	//}
	//return session
}

void Session::Stop() 
{
	if (this->Stoped) {
		return ;
	}
	this->Stoped = true;
	for _, h := range this->StopHandles {
		h()
	}
	//if this->Conn != nil {
		//this->connRW.Flush()
		this->Conn.Close()
		//this->Conn = nil
	//}
	//if this->UDPClient != nil {
		this->UDPClient.Stop()
		//this->UDPClient = nil
	//}
}

void Session::Start() 
{
	defer this->Stop()
	buf1 := make([]byte, 1)
	buf2 := make([]byte, 2)
	while ( !this->Stoped ) {
		if _, err := io.ReadFull(this->connRW, buf1); err != nil {
			log.Println(session, err)
			return
		}
		if (buf1[0] == 0x24 ){ //rtp data
			if _, err := io.ReadFull(this->connRW, buf1); err != nil {
				log.Println(err)
				return
			}
			if _, err := io.ReadFull(this->connRW, buf2); err != nil {
				log.Println(err)
				return
			}
			channel := int(buf1[0])
			rtpLen := int(binary.BigEndian.Uint16(buf2))
			rtpBytes := make([]byte, rtpLen)
			if _, err := io.ReadFull(this->connRW, rtpBytes); err != nil {
				log.Println(err)
				return
			}
			rtpBuf := bytes.NewBuffer(rtpBytes)
			var pack *RTPPack
			switch channel {
			case this->aRTPChannel:
				pack = &RTPPack{
					Type:   RTP_TYPE_AUDIO,
					Buffer: rtpBuf,
				}
			case this->aRTPControlChannel:
				pack = &RTPPack{
					Type:   RTP_TYPE_AUDIOCONTROL,
					Buffer: rtpBuf,
				}
			case this->vRTPChannel:
				pack = &RTPPack{
					Type:   RTP_TYPE_VIDEO,
					Buffer: rtpBuf,
				}
			case this->vRTPControlChannel:
				pack = &RTPPack{
					Type:   RTP_TYPE_VIDEOCONTROL,
					Buffer: rtpBuf,
				}
			default:
				log.Printf("unknow rtp pack type, %v", pack.Type)
				continue;
			}
			if pack == nil {
				log.Printf("session tcp got nil rtp pack")
				continue;
			}
			this->InBytes += rtpLen + 4
			for _, h := range this->RTPHandles {
				h(pack)
			}
		} else { // rtsp cmd
			reqBuf := bytes.NewBuffer(nil)
			reqBuf.Write(buf1)
			while( !this->Stoped) {
				if line, isPrefix, err := this->connRW.ReadLine(); err != nil {
					log.Println(err)
					return ;
				} else {
					reqBuf.Write(line)
					if !isPrefix {
						reqBuf.WriteString("\r\n")
					}
					if len(line) == 0 {
						req := NewRequest(reqBuf.String())
						if req == nil {
							break
						}
						this->InBytes += reqBuf.Len()
						contentLen := req.GetContentLength()
						this->InBytes += contentLen
						if contentLen > 0 {
							bodyBuf := make([]byte, contentLen)
							if n, err := io.ReadFull(this->connRW, bodyBuf); err != nil {
								log.Println(err)
								return
							} else if n != contentLen {
								log.Printf("read rtsp request body failed, expect size[%d], got size[%d]", contentLen, n)
								return
							}
							req.Body = string(bodyBuf)
						}
						this->handleRequest(req)
						break
					}
				}
			}
		}
	}
}

void Session::handleRequest(Request *req) 
{
	if this->Timeout > 0 {
		this->Conn.SetDeadline(time.Now().Add(time.Duration(this->Timeout) * time.Second))
	}

	log.Println("<<<", req)
	res := NewResponse(200, "OK", req.Header["CSeq"], this->ID, "")
	defer func() {
		if p := recover(); p != nil {
			res.StatusCode = 500
			res.Status = fmt.Sprintf("Inner Server Error, %v", p)
		}
		log.Println(">>>", res)
		outBytes := []byte(res.String())
		this->connWLock.Lock()
		this->connRW.Write(outBytes)
		this->connRW.Flush()
		this->connWLock.Unlock()
		this->OutBytes += len(outBytes)
		switch req.Method {
		case "PLAY", "RECORD":
			switch this->Type {
			case SESSEION_TYPE_PLAYER:
				this->Pusher.AddPlayer(this->Player)
			case SESSION_TYPE_PUSHER:
				this->server.AddPusher(this->Pusher)
			}
		case "TEARDOWN":
			this->Stop()
		}
	}()
	switch req.Method {
	case "OPTIONS":
		res.Header["Public"] = "DESCRIBE, SETUP, TEARDOWN, PLAY, PAUSE, OPTIONS, ANNOUNCE, RECORD"
	case "ANNOUNCE":
		this->Type = SESSION_TYPE_PUSHER
		this->URL = req.URL

		url, err := url.Parse(req.URL)
		if err != nil {
			res.StatusCode = 500
			res.Status = "Invalid URL"
			return
		}
		this->Path = url.Path

		this->SDPRaw = req.Body
		this->SDPMap = ParseSDP(req.Body)
		sdp, ok := this->SDPMap["audio"]
		if ok {
			this->AControl = sdp.Control
			this->ACodec = sdp.Codec
			log.Printf("audio codec[%s]\n", this->ACodec)
		}
		sdp, ok = this->SDPMap["video"]
		if ok {
			this->VControl = sdp.Control
			this->VCodec = sdp.Codec
			log.Printf("video codec[%s]\n", this->VCodec)
		}
		this->Pusher = NewPusher(session)
		if this->server.GetPusher(this->Path) == nil {
			this->server.AddPusher(this->Pusher)
		} else {
			res.StatusCode = 406
			res.Status = "Not Acceptable"
			return
		}
	case "DESCRIBE":
		this->Type = SESSEION_TYPE_PLAYER
		this->URL = req.URL

		url, err := url.Parse(req.URL)
		if err != nil {
			res.StatusCode = 500
			res.Status = "Invalid URL"
			return
		}
		this->Path = url.Path
		pusher := this->server.GetPusher(this->Path)
		if pusher == nil {
			res.StatusCode = 404
			res.Status = "NOT FOUND"
			return
		}
		this->Player = NewPlayer(session, pusher)
		this->Pusher = pusher
		this->AControl = pusher.AControl
		this->VControl = pusher.VControl
		this->ACodec = pusher.ACodec
		this->VCodec = pusher.VCodec
		res.SetBody(this->Pusher.SDPRaw)
	case "SETUP":
		ts := req.Header["Transport"]
		control := req.URL[strings.LastIndex(req.URL, "/")+1:]
		mtcp := regexp.MustCompile("interleaved=(\\d+)(-(\\d+))?")
		mudp := regexp.MustCompile("client_port=(\\d+)(-(\\d+))?")

		if tcpMatchs := mtcp.FindStringSubmatch(ts); tcpMatchs != nil {
			this->TransType = TRANS_TYPE_TCP
			if control == this->AControl {
				this->aRTPChannel, _ = strconv.Atoi(tcpMatchs[1])
				this->aRTPControlChannel, _ = strconv.Atoi(tcpMatchs[3])
			} else if control == this->VControl {
				this->vRTPChannel, _ = strconv.Atoi(tcpMatchs[1])
				this->vRTPControlChannel, _ = strconv.Atoi(tcpMatchs[3])
			}
		} else if udpMatchs := mudp.FindStringSubmatch(ts); udpMatchs != nil {
			this->TransType = TRANS_TYPE_UDP
			if this->UDPClient == nil {
				this->UDPClient = &UDPClient{
					Session: session,
				}
			}
			if this->Type == SESSION_TYPE_PUSHER && this->Pusher.UDPServer == nil {
				this->Pusher.UDPServer = &UDPServer{
					Session: session,
				}
			}
			if control == this->AControl {
				this->UDPClient.APort, _ = strconv.Atoi(udpMatchs[1])
				this->UDPClient.AControlPort, _ = strconv.Atoi(udpMatchs[3])
				if err := this->UDPClient.SetupAudio(); err != nil {
					res.StatusCode = 500
					res.Status = fmt.Sprintf("udp client setup audio error, %v", err)
					return
				}

				if this->Type == SESSION_TYPE_PUSHER {
					if err := this->Pusher.UDPServer.SetupAudio(); err != nil {
						res.StatusCode = 500
						res.Status = fmt.Sprintf("udp server setup audio error, %v", err)
						return
					}
					tss := strings.Split(ts, ";")
					idx := -1
					for i, val := range tss {
						if val == udpMatchs[0] {
							idx = i
						}
					}
					tail := append([]string{}, tss[idx+1:]...)
					tss = append(tss[:idx+1], fmt.Sprintf("server_port=%d-%d", this->Pusher.UDPServer.APort, this->Pusher.UDPServer.AControlPort))
					tss = append(tss, tail...)
					ts = strings.Join(tss, ";")
				}
			} else if control == this->VControl {
				this->UDPClient.VPort, _ = strconv.Atoi(udpMatchs[1])
				this->UDPClient.VControlPort, _ = strconv.Atoi(udpMatchs[3])
				if err := this->UDPClient.SetupVideo(); err != nil {
					res.StatusCode = 500
					res.Status = fmt.Sprintf("udp client setup video error, %v", err)
					return
				}

				if this->Type == SESSION_TYPE_PUSHER {
					if err := this->Pusher.UDPServer.SetupVideo(); err != nil {
						res.StatusCode = 500
						res.Status = fmt.Sprintf("udp server setup video error, %v", err)
						return
					}
					tss := strings.Split(ts, ";")
					idx := -1
					for i, val := range tss {
						if val == udpMatchs[0] {
							idx = i
						}
					}
					tail := append([]string{}, tss[idx+1:]...)
					tss = append(tss[:idx+1], fmt.Sprintf("server_port=%d-%d", this->Pusher.UDPServer.VPort, this->Pusher.UDPServer.VControlPort))
					tss = append(tss, tail...)
					ts = strings.Join(tss, ";")
				}
			}
		}
		res.Header["Transport"] = ts
	case "PLAY":
		res.Header["Range"] = req.Header["Range"]
	case "RECORD":
	}
}

Error Session::SendRTP(RTPPack *pack)
{
	if( pack == nullptr  ){
		return "player send rtp got nil pack";
	}
	if( this->TransType == TRANS_TYPE_UDP) {
		if( this->UDPClient == nullptr ){
			return "player use udp transport but udp client not found";
		}
		
        return  this->UDPClient.SendRTP(pack);
	}
	switch pack->Type {
	case RTP_TYPE_AUDIO:
		byte bufChannel[2];//bufChannel := make([]byte, 2)
		bufChannel[0] = 0x24;;
		bufChannel[1] = byte(this->aRTPChannel);
		this->connWLock.lock();
		//this->connRW.Write(bufChannel);
		bufLen bufChannel[2];//bufLen := make([]byte, 2)
		binary.BigEndian.PutUint16(bufLen, uint16(pack.Buffer.Len()));
		this->connRW.Write(bufLen);
		this->connRW.Write(pack.Buffer.Bytes());
		this->connRW.Flush();
		this->connWLock.unlock();
		this->OutBytes += pack.Buffer.Len() + 4;
        break;

	case RTP_TYPE_AUDIOCONTROL:
		byte bufChannel[2];//bufChannel := make([]byte, 2)
		bufChannel[0] = 0x24;
		bufChannel[1] = byte(this->aRTPControlChannel);
		this->connWLock.Lock();
		this->connRW.Write(bufChannel);
		bufLen bufChannel[2];//bufLen := make([]byte, 2)
		binary.BigEndian.PutUint16(bufLen, uint16(pack.Buffer.Len()));
		this->connRW.Write(bufLen);
		this->connRW.Write(pack.Buffer.Bytes());
		this->connRW.Flush();
		this->connWLock.Unlock();
		this->OutBytes += pack.Buffer.Len() + 4;
        break;

	case RTP_TYPE_VIDEO:
		byte bufChannel[2];//bufChannel := make([]byte, 2)
		bufChannel[0] = 0x24;
		bufChannel[1] = byte(this->vRTPChannel);
		this->connWLock.Lock();
		this->connRW.Write(bufChannel);
		bufLen bufChannel[2];//bufLen := make([]byte, 2)
		binary.BigEndian.PutUint16(bufLen, uint16(pack.Buffer.Len()));
		this->connRW.Write(bufLen);
		this->connRW.Write(pack.Buffer.Bytes());
		this->connRW.Flush();
		this->connWLock.Unlock();
		this->OutBytes += pack.Buffer.Len() + 4;
        break;

	case RTP_TYPE_VIDEOCONTROL:
		byte bufChannel[2];//bufChannel := make([]byte, 2)
		bufChannel[0] = 0x24;
		bufChannel[1] = byte(this->vRTPControlChannel);
		this->connWLock.Lock();
		this->connRW.Write(bufChannel);
		bufLen bufChannel[2];//bufLen := make([]byte, 2)
		binary.BigEndian.PutUint16(bufLen, uint16(pack.Buffer.Len()));
		this->connRW.Write(bufLen);
		this->connRW.Write(pack.Buffer.Bytes());
		this->connRW.Flush();
		this->connWLock.Unlock();
		this->OutBytes += pack.Buffer.Len() + 4;
        break;

	default:
		err = fmt.Errorf("session tcp send rtp got unkown pack type[%v]", pack.Type)
	}
	return
}
