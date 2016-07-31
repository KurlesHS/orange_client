/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   transport.h
 * Author: Alexey
 *
 * Created on 31 июля 2016 г., 22:54
 */

#ifndef TRANSPORT_H
#define TRANSPORT_H

#include <SimpleSignal/simplesignal.h>

#include <string>

class ITransport {
public:

    virtual ~ITransport() {
    }

    virtual void close() = 0;

    virtual int read(char *buff, int len) = 0; 
    virtual int write(char *buff, int len) = 0;
    virtual int bytesAvailable() const = 0;
    
    virtual std::string peerAddress() const = 0;

private:
    declare_signal(disconnected, void());
    declare_signal(readyRead, void());

};

#endif /* TRANSPORT_H */

