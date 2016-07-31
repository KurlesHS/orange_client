/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   itimer.h
 * Author: Alexey
 *
 * Created on 28 июля 2016 г., 21:13
 */

#ifndef ITIMER_H
#define ITIMER_H

#include <memory>

#include "SimpleSignal/simplesignal.h"

class ITimer;

typedef std::shared_ptr<ITimer> ITimerSharedPtr;

class ITimerFactory {
public:
    virtual ~ITimerFactory() {}
    virtual ITimerSharedPtr getTimer(const uint32_t interval=0) = 0;
};

class ITimer {
public:
    ITimer(const uint32_t &interval);    
    virtual ~ITimer();
    
    uint32_t interval() const;
    void setInterval(uint32_t mInterval);
    bool isSingleShot() const;
    void setSingleShot(bool singleshot);
    
    virtual void start() = 0;
    virtual void stop() = 0;
    virtual bool isStarted() const = 0;
    
    void start(const uint32_t interval);
    
    
private:
    uint32_t mInterval;
    bool mIsSingleShot;
    
    declare_signal(timeout, void());
};

#endif /* ITIMER_H */

