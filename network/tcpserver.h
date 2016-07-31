/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   tcpserver.h
 * Author: Alexey
 *
 * Created on 28 июля 2016 г., 23:25
 */

#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <memory>
#include <SimpleSignal/simplesignal.h>

#include <ev++.h>

class TcpSocket;

class TcpServerPrivate;

class TcpServer {
public:
    TcpServer(const ev::loop_ref &loop);    
    virtual ~TcpServer();
        
    void stop();
    
    void setMaxClients(const int count);
    int maxClients() const;
    
    bool startListen(int port);
    bool isListening() const;
    bool hasPendingConnections() const;
    int pendingConnectionCount() const;
    
    std::shared_ptr<TcpSocket> nextPendingConnection();
    std::shared_ptr<TcpSocket> nextPendingConnection(const ev::loop_ref &loop);
    
private:
    TcpServerPrivate *d;
    friend class TcpServerPrivate;
    
    declare_signal(newConnection, void() );
};

#endif /* TCPSERVER_H */

