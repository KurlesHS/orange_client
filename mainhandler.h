/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   mainhandler.h
 * Author: Alexey
 *
 * Created on 27 сентября 2016 г., 22:32
 */

#ifndef MAINHANDLER_H
#define MAINHANDLER_H

#include "protocol/protocolfactory.h"
#include "tcpserverendpoint.h"
#include "auth/authmanager.h"

class MainHandler {
public:
    MainHandler();
    virtual ~MainHandler();
    
    void start();
    
private:
    AuthManager mAuthManager;
    ProtocolFactory mFactory;
    TcpServerEndpoint mServer;    
};

#endif /* MAINHANDLER_H */

