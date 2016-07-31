/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   tcpsocket.cpp
 * Author: Alexey
 * 
 * Created on 28 июля 2016 г., 23:25
 */

#include "tcpsocket.h"
#include "itransport.h"

#include <vector>

#include <unistd.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#include <ev++.h>

class TcpSocketPrivate
{
public:
    TcpSocket *q;
    int mSocketId;
    int mReadOffset;
    int mWriteLen;
    ev::io mReadWriteWatcher;
    std::vector<char> mReadBuff;
    std::vector<char> mWriteBuff;
    std::string mPeerAddress;

    TcpSocketPrivate(TcpSocket *q, ev::loop_ref loop, int socketId) :
        q(q),
        mSocketId(socketId),
        mReadOffset(0),
        mWriteLen(0)
    {
        mReadBuff.resize(0x1000);
        mWriteBuff.resize(0x1000);
        mReadWriteWatcher.set(loop);
        mReadWriteWatcher.set<TcpSocketPrivate, &TcpSocketPrivate::readWriteCallback>(this);
        if (socketId >= 0) {
            // вытаскиваем адрес клиента
            mPeerAddress = getPeerAddress();
            // мониторим активность клиента
            mReadWriteWatcher.start(socketId, EV_READ);
        }
    }

    std::string getPeerAddress()
    {
        socklen_t len;
        struct sockaddr_storage addr;
        char ipstr[INET6_ADDRSTRLEN + 1];
        int port;
        len = sizeof addr;
        getpeername(mSocketId, (struct sockaddr*)&addr, &len);

        // deal with both IPv4 and IPv6:
        if (addr.ss_family == AF_INET) {
            struct sockaddr_in *s = (struct sockaddr_in *)&addr;
            port = ntohs(s->sin_port);
            inet_ntop(AF_INET, &s->sin_addr, ipstr, sizeof ipstr);
        } else { // AF_INET6
            struct sockaddr_in6 *s = (struct sockaddr_in6 *)&addr;
            port = ntohs(s->sin6_port);
            inet_ntop(AF_INET6, &s->sin6_addr, ipstr, sizeof ipstr);
        }        
        (void)port;
        return std::string(ipstr);
        
    }

    void readWriteCallback(ev::io &io, int flags)
    {
        (void)io;
        if (flags & EV_ERROR) {
            // ошибка
            close();
        }

        if (flags & EV_READ) {
            readCallback();
        }

        if (flags & EV_WRITE) {
            writeCallback();
        }
    }

    void readCallback()
    {
        int count;
        ioctl(mSocketId, FIONREAD, &count);

        uint total = mReadOffset + count;
        if (mReadBuff.size() < total) {
            mReadBuff.resize(total);
        }

        ssize_t nread = recv(mSocketId, &mReadBuff[mReadOffset], count, 0);

        if (nread < 0) {
            // read error///
            q->emit_disconnected();
            close();
        } else if (nread == 0) {
            // close event
            q->emit_disconnected();
            close();
        } else {
            mReadOffset += nread;
            // std::cout << nread << std::endl;
            q->emit_readyRead();
        }
    }

    void writeCallback()
    {
        if (mWriteLen == 0) {
            mReadWriteWatcher.set(ev::READ);
        } else {
            int count = ::write(mSocketId, &mWriteBuff[0], mWriteLen);
            if (count < 0) {
                // ошибка
                close();
            } else {
                int remain = mWriteLen - count;
                if (remain > 0) {
                    memmove(&mWriteBuff[0], &mWriteBuff[count], remain);
                }
                mWriteLen = remain;
                if (mWriteLen == 0) {
                    mReadWriteWatcher.set(ev::READ);
                }
            }
        }
    }

    void close()
    {
        if (mSocketId >= 0) {
            mReadWriteWatcher.stop();
            ::close(mSocketId);
            mSocketId = 0;
            mWriteLen = 0;
            mReadOffset = 0;
        }
    }

    int read(char *buff, int len)
    {
        len = mReadOffset < len ? mReadOffset : len;
        int bytesRemain = mReadOffset - len;
        memcpy(buff, &mReadBuff[0], len);
        if (bytesRemain) {
            memmove(&mReadBuff[0], &mReadBuff[len], bytesRemain);
        }
        mReadOffset = bytesRemain;
        return len;
    }

    int write(char *buff, int len)
    {
        size_t totalBytes = mWriteLen + len;
        if (mWriteBuff.size() < totalBytes) {
            mWriteBuff.resize(totalBytes);
        }
        memcpy(&mWriteBuff[mWriteLen], buff, len);
        mWriteLen += len;
        mReadWriteWatcher.set(ev::READ | ev::WRITE);

        return len;
    }

    int bytesAvailable() const
    {
        return mReadOffset;
    }
};

TcpSocket::TcpSocket(ev::loop_ref loop) :
    TcpSocket(-1, loop)
{

}

std::string TcpSocket::peerAddress() const
{
    return d->mPeerAddress;
}

TcpSocket::~TcpSocket()
{
    disconnect_disconnected();
    close();    
    delete d;
}

TcpSocket::TcpSocket(int socketId, ev::loop_ref loop) :
    d(new TcpSocketPrivate(this, loop, socketId))
{

}

int TcpSocket::bytesAvailable() const
{
    return d->bytesAvailable();
}

void TcpSocket::close()
{
    return d->close();
}

int TcpSocket::read(char* buff, int len)
{
    return d->read(buff, len);
}

int TcpSocket::write(char* buff, int len)
{
    return d->write(buff, len);
}
