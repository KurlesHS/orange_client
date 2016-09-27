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
#include <chrono>

using namespace std;

class IOutgoingCommand {
public:

    enum class Result {
        Ok,
        Timeout
    };
    IOutgoingCommand();
    virtual ~IOutgoingCommand();

    virtual uint16_t command() const = 0;
    virtual vector<char> commandData() const = 0;

    virtual bool isNeedCheckAuth() const;
    virtual void onResult();

    uint64_t sequenceNum() const;
    void setSequenceNum(const uint64_t seq);

    chrono::system_clock::time_point sendingTime() const;
    void setSendingTime(chrono::system_clock::time_point mSendingTime);


private:
    uint64_t mSequenceNum;
    chrono::system_clock::time_point mSendingTime;


};

#endif /* IOUTGOINGCOMMAND_H */

