/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   startscriptcommand.cpp
 * Author: Alexey
 * 
 * Created on 1 августа 2016 г., 0:28
 */

#include "startscriptcommand.h"

StartScriptCommand::StartScriptCommand(ProtocolImpl *protoImpl) :
    IIncommingCommand(protoImpl)
{
}

StartScriptCommand::~StartScriptCommand()
{
}

