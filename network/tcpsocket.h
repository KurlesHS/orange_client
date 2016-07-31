/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   tcpsocket.h
 * Author: Alexey
 *
 * Created on 28 июля 2016 г., 23:25
 */

#ifndef TCPSOCKET_H
#define TCPSOCKET_H


#include "itransport.h"

#include <ev++.h>
class TcpSocketPrivate;

class TcpSocket : public ITransport {
public:
    TcpSocket(ev::loop_ref loop);
    TcpSocket(int socketId, ev::loop_ref loop);
    
    virtual ~TcpSocket();

    void close();

    int read(char *buff, int len) override;
    int write(char *buff, int len) override;
    int bytesAvailable() const override;    
    std::string peerAddress() const override;    

private:
    TcpSocketPrivate *d;
    friend class TcpSocketPrivate;
    //declare_signal(disconnected, void());
    //declare_signal(readyRead, void());
};

#endif /* TCPSOCKET_H */

