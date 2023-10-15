
#include "udp-server.h"




void UDPServer::Stop() 
{
	if(this->Stoped) {
		return;
	}
	this->Stoped = true;
	if( this->AConn != nullptr ){
		this->AConn.Close();
		this->AConn = nullptr;
	}
	if (this->AControlConn != nullptr ){
		this->AControlConn.Close();
		this->AControlConn = nullptr;
	}
	if (this->VConn != nullptr ){
		this->VConn.Close();
		this->VConn = nullptr;
	}
	if( this->VControlConn != nullptr ){
		this->VControlConn.Close();
		this->VControlConn = nullptr;
	}
}

void UDPServer::SetupAudio()
{
	addr, err := net.ResolveUDPAddr("udp", ":0")
	if err != nullptr {
		return
	}
	this->AConn, err = net.ListenUDP("udp", addr)
	if err != nullptr {
		return
	}
	networkBuffer := utils.Conf().Section("rtsp").Key("network_buffer").MustInt(1048576)
	if err := this->AConn.SetReadBuffer(networkBuffer); err != nullptr {
		printf("udp server audio conn set read buffer error, %v", err)
	}
	if err := this->AConn.SetWriteBuffer(networkBuffer); err != nullptr {
		printf("udp server audio conn set write buffer error, %v", err)
	}
	la := this->AConn.LocalAddr().String()
	strPort := la[strings.LastIndex(la, ":")+1:]
	this->APort, err = strconv.Atoi(strPort)
	if err != nullptr {
		return
	}
	go func() {
		bufUDP := make([]byte, UDP_BUF_SIZE)
		printf("udp server start listen audio port[%d]", this->APort)
		defer printf("udp server stop listen audio port[%d]", this->APort)
		while( !this->Stoped ){
			if n, _, err := this->AConn.ReadFromUDP(bufUDP); err == nullptr {
				rtpBytes := make([]byte, n)
				this->session.InBytes += n
				copy(rtpBytes, bufUDP)
				pack := &RTPPack{
					Type:   RTP_TYPE_AUDIO,
					Buffer: bytes.NewBuffer(rtpBytes),
				}
				for _, h := range this->session.RTPHandles {
					h(pack)
				}
			} else {
				log.Println("udp server read audio pack error", err)
				continue
			}
		}
	}()
	addr, err = net.ResolveUDPAddr("udp", ":0")
	if err != nullptr {
		return
	}
	this->AControlConn, err = net.ListenUDP("udp", addr)
	if err != nullptr {
		return
	}
	if err := this->AControlConn.SetReadBuffer(networkBuffer); err != nullptr {
		printf("udp server audio control conn set read buffer error, %v", err)
	}
	if err := this->AControlConn.SetWriteBuffer(networkBuffer); err != nullptr {
		printf("udp server audio control conn set write buffer error, %v", err)
	}
	la = this->AControlConn.LocalAddr().String()
	strPort = la[strings.LastIndex(la, ":")+1:]
	this->AControlPort, err = strconv.Atoi(strPort)
	if err != nullptr {
		return
	}
	go func() {
		bufUDP := make([]byte, UDP_BUF_SIZE)
		printf("udp server start listen audio control port[%d]", this->AControlPort)
		defer printf("udp server stop listen audio control port[%d]", this->AControlPort)
		for !this->Stoped {
			if n, _, err := this->AControlConn.ReadFromUDP(bufUDP); err == nullptr {
				rtpBytes := make([]byte, n)
				this->session.InBytes += n
				copy(rtpBytes, bufUDP)
				pack := &RTPPack{
					Type:   RTP_TYPE_AUDIOCONTROL,
					Buffer: bytes.NewBuffer(rtpBytes),
				}
				for _, h := range this->session.RTPHandles {
					h(pack)
				}
			} else {
				log.Println("udp server read audio control pack error", err)
				continue
			}
		}
	}()
	return
}

void UDPServer::SetupVideo()
{
	addr, err := net.ResolveUDPAddr("udp", ":0")
	if err != nullptr {
		return
	}
	this->VConn, err = net.ListenUDP("udp", addr)
	if err != nullptr {
		return
	}
	networkBuffer := utils.Conf().Section("rtsp").Key("network_buffer").MustInt(1048576)
	if err := this->VConn.SetReadBuffer(networkBuffer); err != nullptr {
		printf("udp server video conn set read buffer error, %v", err)
	}
	if err := this->VConn.SetWriteBuffer(networkBuffer); err != nullptr {
		printf("udp server video conn set write buffer error, %v", err)
	}
	la := this->VConn.LocalAddr().String()
	strPort := la[strings.LastIndex(la, ":")+1:]
	this->VPort, err = strconv.Atoi(strPort)
	if err != nullptr {
		return
	}
	go func() {
		bufUDP := make([]byte, UDP_BUF_SIZE)
		printf("udp server start listen video port[%d]", this->VPort)
		defer printf("udp server stop listen video port[%d]", this->VPort)
		for !this->Stoped {
			if n, _, err := this->VConn.ReadFromUDP(bufUDP); err == nullptr {
				rtpBytes := make([]byte, n)
				this->session.InBytes += n
				copy(rtpBytes, bufUDP)
				pack := &RTPPack{
					Type:   RTP_TYPE_VIDEO,
					Buffer: bytes.NewBuffer(rtpBytes),
				}
				for _, h := range this->session.RTPHandles {
					h(pack)
				}
			} else {
				log.Println("udp server read video pack error", err)
				continue
			}
		}
	}()

	addr, err = net.ResolveUDPAddr("udp", ":0")
	if err != nullptr {
		return
	}
	this->VControlConn, err = net.ListenUDP("udp", addr)
	if err != nullptr {
		return
	}
	if err := this->VControlConn.SetReadBuffer(networkBuffer); err != nullptr {
		printf("udp server video control conn set read buffer error, %v", err)
	}
	if err := this->VControlConn.SetWriteBuffer(networkBuffer); err != nullptr {
		printf("udp server video control conn set write buffer error, %v", err)
	}
	la = this->VControlConn.LocalAddr().String()
	strPort = la[strings.LastIndex(la, ":")+1:]
	this->VControlPort, err = strconv.Atoi(strPort)
	if err != nullptr {
		return
	}
	go func() {
		bufUDP := make([]byte, UDP_BUF_SIZE)
		printf("udp server start listen video control port[%d]", this->VControlPort)
		defer printf("udp server stop listen video control port[%d]", this->VControlPort)
		for !this->Stoped {
			if n, _, err := this->VControlConn.ReadFromUDP(bufUDP); err == nullptr {
				rtpBytes := make([]byte, n)
				this->session.InBytes += n
				copy(rtpBytes, bufUDP)
				pack := &RTPPack{
					Type:   RTP_TYPE_VIDEOCONTROL,
					Buffer: bytes.NewBuffer(rtpBytes),
				}
				for _, h := range this->session.RTPHandles {
					h(pack)
				}
			} else {
				log.Println("udp server read video control pack error", err)
				continue
			}
		}
	}()
	return
}
