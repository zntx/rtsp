#ifndef SADP_INCLUDE_H
#define SADP_INCLUDE_H

#include <iostream>
#include <map>
#include <mutex>
#include <thread>
#include <vector>

std::vector<std::string> string_split(std::string str, const std::string cut);

std::vector<std::string> string_split(std::string str, const std::string cut, std::size_t count);

void string_trim(std::string &str);

class SDPInfo {
	std::string AVType;
	std::string Codec;
	int TimeScale;
	std::string Control;
	int Rtpmap;
	//byte* Config;
	//byte** SpropParameterSets;
	int PayloadType;
	int SizeLength;
	int IndexLength;


    std::map<std::string, SDPInfo*> ParseSDP(std::string sdpRaw) ;

	SDPInfo( std::string avtype);
};

#endif