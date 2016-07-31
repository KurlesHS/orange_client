/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   iincommingcommand.cpp
 * Author: Alexey
 * 
 * Created on 31 июля 2016 г., 23:06
 */

#include "iincommingcommand.h"

#include "protocolimpl.h"

IIncommingCommand::~IIncommingCommand()
{
}

IIncommingCommand::IIncommingCommand(ProtocolImpl* protocolImpl) :
    mProtoImpl(protocolImpl)
{

}

void IIncommingCommand::done(Result result)
{
    mResult = result;
    mProtoImpl->sendResponse(this);
}

void IIncommingCommand::setCommandPayload(const std::vector<char>& payload)
{
    mCommandPayload = payload;
}

std::string IIncommingCommand::commandId() const
{
    return mCommandId;
}

std::vector<char> IIncommingCommand::commandPayload() const
{
    return mCommandPayload;
}

IIncommingCommand::Result IIncommingCommand::result() const
{
    return mResult;
}

uint16_t IIncommingCommand::resultCommandCode() const
{
    return (uint16_t)mResult;
}



