/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   IProtocol.cpp
 * Author: Alexey
 * 
 * Created on 11 сентября 2016 г., 13:44
 */

#include "iprotocol.h"

#include "network/itransport.h"

IProtocol::IProtocol(shared_ptr<ITransport> transport) :
    mTransport(transport)
{
    mDisconnectedConnectionId = transport->connect_disconnected([this]() {
        onDisconnectedHelper();
    });
    
    mOnReadyReadConnectionId = transport->connect_readyRead([this]() {
       onReadyRead(); 
    });
    
}

IProtocol::~IProtocol()
{
    transport()->disconnect_disconnected(mDisconnectedConnectionId);
    transport()->disconnect_readyRead(mOnReadyReadConnectionId);
}

ITransport* IProtocol::transport() const
{
    return mTransport.get();
}

void IProtocol::onDisconnectedHelper()
{
    onDisconnected();
}

void IProtocol::onReadyRead()
{
    auto bytesToRead = mTransport->bytesAvailable();
    if (bytesToRead > 0) {
        vector<char> data;
        data.resize(bytesToRead);
        mTransport->read(data.data(), bytesToRead);
        dataReceived(data);
    }
}




