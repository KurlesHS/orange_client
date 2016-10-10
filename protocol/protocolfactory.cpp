/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ProtocolFactory.cpp
 * Author: Alexey
 * 
 * Created on 12 сентября 2016 г., 0:28
 */

#include <iostream>

#include "protocolfactory.h"
#include "protocolimpl.h"
#include "network/itransport.h"

ProtocolFactory::ProtocolFactory(AuthManager *authManager) :
    mAuthManager(authManager)
{
    
}

ProtocolFactory::~ProtocolFactory()
{
}

shared_ptr<IProtocol> ProtocolFactory::buildProtocol(shared_ptr<ITransport> transport)
{
    shared_ptr<ProtocolImpl> retVal = make_shared<ProtocolImpl>(transport, mAuthManager);

    
    retVal->connect_disconnected([this](ProtocolImpl * protocol) {
        onDisconnected(protocol);
    });        
    
    mActiveProtocols.push_back(retVal);
    retVal->onConnected();
    return retVal;
}

void ProtocolFactory::onDisconnected(ProtocolImpl* protocol)
{
    //protocol->disconnect_disconnected();
    mActiveProtocols.remove_if([protocol](shared_ptr<ProtocolImpl> compare) {
        return protocol == compare.get();
    });
    cout << mActiveProtocols.size() << endl;
}




