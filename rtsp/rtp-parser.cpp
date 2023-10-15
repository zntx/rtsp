#include "rtp-parser.h"

RTPInfo* RTPInfo::ParseRTP(Slice<uint8_t> rtpBytes)
{
	if (rtpBytes.len < RTP_FIXED_HEADER_LENGTH) {
		return nullptr;
	}
	uint8_t firstByte = rtpBytes[0];
	uint8_t secondByte = rtpBytes[1];
	RTPInfo info(
		/*Version:  */ int(firstByte >> 6),
		/*Padding:   */(firstByte>>5)&1 == 1,
		/*Extension: */(firstByte>>4)&1 == 1,
		/*CSRCCnt:   */int(firstByte & 0x0f),

		/*Marker:        */ secondByte>>7 == 1,
		/*PayloadType:    */int(secondByte & 0x7f),
		/*SequenceNumber: */int(binary.BigEndian.Uint16(rtpBytes[2:])),
		/*Timestamp:      */int(binary.BigEndian.Uint32(rtpBytes[4:])),
		/*SSRC:          */ int(binary.BigEndian.Uint32(rtpBytes[8:]))
    );
	std::size_t offset = RTP_FIXED_HEADER_LENGTH;
	std::size_t end = rtpBytes.len;
	if (end-offset >= 4*info->CSRCCnt) {
		offset += 4 * info->CSRCCnt;
	}
	if(info->Extension && end-offset >= 4) {
		std::size_t extLen = 4 * int(binary.BigEndian.Uint16(rtpBytes[offset+2:]));
		offset += 4;
		if (end-offset >= extLen ){
			offset += extLen;
		}
	}
	if (info->Padding && end-offset > 0 ){
		int paddingLen := int(rtpBytes[end-1]);
		if (end-offset >= paddingLen) {
			end -= paddingLen;
		}
	}
	info->Payload = rtpBytes[offset:end];
	return info;
}

RTPInfo::RTPInfo( int  version,
	bool padding,
	bool extension,
	int  csrccnt,
	bool marker,
	int payloadtype,
	int sequencenumber,
	int timestamp,
	int ssrc)
{
	Version   = version
	Padding   = padding
	Extension = extension
	CSRCCnt   = csrccnt
	Marker    = marker
	PayloadType = payloadtype
	SequenceNumber = sequencenumber
	Timestamp     = timestamp
	SSRC  = ssrc
}



bool RTPInfo::IsKeyframeStart()  
{
	if (this->Payload.len >= 2 && this->Payload[0] == 0x7c && (this->Payload[1] == 0x87 || this->Payload[1] == 0x85) ){
		return true;
	}
	return false;
}

bool RTPInfo::IsKeyframeStartH265()  
{
	if (this->Payload.len >= 3) {
		uint8_t firstByte = this->Payload[0];
		uint8_t headerType = (firstByte >> 1) & 0x3f;
		if (headerType == 49 ){
			uint8_t frametByte = this->Payload[2];
			uint8_t frameType = frametByte & 0x3f;
			uint8_t rtpStart = (frametByte & 0x80) >> 7;
			if (rtpStart == 1 && (frameType == 19 || frameType == 20 || frameType == 21 || frameType == 32 || frameType == 33 || frameType == 34) ){
				return true;
			}
		}
	}
	return false;
}
