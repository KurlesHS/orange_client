/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   startscriptcommand.h
 * Author: Alexey
 *
 * Created on 1 августа 2016 г., 0:28
 */

#ifndef STARTSCRIPTCOMMAND_H
#define STARTSCRIPTCOMMAND_H

#include "iincommingcommand.h"
#include "protocolimpl.h"

class StartScriptCommand : public IIncommingCommand {
public:
    StartScriptCommand(ProtocolImpl *protoImpl);
    virtual ~StartScriptCommand();
    
    
    
    
private:

};

#endif /* STARTSCRIPTCOMMAND_H */

