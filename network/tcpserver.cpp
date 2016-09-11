/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   tcpserver.cpp
 * Author: Alexey
 * 
 * Created on 28 июля 2016 г., 23:25
 */

#include "tcpserver.h"

#include "thread/threadregister.h"
#include "tcpsocket.h"
#include "helpers.h"
#include "ioc/resolver.h"

#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <resolv.h>
#include <errno.h>
#include <fcntl.h>
#include <list>

static int setnonblock(int fd)
{
    int flags;

    flags = fcntl(fd, F_GETFL);
    flags |= O_NONBLOCK;

    return fcntl(fd, F_SETFL, flags);
}

class TcpServerPrivate
{
public:
    TcpServer *q;
    ev::io mConnectionWatcher;
    std::list<intptr_t> mPendingSockets;
    intptr_t mServerId;
    std::string mError;
    int mMaxClients = 10000;

    void onNewConnection(ev::io &watcher, int flag)
    {
        (void)watcher;
        (void)flag;
        socklen_t len;
        sockaddr_in client_addr;
        intptr_t client_sd = accept(watcher.fd, (sockaddr*) & client_addr, &len);

        if (client_sd < 0) {
            return;
        }

        setnonblock(client_sd);
        mPendingSockets.emplace_back(client_sd);
        q->emit_newConnection();
        
    }
    
        bool startListen(int port)
    {
        if (mServerId >= 0) {
            // уже слушаем порт
            mError = "already binged";
            return false;
        }
        mServerId = socket(PF_INET, SOCK_STREAM, 0);

        if (mServerId < 0) {
            mError = "socket error";
            return false;
        }

        setnonblock(mServerId);
        struct sockaddr_in addr;
        bzero(&addr, sizeof (sockaddr_in));
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        addr.sin_addr.s_addr = INADDR_ANY;

        // для быстрого перебиндивания порта....
        int yes = 1;
        if (setsockopt(mServerId, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof (yes)) == 1) {
            mError = "setsockopt error";
            goto on_error;
        }

        if (bind(mServerId, (struct sockaddr *)&addr, sizeof (addr)) != 0) {
            mError = "bind error";
            goto on_error;
        }

        if (::listen(mServerId, mMaxClients) == 0) {
            mConnectionWatcher.start(mServerId, EV_READ);
            return true;
        }
        mError = "can't start listening port";
on_error:
        close(mServerId);
        mServerId = -1;
        return false;
    }
};

TcpServer::TcpServer(const ev::loop_ref &loop) :
    d(new TcpServerPrivate)
{
    d->q = this;
    d->mServerId = -1;
    d->mConnectionWatcher.set(loop);
    d->mConnectionWatcher.set<TcpServerPrivate, &TcpServerPrivate::onNewConnection>(d);

}

int TcpServer::maxClients() const
{
    return d->mMaxClients;
}



void TcpServer::setMaxClients(const int count)
{
    d->mMaxClients = count;
}



TcpServer::~TcpServer()
{
    delete d;
}

bool TcpServer::hasPendingConnections() const
{
    return d->mPendingSockets.size();
}

bool TcpServer::isListening() const
{
    return d->mServerId >= 0;
}

std::shared_ptr<TcpSocket> TcpServer::nextPendingConnection()
{    
    if (d->mPendingSockets.size()) {
        auto loop = ThreadRegister::loopForCurrentThread();
        if (loop.raw_loop) {
            auto it = d->mPendingSockets.front();
            d->mPendingSockets.pop_front();
            return std::make_shared<TcpSocket>(it, loop);
        }
    }
    return nullptr;  
}

std::shared_ptr<TcpSocket> TcpServer::nextPendingConnection(const ev::loop_ref& loop)
{
    if (d->mPendingSockets.size()) {
        auto it = d->mPendingSockets.front();
        d->mPendingSockets.pop_front();
        return std::make_shared<TcpSocket>(it, loop);
    } else {
        return nullptr;
    }
}

int TcpServer::pendingConnectionCount() const
{
    return d->mPendingSockets.size();
}

bool TcpServer::startListen(int port)
{   
    return d->startListen(port);
}

void TcpServer::stop()
{
   if (d->mConnectionWatcher.is_active()) {
        d->mConnectionWatcher.stop();
    }

    if (d->mServerId >= 0) {
        close(d->mServerId);
        d->mServerId = -1;
    }}







