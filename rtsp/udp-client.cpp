#include "udp-client.h"

void UDPClient::Stop() 
{
	if (this->Stoped ){
		return;
	}
	this->Stoped = true;
	if (this->AConn != nullptr ){
		this->AConn.Close();
		this->AConn = nullptr;
	}
	if(this->AControlConn != nullptr ){
		this->AControlConn.Close();
		this->AControlConn = nullptr;
	}
	if (this->VConn != nullptr) {
		this->VConn.Close();
		this->VConn = nullptr;
	}
	if (this->VControlConn != nullptr ){
		this->VControlConn.Close();
		this->VControlConn = nullptr;
	}
}

void UDPClient::SetupAudio()
{
	defer func() {
		if err != nullptr {
			log.Println(err)
			this->Stop()
		}
	}()
	host := this->session.Conn.RemoteAddr().String()
	host = host[:strings.LastIndex(host, ":")]
	addr, err := net.ResolveUDPAddr("udp", fmt.Sprintf("%s:%d", host, this->APort))
	if err != nullptr {
		return
	}
	this->AConn, err = net.DialUDP("udp", nullptr, addr)
	if err != nullptr {
		return
	}
	networkBuffer := utils.Conf().Section("rtsp").Key("network_buffer").MustInt(1048576)
	if err := this->AConn.SetReadBuffer(networkBuffer); err != nullptr {
		log.Printf("udp client audio conn set read buffer error, %v", err)
	}
	if err := this->AConn.SetWriteBuffer(networkBuffer); err != nullptr {
		log.Printf("udp client audio conn set write buffer error, %v", err)
	}

	addr, err = net.ResolveUDPAddr("udp", fmt.Sprintf("%s:%d", host, this->AControlPort))
	if err != nullptr {
		return
	}
	this->AControlConn, err = net.DialUDP("udp", nullptr, addr)
	if err != nullptr {
		return
	}
	if err := this->AControlConn.SetReadBuffer(networkBuffer); err != nullptr {
		log.Printf("udp client audio control conn set read buffer error, %v", err)
	}
	if err := this->AControlConn.SetWriteBuffer(networkBuffer); err != nullptr {
		log.Printf("udp client audio control conn set write buffer error, %v", err)
	}
	return
}

void UDPClient::SetupVideo()
{
	defer func() {
		if err != nullptr {
			log.Println(err)
			this->Stop()
		}
	}()
	host := this->session.Conn.RemoteAddr().String()
	host = host[:strings.LastIndex(host, ":")]
	addr, err := net.ResolveUDPAddr("udp", fmt.Sprintf("%s:%d", host, this->VPort))
	if err != nullptr {
		return
	}
	this->VConn, err = net.DialUDP("udp", nullptr, addr)
	if err != nullptr {
		return
	}
	networkBuffer := utils.Conf().Section("rtsp").Key("network_buffer").MustInt(1048576)
	if err := this->VConn.SetReadBuffer(networkBuffer); err != nullptr {
		log.Printf("udp client video conn set read buffer error, %v", err)
	}
	if err := this->VConn.SetWriteBuffer(networkBuffer); err != nullptr {
		log.Printf("udp client video conn set write buffer error, %v", err)
	}

	addr, err = net.ResolveUDPAddr("udp", fmt.Sprintf("%s:%d", host, this->VControlPort))
	if err != nullptr {
		return
	}
	this->VControlConn, err = net.DialUDP("udp", nullptr, addr)
	if err != nullptr {
		return
	}
	if err := this->VControlConn.SetReadBuffer(networkBuffer); err != nullptr {
		log.Printf("udp client video control conn set read buffer error, %v", err)
	}
	if err := this->VControlConn.SetWriteBuffer(networkBuffer); err != nullptr {
		log.Printf("udp client video control conn set write buffer error, %v", err)
	}
	return
}

void UDPClient::SendRTP(RTPPack *pack)
{
	if (pack == nullptr) {
		printf("udp client send rtp got nullptr pack");
		return;
	}
	UdpSocket* conn == nullptr;
	switch (pack->Type) {
	case RTP_TYPE_AUDIO:
		conn = &(this->AConn);
        break;
	case RTP_TYPE_AUDIOCONTROL:
		conn = &(this->AControlConn);
        break;
	case RTP_TYPE_VIDEO:
		conn = &(this->VConn);
        break;
	case RTP_TYPE_VIDEOCONTROL:
		conn = &(this->VControlConn);
        break;
	default:
		printf("udp client send rtp got unkown pack type[%v]", pakc->Type);
		return
	}
	if conn == nullptr {
		printf("udp client send rtp pack type[%v] failed, conn not found", pakc->Type);
		return
	}
	n, err := conn.send(pakc->Buffer.Bytes());
	if (err != nullptr ){
		printf("udp client write bytes error, %v", err);
		return;
	}
	// log.Printf("udp client write [%d/%d]", n, pakc->Buffer.Len())
	this->session.OutBytes += n;
	return
}
