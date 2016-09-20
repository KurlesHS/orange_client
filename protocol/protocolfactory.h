/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ProtocolFactory.h
 * Author: Alexey
 *
 * Created on 12 сентября 2016 г., 0:28
 */

#ifndef PROTOCOLFACTORY_H
#define PROTOCOLFACTORY_H

#include "network/iprotocolfactory.h"
#include "protocolimpl.h"
#include <list>

using namespace std;

class ProtocolImpl;

class ProtocolFactory : public IProtocolFactory {
public:

    ProtocolFactory();
    virtual ~ProtocolFactory();
    shared_ptr<IProtocol> buildProtocol(shared_ptr<ITransport> transport) override;
    
    void onDisconnected(ProtocolImpl *protocol);

private:
    list<shared_ptr<ProtocolImpl>> mActiveProtocols;
};

#endif /* PROTOCOLFACTORY_H */

