/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   TcpServerEndpoint.h
 * Author: Alexey
 *
 * Created on 10 сентября 2016 г., 23:04
 */

#ifndef TCPSERVERENDPOINT_H
#define TCPSERVERENDPOINT_H

#include <stdint.h>

class IProtocolFactory;
class TcpServer;

class TcpServerEndpoint {
public:
    TcpServerEndpoint(IProtocolFactory *factory);    
    virtual ~TcpServerEndpoint();
    
    bool listen(const uint16_t port);
    
private:
    void onNewConnection();
    
private:
    TcpServer *mServer;
    IProtocolFactory *mFactory;

};

#endif /* TCPSERVERENDPOINT_H */

