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
    std::string onReadyReadConnection;
    std::string onDisconnectConnection;
    std::string mUserName;
    ITimerSharedPtr mTimer;
    CommandParser mCommandParser;
    std::vector<char> mBuffer;
};

ProtocolImpl::ProtocolImpl(std::shared_ptr<ITransport> transport) :
    IProtocol(transport),
    d(new ProtocolImplPrivate)
{
    d->mUserName = "unknown user name";
    d->mTimer = Resolver::resolveDi<TimerFactory>()->getTimer(waitDataTimeout);
}

ProtocolImpl::~ProtocolImpl()
{

}

void ProtocolImpl::onDisconnected()
{
    std::stringstream ss;
    ss << "disconnected from " << transport()->peerAddress() <<
            " (" << d->mUserName << ")";

    emit_logMessage(ss.str());
    emit_disconnected(this);
}

void ProtocolImpl::dataReceived(const vector<char>& data)
{
    stopTimeoutTimer();
    auto newSize = data.size() + d->mBuffer.size();
    d->mBuffer.reserve(newSize);
    d->mBuffer.insert(d->mBuffer.end(), data.begin(), data.end());
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

void ProtocolImpl::onConnected()
{

}

void ProtocolImpl::handleErrorCommand()
{
    // при ошибке - рвём соединение и информируем об этом внешний мир
    std::stringstream ss;
    ss << "received mailformed (wrong) message from " << transport()->peerAddress() <<
            " (" << d->mUserName << ")";

    emit_logMessage(ss.str());

    transport()->close();

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
    ss << "timeout while waiting data from " << transport()->peerAddress() <<
            " (" << d->mUserName << ")";

    emit_logMessage(ss.str());

    transport()->close();

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
