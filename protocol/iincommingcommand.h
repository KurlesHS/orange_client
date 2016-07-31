/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   iincommingcommand.h
 * Author: Alexey
 *
 * Created on 31 июля 2016 г., 23:06
 */

#ifndef IINCOMMINGCOMMAND_H
#define IINCOMMINGCOMMAND_H

#include <memory>
#include "command.h"

class ProtocolImpl;

class IIncommingCommand;

typedef std::shared_ptr<IIncommingCommand> IIncommingCommandSharedPtr;

class IIncommingCommand {
public:
    enum class CommandType {
        Auth = 0x0001, 
        StartScript = 0x0002,
        CancelScript = 0x0003,
        RelayStatus = 0x0004        
    };
    
    enum class Result : uint16_t {
        Ok = 0x0000, 
        AuthError = 0xfffe,
        CommandError = 0xffff
    };
    IIncommingCommand(ProtocolImpl *protocolImpl);
    virtual ~IIncommingCommand();
    
    virtual CommandType commandType() const = 0;
    
    virtual void done(Result result);    
    Result result() const;    
    uint16_t resultCommandCode() const;   
    std::string commandId() const;
    std::vector<char> commandPayload() const;
    
protected:
    void setCommandPayload(const std::vector<char> &payload);   
    
private:
    ProtocolImpl *mProtoImpl;
    std::string mCommandId;
    std::vector<char> mCommandPayload;
    Result mResult;
    

};

#endif /* IINCOMMINGCOMMAND_H */

