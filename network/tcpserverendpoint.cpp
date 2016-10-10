/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   TcpServerEndpoint.cpp
 * Author: Alexey
 * 
 * Created on 10 сентября 2016 г., 23:04
 */

#include "tcpserverendpoint.h"

#include "thread/threadregister.h"

#include "network/tcpserver.h"
#include "network/tcpsocket.h"
#include "network/iprotocolfactory.h"
#include "iprotocol.h"

TcpServerEndpoint::TcpServerEndpoint(IProtocolFactory *factory) :
    mServer(new TcpServer(ThreadRegister::loopForCurrentThread())),
    mFactory(factory)
{
    mServer->connect_newConnection([this]() {
        onNewConnection();
    });
}

TcpServerEndpoint::~TcpServerEndpoint()
{
    delete mServer;    
}

bool TcpServerEndpoint::listen(const uint16_t port)
{
    return mServer->startListen(port);
}

void TcpServerEndpoint::onNewConnection()
{
    while (mServer->hasPendingConnections()) {
        auto tcpSocket = mServer->nextPendingConnection();
        if (tcpSocket) {
            mFactory->buildProtocol(tcpSocket);
            
        }
    }
}

