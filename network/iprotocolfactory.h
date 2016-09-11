/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   iprotocolfactory.h
 * Author: Alexey
 *
 * Created on 10 сентября 2016 г., 23:17
 */

#ifndef IPROTOCOLFACTORY_H
#define IPROTOCOLFACTORY_H

#include <memory>

using namespace std;

class IProtocol;
class ITransport;

class IProtocolFactory
{
public:
    virtual ~IProtocolFactory() {}
    
    virtual shared_ptr<IProtocol> buildProtocol(shared_ptr<ITransport> transport) = 0;
};

#endif /* IPROTOCOLFACTORY_H */

