/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   testtimerfactory.cpp
 * Author: Alexey
 * 
 * Created on 24 июля 2016 г., 1:46
 */

#include <algorithm>

#include "testtimerfactory.h"

testtimerfactory::testtimerfactory() :
    mCurrentTime(0)
{
}

testtimerfactory::testtimerfactory(const testtimerfactory& orig)
{
}

testtimerfactory::~testtimerfactory()
{
}

ITimerSharedPtr testtimerfactory::getTimerSharedPtr()
{
    auto timer = std::make_shared<FakeTimer>(this);
    mTimers.emplace_back(timer);
    return timer;
}

int testtimerfactory::currentTime() const
{
    return mCurrentTime;
}

void testtimerfactory::removeDeletedTimers()
{
    mTimers.remove_if([](std::weak_ptr<FakeTimer> timer) {
        return timer.expired();
    });
}

using timerSharedPtr = std::weak_ptr<FakeTimer>;

void testtimerfactory::forward(int ms)

{

    auto pred = [](const timerSharedPtr &a, const timerSharedPtr & b) {
        return a.lock()->nextFireAfter() < b.lock()->nextFireAfter();
    };

    while (ms > 0) {
        removeDeletedTimers();
        
        mTimers.sort(pred);
        bool fired = false;
        for (auto it = mTimers.begin();it != mTimers.end();) {
            int delta = it->lock()->nextFireAfter();
            if (delta > 0) {                
                if (ms >= delta) {
                    ms = -delta;
                    mCurrentTime += delta;
                    if (it->use_count()) {
                        it->lock()->doIt();
                        fired = true;
                        break;
                    }
                } else {
                    mCurrentTime += ms;
                    ms = 0;
                    break;
                }
            }
        }
        if (!fired) {
            mCurrentTime += ms;
            ms = 0;
        }
    }

}

bool FakeTimer::isActive() const
{
    return mIsActive;
}

void FakeTimer::start(int ms)
{
    if (ms > 0) {
        setTimeout(ms);
        mStartTime = mFactory->currentTime();
    }
}

void FakeTimer::doIt()
{
    fire();
    if (isSingleshot()) {
        stop();
    }
}

int FakeTimer::nextFireAfter() const
{
    if (!isActive()) {
        return -1;
    }
    int delta = (mFactory->currentTime() - mStartTime) % timeout();

    if (delta == 0) {
        return delta;
    } else {
        return timeout() - delta;
    }
}

void FakeTimer::stop()
{
    mIsActive = false;
}

FakeTimer::~FakeTimer()
{

}

FakeTimer::FakeTimer(testtimerfactory* factory) :
    mFactory(factory)
{

}
