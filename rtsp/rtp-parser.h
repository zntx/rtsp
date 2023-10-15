#include <iostream>
#include "buff/buff.h"

class RTPInfo {
	int  Version;
	bool Padding;
	bool Extension;
	int  CSRCCnt;
	bool Marker;
	int PayloadType;
	int SequenceNumber;
	int Timestamp;
	int SSRC;
	Slice<uint8_t> Payload;

    static RTPInfo* RTPInfo::ParseRTP(Slice<uint8_t> rtpBytes);
    RTPInfo::RTPInfo( int  version,
	bool padding,
	bool extension,
	int  csrccnt,
	bool marker,
	int payloadtype,
	int sequencenumber,
	int timestamp,
	int ssrc);
    bool RTPInfo::IsKeyframeStart() ;
    bool RTPInfo::IsKeyframeStartH265();
};
