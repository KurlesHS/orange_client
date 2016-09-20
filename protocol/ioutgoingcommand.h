/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ioutgoingcommand.h
 * Author: Alexey
 *
 * Created on 19 сентября 2016 г., 19:31
 */

#ifndef IOUTGOINGCOMMAND_H
#define IOUTGOINGCOMMAND_H

#include <stdint.h>
#include <vector>

using namespace std;

class IOutgoingCommand {
public:
    enum class Result {
        Ok,
        Timeout
    };
    IOutgoingCommand();
    virtual ~IOutgoingCommand();
    
    virtual uint16_t command() const;
    virtual vector<char> commandData() const;
    
    virtual void onResult()
    
private:

};

#endif /* IOUTGOINGCOMMAND_H */

