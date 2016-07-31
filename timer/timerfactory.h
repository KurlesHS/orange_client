/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   timerfactory.h
 * Author: Alexey
 *
 * Created on 28 июля 2016 г., 23:01
 */

#ifndef TIMERFACTORY_H
#define TIMERFACTORY_H

#include "itimer.h"

class TimerFactory : public ITimerFactory {
public:
    ITimerSharedPtr getTimer(const uint32_t interval) override;

private:

};

#endif /* TIMERFACTORY_H */

