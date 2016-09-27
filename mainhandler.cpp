/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   mainhandler.cpp
 * Author: Alexey
 * 
 * Created on 27 сентября 2016 г., 22:32
 */

#include "mainhandler.h"
#include "tcpserverendpoint.h"

MainHandler::MainHandler() :
    mFactory(&mAuthManager),
    mServer(&mFactory)
{
}

MainHandler::~MainHandler()
{
}

void MainHandler::start()
{
    mServer.listen(55555);
}

