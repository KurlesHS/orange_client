/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   protocolimpl.h
 * Author: Alexey
 *
 * Created on 31 июля 2016 г., 22:52
 */

#ifndef PROTOCOLIMPL_H
#define PROTOCOLIMPL_H


#include <memory>

#include "SimpleSignal/simplesignal.h"

#include "iincommingcommand.h"

#include "network/iprotocol.h"

class ProtocolImplPrivate;

class ITransport;

class ProtocolImpl : public IProtocol {
public:

    ProtocolImpl(std::shared_ptr<ITransport> transport);
    virtual ~ProtocolImpl();

    void sendResponse(IIncommingCommand *cmd);
    
protected:
    virtual void dataReceived(const vector<char> &data) override;
    virtual void onConnected() override;
    virtual void onDisconnected() override;

private:
    void onWaitDataTimeout();

    void handleOkCommand();
    void handleErrorCommand();
    void handleIncompleteCommand();
    
    void handleSendOkResponse(IIncommingCommand *cmd);
    void handleSendAuthErrorRsponse(IIncommingCommand *cmd);
    void handleSendCommandErrorResponse(IIncommingCommand *cmd);

    void startTimeoutTimer();
    void stopTimeoutTimer();
private:
    friend class ProtocolImplPrivate;
    ProtocolImplPrivate *d;

private:
    // вохдящяя команда - уже проверицированная
    declare_signal(incommingCommand, void(const IIncommingCommandSharedPtr &cmd));
    declare_signal(connected, void(ProtocolImpl*, const std::string &userName));
    declare_signal(disconnected, void(ProtocolImpl*));
    declare_signal(logMessage, void(const std::string &message));
};

#endif /* PROTOCOLIMPL_H */

