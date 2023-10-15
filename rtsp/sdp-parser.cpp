#include "sdp-parser.h"

std::vector<std::string> string_split(std::string str, const std::string cut)
{
	std::vector<std::string> vec;
	std::string::size_type end_index = str.find(cut);
	std::string::size_type begin_index = 0;

	while (std::string::npos != end_index)
	{
		vec.push_back(str.substr(begin_index, end_index - begin_index));
		begin_index = end_index + cut.size();
	}

	if (begin_index != str.length())
	{
		vec.push_back(str.substr(begin_index));
	}

	return vec;
}

std::vector<std::string> string_split(std::string str, const std::string cut, std::size_t count)
{
	std::vector<std::string> vec;
	std::string::size_type end_index = str.find(cut);
	std::string::size_type begin_index = 0;

	std::size_t i = 0;
	for (i = 0; std::string::npos != end_index && i < count; i++)
	{
		vec.push_back(str.substr(begin_index, end_index - begin_index));
		begin_index = end_index + cut.size();
	}

	if (begin_index != str.length() && i < count)
	{
		vec.push_back(str.substr(begin_index));
	}

	return vec;
}

void string_trim(std::string &str)
{
	str.erase(str.begin(), std::find_if(str.begin(), str.end(), [](int ch)
										{ return !std::isspace(ch); }));

	str.erase(std::find_if(str.rbegin(), str.rend(), [](int ch)
						   { return !std::isspace(ch); })
				  .base(),
			  str.end());
}

std::map<std::string, SDPInfo *> SDPInfo::ParseSDP(std::string sdpRaw)
{
	std::map<std::string, SDPInfo *> sdpMap; // sdpMap := make(map[string]*SDPInfo)
	SDPInfo *info = nullptr;				 // var info *SDPInfo
	std::vector<std::string> vec_line = string_split(sdpRaw, "\n");
	// for _, line := range strings.Split(sdpRaw, "\n") {
	for (std::string line : vec_line) {
		string_trim(line);
		std::vector<std::string> typeval = string_split(line, "=", 2);

		if (typeval.size() == 2) {
			std::vector<std::string> fields = string_split(typeval[1], " ", 2);

			if (typeval[0] == "m") {
				if (fields.size() > 0) {
					if (fields[0] == "audio" || fields[0] == "video") {
						// sdpMap[fields[0]] = new SDPInfo(  fields[0]);
						// info = sdpMap[fields[0]]
						info = new SDPInfo(fields[0]);
						sdpMap[fields[0]] = info;

						std::vector<std::string> mfields = string_split(fields[1], " ");
						if (mfields.size() >= 3) {
							info->PayloadType = atoi(mfields[2].c_str());
						}
					}
				}
			}

			if (typeval[0] == "a") {

				if (info != nullptr) {
					for (std::string field : fields) {
						std::vector<std::string> keyval = string_split(field, ":", 2);
						if (keyval.size() >= 2) {
							std::string key = keyval[0];
							std::string val = keyval[1];

							if (key == "control")
								info->Control = val;
							if (key == "rtpmap")
								info->Rtpmap = atoi(val.c_str());
						}

						keyval = string_split(field, "/");
						if (keyval.size() >= 2) {
							std::string key = keyval[0];

							if (key == "MPEG4-GENERIC")
								info->Codec = "aac";
							if (key == "H264")
								info->Codec = "h264";

							// if i, err := strconv.Atoi(keyval[1]); err == nil {
							info->TimeScale = atoi(keyval[1].c_str());
							//}
						}

						keyval = string_split(field, ";");
						if (keyval.size() > 1) {
							for (std::string field : keyval) {
								keyval = string_split(field, "=", 2);
								if (keyval.size() == 2) {
									// key := strings.TrimSpace(keyval[0]);
									// val := keyval[1];
									std::string key = keyval[0];
									std::string val = keyval[1];
									string_trim(key);

									if (key == "config")
										info->Config, _ = hex.DecodeString(val);
									if (key == "sizelength")
										info->SizeLength, _ = strconv.Atoi(val);
									if (key == "indexlength")
										info->IndexLength, _ = strconv.Atoi(val);
									if (key == "sprop-parameter-sets")
									{
										std::string tmp_fields = string_split(val, ",");
										for (std::string field : fields)
										{
											// val, _ := base64.StdEncoding.DecodeString(field)
											// info.SpropParameterSets = append(info.SpropParameterSets, val)
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
	return sdpMap;
}

SDPInfo::SDPInfo(std::string avtype)
{
	AVType = avtype;
}