/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ioutgoingcommand.cpp
 * Author: Alexey
 * 
 * Created on 19 сентября 2016 г., 19:31
 */

#include "ioutgoingcommand.h"

IOutgoingCommand::IOutgoingCommand()
{
    mSendingTime = chrono::system_clock::now();
}

IOutgoingCommand::~IOutgoingCommand()
{
}

bool IOutgoingCommand::isNeedCheckAuth() const
{
    return true;
}

void IOutgoingCommand::onResult()
{

}

uint64_t IOutgoingCommand::sequenceNum() const
{
    return mSequenceNum;
}

void IOutgoingCommand::setSequenceNum(const uint64_t seq)
{
    mSequenceNum = seq;
}

void IOutgoingCommand::setSendingTime(chrono::system_clock::time_point time)
{
    this->mSendingTime = time;
}


chrono::system_clock::time_point IOutgoingCommand::sendingTime() const
{
    return mSendingTime;
}
