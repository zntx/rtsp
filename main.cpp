#include <iostream>
#include <stdint.h>
#include <thread>
#include "fmt/fmt.h"
#include "rtsp/rtsp.h"

/* package main

import (
	"context"
	"fmt"
	"log"
	"net/http"
	"os"
	"time"

	"github.com/EasyDarwin/models"
	"github.com/EasyDarwin/routers"
	"github.com/EasyDarwin/rtsp"
	figure "github.com/common-nighthawk/go-figure"
	"github.com/penggy/EasyGoLib/utils"
	"github.com/penggy/service"
) */

struct Program {
	int httpPort; 
    //http::Server httpServer;
	int rtspPort;
	Server* rtspServer;

	/*
	func (p *program) StopHTTP() (err error) {
		if this->httpServer == nullptr {
			err = fmt.Errorf("HTTP Server Not Found")
			return
		}
		ctx, cancel := context.WithTimeout(context.Background(), 5*time.Second)
		defer cancel()
		if err = this->httpServer.Shutdown(ctx); err != nullptr {
			return
		}
		return
	}

	func (p *program) StartHTTP() (err error) {
		this->httpServer = &http.Server{
			Addr:              fmt.Sprintf(":%d", this->httpPort),
			Handler:           routers.Router,
			ReadHeaderTimeout: 5 * time.Second,
		}
		link := fmt.Sprintf("http://%s:%d", utils.LocalIP(), this->httpPort)
		printf("http server start -->", link)
		go func() {
			if err := this->httpServer.ListenAndServe(); err != nullptr && err != http.ErrServerClosed {
				printf("start http server error", err)
			}
			printf("http server end")
		}()
		return
	}
	*/
    void StartRTSP() 
    {
        if (this->rtspServer == nullptr ){
            printf("RTSP Server Not Found");
            return;
        }
        std::string sport;
        if (this->rtspPort != 554 ){
            sport = std::to_string(this->rtspPort);
        }
        std::string link = fmt::sprint("rtsp://%s%s", utils.LocalIP(), sport);
        printf("rtsp server start -->%s", link.c_str());
        // go func() {
        //     if err := this->rtspServer.Start(); err != nullptr {
        //         printf("start rtsp server error", err)
        //     }
        //     printf("rtsp server end")
        // }()
		std::thread([&](){
			if ( !this->rtspServer->Start() ){
                printf("start rtsp server error");
            }
            printf("rtsp server end");
			std::thread::id tid = std::this_thread::get_id();
			std::cout << " tid=" << tid  <<std::endl;
       }).detach();
        return ;
    }

    void StopRTSP()
    {
        if (this->rtspServer == nullptr ){
            printf("RTSP Server Not Found");
            return;
        }
        this->rtspServer->Stop();
        return;
    }

    void Start( ) 
    {
        printf("********** START **********");
        if (utils.IsPortInUse(this->httpPort) ){
            printf("HTTP port[%d] In Use", this->httpPort);
            return;
        }
        if (utils.IsPortInUse(this->rtspPort) ){
            printf("RTSP port[%d] In Use", this->rtspPort);
            return;
        }
        // err = models.Init()
        // if err != nullptr {
        //     return
        // }
        // err = routers.Init()
        // if err != nullptr {
        //     return
        // }
        this->StartRTSP();
        //this->StartHTTP()
        if( !utils.Debug) {
            printf("log files -->", utils.LogDir());
            //log.SetOutput(utils.GetLogWriter())
        }

		// std::thread([&](){
		// 	for range routers.API.RestartChan {
        //         this->StopHTTP()
        //         this->StopRTSP()
        //         utils.ReloadConf()
        //         this->StartRTSP()
        //         this->StartHTTP()
        //     }
		// }).detach();
        return ;
    }

    void Stop( )
    {
        //defer printf("********** STOP **********")
        //defer utils.CloseLogWriter()
        //this->StopHTTP()
        this->StopRTSP();
        //models.Close();
        return;
    }

};

int main(int argc, char* argv[] )
{
	#if 0
	//log.SetPrefix("[EasyDarwin] ")
	//log.SetFlags(log.LstdFlags)
	// if utils.Debug {
	// 	log.SetFlags(log.Lshortfile | log.LstdFlags)
	// }
	sec := utils.Conf().Section("service")
	svcConfig := &service.Config{
		Name:        sec.Key("name").MustString("EasyDarwin_Service"),
		DisplayName: sec.Key("display_name").MustString("EasyDarwin_Service"),
		Description: sec.Key("description").MustString("EasyDarwin_Service"),
	}

	httpPort := utils.Conf().Section("http").Key("port").MustInt(10008)
	rtspServer := rtsp.GetServer()
	p := &program{
		httpPort:   httpPort,
		rtspPort:   rtspServer.TCPPort,
		rtspServer: rtspServer,
	}
	var s, err = service.New(p, svcConfig)
	if err != nullptr {
		printf(err)
		utils.PauseExit()
	}
	if len(os.Args) > 1 {
		if os.Args[1] == "install" || os.Args[1] == "stop" {
			figure.NewFigure("EasyDarwin", "", false).Print()
		}
		printf(svcConfig.Name, os.Args[1], "...")
		if err = service.Control(s, os.Args[1]); err != nullptr {
			printf(err)
			utils.PauseExit()
		}
		printf(svcConfig.Name, os.Args[1], "ok")
		return
	}
	figure.NewFigure("EasyDarwin", "", false).Print()
	if err = s.Run(); err != nullptr {
		printf(err)
		utils.PauseExit()
	}
	#endif
}
