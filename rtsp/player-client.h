#ifndef RTSP_PLAY_CLIENT_H
#define RTSP_PLAY_CLIENT_H

#include <iostream>
#include <list>
#include <map>
#include "rtsp-server.h"
#include "pusher.h"
#include "net/net.h"

class PlayerClient {
	bool Stoped ;
	std::string Path
	TcpStream   Conn;
	bool AuthHeaders ;
	std::string  session;
	int          Seq;
	//connRW      *bufio.ReadWriter
	uint64_t     InBytes;  

public:
    PlayerClient(std::string path);
    observable.Observable Start() ;

    (resp *Response, err error) Request(std::string method, std::map<std::string,std::string> headers );

};

/*
 request(method, headers, _url) {
        _url = _url || `${this.origin}${this.pathname}${this.search}`;
        headers = headers || {};
        headers["User-Agent"] = "EasyDarwin";
        if(this.authHeaders) {
            headers["Authorization"] = this.digest(method, _url);
        }
        if(this.session) {
            headers["Session"] = this.session;
        }
        var cseq = ++this.cseq;
        var req = `${method} ${_url} RTSP/1.0\r\n`;
        req += `CSeq: ${cseq}\r\n`;
        req += Object.keys(headers).map(header => {
            return `${header}: ${headers[header]}\r\n`
        }).join("");
        console.log(`[RTSP Client][${utils.formatDateTime()}] >>>>>> ${req}`);
        this.socket.write(`${req}\r\n`);

        return new Promise((resolve, reject) => {
            var timer;
            var reqHeaders = headers;
            var listener = (statusLine, headers, body) => {
                if(headers["CSeq"] != cseq) {
                    console.log(`Bad RTSP CSeq [${headers["CSeq"]}], want[${cseq}]!`);
                    return;
                }
                timer && clearTimeout(timer);
                this.removeListener("response", listener);
                var code = statusLine.split(/\s/)[1];
                if(code == "200") {
                    resolve({
                        headers: headers,
                        body: body
                    })
                    return;
                }
                if(code == "401" && !this.authHeaders) {
                    var type = headers[WWW_AUTH].split(" ")[0];
                    this.authHeaders = {
                        type: type
                    };
                    var reg = new RegExp('([a-z]+)=\"([^,\s]+)\"',"g");
                    var matchs = reg.exec(headers[WWW_AUTH]);
                    while(matchs) {
                        this.authHeaders[matchs[1]] = matchs[2];
                        matchs = reg.exec(headers[WWW_AUTH]);
                    }
                    resolve(this.request(method, reqHeaders, _url));
                    return;
                }
                reject(new Error(`Bad RTSP status code ${code}!`));
                return;
            }
            var timeout = utils.db.get("rtspClientTimeout").cloneDeep().value() || 5;
            if(!isNaN(timeout)){
                timer = setTimeout(() => {
                    this.removeListener("response", listener);
                    reject(new Error(`${method} timeout`));
                }, timeout * 1000);
            }
            this.on("response", listener);
        })
    }
*/
#endif