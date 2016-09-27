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
#include "command.h"

#include "network/itransport.h"
#include "uuid.h"
#include "auth/authmanager.h"

#include <ioc/resolver.h>
#include <timer/timerfactory.h>

#include <sstream>
#include <thread>
#include <list>

static constexpr int waitDataTimeout = 5000;

class ProtocolImplPrivate
{
public:
    ProtocolImpl *q;
    AuthManager *mAuthManager;

    std::string onReadyReadConnection;
    std::string onDisconnectConnection;
    std::string mUserName;
    // TODO: заполнить поле "пароль"
    string mPassword;
    std::string mSessionId;
    ITimerSharedPtr mTimer;
    CommandParser mCommandParser;
    std::vector<char> mBuffer;
    uint64_t mSequenceNum;

    bool mIsAuthorized;

    std::list<shared_ptr<IOutgoingCommand>> mCommands;
    uint64_t mAuthCmdSequenceNumber;

    uint64_t getNextSequenceNumber()
    {
        return mSequenceNum++;
    }

    bool sendCmd(shared_ptr<IOutgoingCommand> cmd)
    {
        auto transport = q->transport();
        if (!transport) {
            return false;
        }
        Command c;
        c.command = cmd->command();
        c.data = cmd->commandData();
        c.sequenceNum = cmd->sequenceNum();
        if (cmd->isNeedCheckAuth()) {
            c.updateSign(mUserName, mPassword, mSessionId);
        }
        auto packet = CommandParser::bulidPacket(c);
        cmd->setSendingTime(chrono::system_clock::now());
        transport->write(packet.data(), packet.size());
        return false;
    }

    void logAndCloseConnections(const string &message)
    {
        q->emit_logMessage(message);
        auto transport = q->transport();
        if (!transport) {
            return;
        }
        transport->close();

    }
};

ProtocolImpl::ProtocolImpl(std::shared_ptr<ITransport> transport, AuthManager *authManager) :
    IProtocol(transport),
    d(new ProtocolImplPrivate)
{
    d->q = this;
    d->mAuthManager = authManager;
    d->mTimer = Resolver::resolveDi<TimerFactory>()->getTimer(waitDataTimeout);
    d->mSessionId = Uuid::createUuid().toString();
    d->mSequenceNum = 0;
    d->mIsAuthorized = false;

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
    // Соединились - шлём команду авторизации
    d->mAuthCmdSequenceNumber = d->getNextSequenceNumber();
    Command c;
    c.command = 0x0001;
    c.data = vector<char>(d->mSessionId.begin(), d->mSessionId.end());
    c.sequenceNum = d->mAuthCmdSequenceNumber;
    auto packet = CommandParser::bulidPacket(c);
    transport()->write(packet.data(), packet.size());
}

void ProtocolImpl::addCommand(shared_ptr<IOutgoingCommand> cmd)
{
    cmd->setSequenceNum(d->mSequenceNum++);
    d->mCommands.push_back(cmd);
    if (d->mIsAuthorized) {
        d->sendCmd(cmd);
    }
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
    auto cmd = d->mCommandParser.cmd();
    // удаляем 
    if (cmd.commandTotalLen() >= d->mBuffer.size()) {
        d->mBuffer.clear();
    } else {
        d->mBuffer.erase(d->mBuffer.begin(), d->mBuffer.begin() + cmd.commandTotalLen());
    }
    if (cmd.command == 0x0000) {
        // отклик

        if (!d->mIsAuthorized && cmd.sequenceNum == d->mAuthCmdSequenceNumber) {
            d->mUserName = string(cmd.data.begin(), cmd.data.end());
            d->mPassword = d->mAuthManager->getUserPassword(d->mUserName);
            // вытащили данные для авторизации, теперь проверка валидности
            // команды в общем порядке 
        }
        
        

        // Авторизированы, нужно проверить валидность подписи
        if (cmd.checkSign(d->mUserName, d->mPassword, d->mSessionId)) {
            // подпись валидна
        } else {
            std::stringstream ss;
            ss << "disconnected from " << transport()->peerAddress() <<
                    ": incorrect credentials";
            d->logAndCloseConnections(ss.str());
        }

    } else {
        // какая-то входящая команда
    }
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
