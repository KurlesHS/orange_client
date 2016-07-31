/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   testtimerfactory.h
 * Author: Alexey
 *
 * Created on 24 июля 2016 г., 1:46
 */

#ifndef TESTTIMERFACTORY_H
#define TESTTIMERFACTORY_H

#include "timer/itimer.h"
#include <list>
#include <memory>
#include <algorithm>

class TestTimerFactory;

class FakeTimer : public ITimer {
public:

    FakeTimer(TestTimerFactory *factory);
    bool isStarted() const override;
    void start() override;
    void stop() override;

    virtual ~FakeTimer();

    int nextFireAfter() const;


    bool doIt();

private:
    int mStartTime;
    bool mIsActive = false;
    TestTimerFactory *mFactory;
    int mLastFiredTime;
};

class TestTimerFactory : public ITimerFactory {
public:
    TestTimerFactory();
    TestTimerFactory(const TestTimerFactory& orig);
    virtual ~TestTimerFactory();
    ITimerSharedPtr getTimer(const uint32_t interval) override;

    void removeDeletedTimers();

    void forward(int ms);

    int currentTime() const;

    int timersCount() const {
        return mTimers.size();
    }

private:
    std::list<std::weak_ptr<FakeTimer> > mTimers;
    int mCurrentTime;
};

TestTimerFactory::TestTimerFactory() :
mCurrentTime(0) {
}

TestTimerFactory::TestTimerFactory(const TestTimerFactory& orig) {
}

TestTimerFactory::~TestTimerFactory() {
}

ITimerSharedPtr TestTimerFactory::getTimer(const uint32_t interval) {
    auto timer = std::make_shared<FakeTimer>(this);
    timer->setInterval(interval);
    mTimers.emplace_back(timer);
    return timer;
}

int TestTimerFactory::currentTime() const {
    return mCurrentTime;
}

void TestTimerFactory::removeDeletedTimers() {
    mTimers.remove_if([](std::weak_ptr<FakeTimer> timer) {
        return timer.expired();
    });
}

using timerSharedPtr = std::weak_ptr<FakeTimer>;

void TestTimerFactory::forward(int ms) {
    auto pred = [](const timerSharedPtr &a, const timerSharedPtr & b) {
        return a.lock()->nextFireAfter() < b.lock()->nextFireAfter();
    };

    while (ms > 0) {
        removeDeletedTimers();
        mTimers.sort(pred);
        bool isExecuted = false;
        for (auto it = mTimers.begin(); it != mTimers.end(); ++it) {
            int delta = it->lock()->nextFireAfter();
            if (delta >= 0) {
                if (isExecuted && delta != 0) {
                    break;
                }
                if (ms >= delta) {
                    ms -= delta;
                    mCurrentTime += delta;
                    if (it->use_count()) {
                        isExecuted = true;
                        it->lock()->doIt();
                    }
                }
            }
        }

        if (!isExecuted) {
            mCurrentTime += ms;
            ms = 0;
        }
    }

}

bool FakeTimer::isStarted() const {
    return mIsActive;
}

void FakeTimer::start() {
    if (interval() > 0) {
        mIsActive = true;        
        mStartTime = mFactory->currentTime();
        mLastFiredTime = mStartTime;
    }
}

bool FakeTimer::doIt() {
    emit_timeout();
    mLastFiredTime = mFactory->currentTime();
    if (isSingleShot()) {
        stop();
    }
    return true;
}

int FakeTimer::nextFireAfter() const {
    if (!isStarted()) {
        return -1;
    }
    int delta = (mFactory->currentTime() - mStartTime) % interval();

    if (delta == 0 && mLastFiredTime != mFactory->currentTime()) {
        return delta;
    } else {
        return interval() - delta;
    }
}

void FakeTimer::stop() {
    mIsActive = false;
}

FakeTimer::~FakeTimer() {

}

FakeTimer::FakeTimer(TestTimerFactory* factory) :
ITimer(0),
mFactory(factory) {

}


#endif /* TESTTIMERFACTORY_H */

