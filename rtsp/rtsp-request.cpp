
#include "fmt/fmt.h"
#include "rtsp-request.h"

Request* Request::NewRequest(std::string content) 
{
	lines := strings.Split(strings.TrimSpace(content), "\r\n")
	if len(lines) == 0 {
		return nullptr;
	}
	items := regexp.MustCompile("\\s+").Split(strings.TrimSpace(lines[0]), -1)
	if len(items) < 3 {
		return nullptr;
	}
	if !strings.HasPrefix(items[2], "RTSP") {
		log.Printf("invalid rtsp request, line[0] %s", lines[0])
		return nullptr;
	}
	header := make(map[string]string)
	for( auto i = 1; i < len(lines); i++) {
		line := strings.TrimSpace(lines[i])
		headerItems := regexp.MustCompile(":\\s+").Split(line, 2)
		if len(headerItems) < 2 {
			continue;
		}
		header[headerItems[0]] = headerItems[1]
	}
	return new Request(
		/*Method:  */,items[0],
		/*URL:     */,items[1],
		/*Version: */,items[2],
		/*Header:  */,header,
		/*Content: */,content
		/*Body:    "", */
	);
}

Request::Request(std::string method, std::string url, std::string version, std::string heard, std::string count )
{

}

std::string Request::String()
{
	std::string str = fmt::sprint("%s %s %s\r\n", this->Method, this->URL, this->Version);
    for(auto it = this->Header.begin(); it != this->Header.end(); it++) {
        //cout<<it->first<<" "<<itr->second<<endl;//输出key 和value值
        str += fmt::sprint("%s: %s\r\n", it->first, it->second);
    }
	str += "\r\n";
	str += this->Body;
	return str;
}

int Request::GetContentLength()
{
	// v, err := strconv.ParseInt(this->Header["Content-Length"], 10, 64)
	// if err != nil {
	// 	return 0
	// } else {
	// 	return int(v)
	// }

    std::map<std::string, std::string>::iterator it = this->Header.find("Content-Length");
    if( it != this->Header.end()) {
        return atoi(it->second);
    }

    return 0;
}
