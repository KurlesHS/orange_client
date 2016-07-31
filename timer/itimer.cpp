 /*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   itimer.cpp
 * Author: Alexey
 * 
 * Created on 28 июля 2016 г., 21:13
 */

#include "itimer.h"

ITimer::ITimer(const uint32_t &interval) : mInterval(interval), mIsSingleShot(false)
{
}

void ITimer::start(const uint32_t interval)
{
    setInterval(interval);
    start();
}

uint32_t ITimer::interval() const
{
    return mInterval;
}

bool ITimer::isSingleShot() const
{
    return mIsSingleShot;
}

void ITimer::setInterval(uint32_t mInterval)
{
    this->mInterval = mInterval;
}

void ITimer::setSingleShot(bool singleshot)
{
    this->mIsSingleShot = singleshot;
}

ITimer::~ITimer()
{
}

