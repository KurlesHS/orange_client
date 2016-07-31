/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   protocolimpl.cpp
 * Author: Alexey
 * 
 * Created on 31 июля 2016 г., 22:52
 */

#include "protocolimpl.h"
#include "network/itransport.h"
#include "command.h"

#include <ioc/resolver.h>
#include <timer/timerfactory.h>

#include <sstream>

static constexpr int waitDataTimeout = 5000;

class ProtocolImplPrivate
{
public:
    std::shared_ptr<ITransport> mTransport;
    std::string onReadyReadConnection;
    std::string onDisconnectConnection;
    std::string mUserName;
    ITimerSharedPtr mTimer;
    CommandParser mCommandParser;
    std::vector<char> mBuffer;
};

ProtocolImpl::ProtocolImpl(std::shared_ptr<ITransport> transport) :
    d(new ProtocolImplPrivate)
{
    d->mUserName = "unknown user name";
    d->mTimer = Resolver::resolveDi<TimerFactory>()->getTimer(waitDataTimeout);
    d ->mTransport = transport;
    d->onDisconnectConnection = d->mTransport->connect_disconnected([this]() {
        onDisconnected();
    });

    d->onReadyReadConnection = d->mTransport->connect_readyRead([this]() {
        onReadyRead();
    });
}

ProtocolImpl::~ProtocolImpl()
{

}

void ProtocolImpl::onDisconnected()
{
    std::stringstream ss;
    ss << "disconnected from " << d->mTransport->peerAddress() <<
            " (" << d->mUserName << ")";

    emit_logMessage(ss.str());
    emit_disconnected(this);
}

void ProtocolImpl::onReadyRead()
{

    const int bytesCount = d->mTransport->bytesAvailable();
    if (bytesCount > 0) {
        stopTimeoutTimer();
        int currentOffset = d->mBuffer.size();
        // читаем данные
        d->mBuffer.resize(currentOffset + bytesCount);
        d->mTransport->read(&d->mBuffer[currentOffset], bytesCount);
        // парсим данные
        switch (d->mCommandParser.parse(d->mBuffer.data(), d->mBuffer.size())) {
            case CommandParser::Result::Error:
                handleErrorCommand();
                break;
            case CommandParser::Result::Incomplete:
                handleIncompleteCommand();
                break;
            case CommandParser::Result::Ok:
                handleOkCommand();
                break;
        }
    }
}

void ProtocolImpl::handleErrorCommand()
{
    // при ошибке - рвём соединение и информируем об этом внешний мир
    std::stringstream ss;
    ss << "received mailformed (wrong) message from " << d->mTransport->peerAddress() <<
            " (" << d->mUserName << ")";

    emit_logMessage(ss.str());

    d->mTransport->close();

}

void ProtocolImpl::startTimeoutTimer()
{
    d->mTimer->start();
}

void ProtocolImpl::stopTimeoutTimer()
{
    d->mTimer->stop();
}

void ProtocolImpl::handleIncompleteCommand()
{
    // не полный пакет - просто включаем таймаут таймер
    startTimeoutTimer();
}

void ProtocolImpl::handleOkCommand()
{
    // 
}

void ProtocolImpl::sendResponse(IIncommingCommand *cmd)
{
    switch (cmd->result()) {
        case (IIncommingCommand::Result::Ok):
            handleSendOkResponse(cmd);
            break;
        case (IIncommingCommand::Result::AuthError):
            handleSendAuthErrorRsponse(cmd);
            break;
        default:
            handleSendCommandErrorResponse(cmd);
            break;
    };
}

    void ProtocolImpl::onWaitDataTimeout()
    {
        // при таймауте рвём соединение
        std::stringstream ss;
        ss << "timeout while waiting data from " << d->mTransport->peerAddress() <<
                " (" << d->mUserName << ")";

        emit_logMessage(ss.str());

        d->mTransport->close();

}   

void ProtocolImpl::handleSendAuthErrorRsponse(IIncommingCommand* cmd)
{
    
}

void ProtocolImpl::handleSendCommandErrorResponse(IIncommingCommand* cmd)
{

}

void ProtocolImpl::handleSendOkResponse(IIncommingCommand* cmd)
{

}
 