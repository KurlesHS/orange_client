/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   IProtocol.h
 * Author: Alexey
 *
 * Created on 11 сентября 2016 г., 13:44
 */

#ifndef IPROTOCOL_H
#define IPROTOCOL_H

#include <memory>
#include <vector>

#include "itransport.h"

using namespace std;

class ITransport;

class IProtocol {
public:
    IProtocol(shared_ptr<ITransport> transport);
    virtual ~IProtocol();

    ITransport *transport() const;

protected:
    virtual void dataReceived(const vector<char> &data) = 0;
    virtual void onConnected() = 0;
    virtual void onDisconnected() = 0;

private:
    void onReadyRead();
    void onDisconnectedHelper();



private:
    shared_ptr<ITransport> mTransport;
    size_t mDisconnectedConnectionId;
    size_t mOnReadyReadConnectionId;

};

#endif /* IPROTOCOL_H */

