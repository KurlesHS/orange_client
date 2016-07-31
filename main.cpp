/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.cpp
 * Author: Alexey
 *
 * Created on 27 июля 2016 г., 23:52
 */

#include <list>
#include <mutex>
#include <thread>
#include <memory>
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <functional>

#include <ev++.h>

#include "timer/timerfactory.h"
#include "thread/threadregister.h"
#include "network/tcpserver.h"
#include "network/tcpsocket.h"

using namespace std;
using namespace chrono;
#define lock_mutex() lock_guard<mutex> locker(mMutex);

class TestThread
{
public:

    void start()
    {
        mThread = thread(&TestThread::mainLoop, this);
    }

    void test()
    {

        if (mThread.get_id() == this_thread::get_id()) {
            // cout << "same thread" << endl << endl;
            ++mCounter;
            uint64_t elapsedSeconds = duration_cast<seconds>(system_clock::now() - mStartTime).count();
            if (elapsedSeconds != mLastSecond) {
                cout << mCounter << endl;
                mLastSecond = elapsedSeconds;
            }
        } else {
            {
                lock_mutex();
                // cout << "different thread" << endl;
                mPendingCommands.push_back([this]() {
                    test();
                });
            }
            mAsync.send();
        }
    }

    void eventHandler()
    {
        // cout << "event_handler" << endl;

        mMutex.lock();
        if (mPendingCommands.size()) {
            list < function<void()> > commands;
            mPendingCommands.swap(commands);
            mMutex.unlock();
            for (const auto &pendingFunction : commands) {
                pendingFunction();
            }
        } else {
            mMutex.unlock();
        }
    }

    void mainLoop()
    {
        cout << "loop" << endl;
        ev::dynamic_loop threadLoop;
        mAsync.set(threadLoop);
        mAsync.set<TestThread, &TestThread::eventHandler>(this);
        mAsync.start();

        threadLoop.run();

        mAsync.stop();
        mThread.detach();
    }

private:
    thread mThread;
    ev::async mAsync;
    list < function<void()> > mPendingCommands;
    mutex mMutex;
    uint64_t mCounter = 0;
    time_point<system_clock> mStartTime = system_clock::now();
    uint64_t mLastSecond = 0;
};

int main(int argc, char** argv)
{
    (void)argc;
    (void)argv;

    TestThread t;

    t.start();

    ev::default_loop mainLoop;

    ev::timer timer;
    timer.set(mainLoop);
    timer.set<TestThread, &TestThread::test>(&t);
    timer.start(0.001, 0.001);

    std::shared_ptr<int> shared = make_shared<int>(12);
    std::weak_ptr<int> weak(shared);
    std::weak_ptr<int> weak2 = weak;

    ThreadRegister::registerThread(std::this_thread::get_id(), mainLoop);
    TimerFactory TFactory;
    auto sharedTimer = TFactory.getTimer(500);
    sharedTimer->connect_timeout([]() {
        cout << "from factored timer" << endl;
    });
    sharedTimer->start();

    TcpServer server(mainLoop);
    cout << "start listen " << server.startListen(9000) << endl;
    std::list<std::shared_ptr < TcpSocket>> sockets;
    server.connect_newConnection([&server, &sockets]() {
        cout << "new connection" << endl;
        while (server.hasPendingConnections()) {
            auto socket = server.nextPendingConnection();
                    auto raw = socket.get();
                    sockets.emplace_back(socket);
                    auto weak = std::weak_ptr<TcpSocket>(socket);

                    socket->connect_disconnected([raw, &sockets]() {
                        cout << "disconnected" << endl;
                    });
            socket->connect_readyRead([socket]() {
                char buff[0x1000];
                while (socket->bytesAvailable()) {
                    int len = socket->read(buff, sizeof(buff));
                    socket->write(buff, len);
                }
            });

        }


    });

    Simple::Signal<void() > s;
    s.connect([]() {
        cout << "test signals" << endl;
    });
    s.emit();

    mainLoop.ref();
    mainLoop.run();
    return 0;
}

