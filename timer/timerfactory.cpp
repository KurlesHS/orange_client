/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   timerfactory.cpp
 * Author: Alexey
 * 
 * Created on 28 июля 2016 г., 23:01
 */

#include "timerfactory.h"

#include "thread/threadregister.h"

#include "ev++.h"

class Timer : public ITimer
{
public:

    Timer(const ev::loop_ref &loop, uint32_t interval) :
        ITimer(interval)
    {
        mTimer.set(loop);
        mTimer.set<Timer, &Timer::onTimeout>(this);
    }

    bool isStarted() const override
    {
        return mTimer.is_active();
    }

    void start() override
    {
        if (interval()) {
            float seconds = interval() / 1000.;
            if (isSingleShot()) {
                mTimer.start(seconds, 0.);
            } else {
                mTimer.start(seconds, seconds);
            }
        }
    }

    void stop() override
    {
        if (mTimer.is_active()) {
            mTimer.stop();
        }
    }

    void onTimeout()
    {
        emit_timeout();
    }

private:
    ev::timer mTimer;


};

ITimerSharedPtr TimerFactory::getTimer(const uint32_t interval)
{
    auto loop = ThreadRegister::loopForThread(std::this_thread::get_id());
    if (loop.raw_loop) {        
        return std::make_shared<Timer>(loop, interval);
    } 
    return nullptr;
}

