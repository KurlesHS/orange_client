/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   command.h
 * Author: Alexey
 *
 * Created on 22 июля 2016 г., 17:05
 */

#ifndef COMMAND_H
#define COMMAND_H

#include <vector>
#include <string>

static constexpr size_t command_header_lenght = 46;

struct Command {
    uint64_t sequenceNum;
    uint16_t command;
    char sign[28]; // sha224
    std::vector<char> data;

    size_t commandTotalLen() const {
        return data.size() + command_header_lenght;
    }
    
    void updateSign(const std::string &username, const std::string &password,
            const std::string &sessionId);
    
    bool checkSign(const std::string &username, const std::string &password,
            const std::string &sessionId);
};

class CommandParser {
public:

    enum class Result {
        Ok,
        Incomplete,
        Error
    };
    Command cmd() const;

    Result parse(const char *data, size_t length);

    static std::vector<char> bulidPacket(const Command &cmd);    

private:
    Command mCommand;
};


#endif /* COMMAND_H */

