/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   command.cpp
 * Author: Alexey
 * 
 * Created on 22 июля 2016 г., 17:05
 */


#include <string.h>
#include <stdint.h>
#include <stdlib.h>

#include "command.h"

#include <endian.hpp>

#include <openssl/sha.h>


constexpr static char signature[] = {(char)0xcd, 0x63, 0x1d, (char)0x9a};

CommandParser::Result CommandParser::parse(const char* data, size_t length)
{
    if (length < 4) {
        // меньше 4х байт - однозначно не полный пакет
        return Result::Incomplete;
    }
    if (memcmp(signature, data, 0x04) != 0) {
        // сигнатура не сходится - не верный пакет
        return Result::Error;
    }

    if (length < command_header_lenght) {
        // общая длинна меньше длинны заголовка - не полный пакет
        return Result::Incomplete;
    }
    size_t lenght = fromBigEndian<uint32_t>(data + 4);
    if (lenght + command_header_lenght < length) {
        return Result::Incomplete;
    }
    memcpy(mCommand.sign, data + 18, sizeof(mCommand.sign));
    mCommand.sequenceNum = fromBigEndian<uint64_t>(data + 8);
    mCommand.command = fromBigEndian<uint16_t>(data + 16);
    mCommand.data = std::vector<char>(data + command_header_lenght,
            data + command_header_lenght + lenght);
    return Result::Ok;
}

Command CommandParser::cmd() const
{
    return mCommand;
}

std::vector<char> CommandParser::bulidPacket(const Command& cmd)
{
    std::vector<char> result;
    result.resize(command_header_lenght + cmd.data.size());
    memcpy(&result[0], signature, 0x04);
    toBigEndian<uint32_t>(cmd.data.size(), &result[4]);
    toBigEndian<uint64_t>(cmd.sequenceNum, &result[8]);
    toBigEndian<uint16_t>(cmd.command, &result[16]);
    memcpy(&result[18], cmd.sign, 28);
    memcpy(&result[command_header_lenght], cmd.data.data(), cmd.data.size());
    return result;
}

void Command::updateSign(const std::string& username, const std::string& password, const std::string& sessionId)
{
    char buff[8];
    SHA256_CTX ctx;
    SHA224_Init(&ctx);
    toBigEndian<uint64_t>(sequenceNum, buff);
    SHA224_Update(&ctx, buff, 0x08); // sequence number
    SHA224_Update(&ctx, username.data(), username.length()); // user name
    SHA224_Update(&ctx, "_", 1); // separator    
    SHA224_Update(&ctx, password.data(), password.length()); // password
    SHA224_Update(&ctx, "_", 1); // separator    
    SHA224_Update(&ctx, sessionId.data(), sessionId.length()); // session id
    SHA224_Update(&ctx, data.data(), data.size());
    SHA224_Final((unsigned char*)sign, &ctx);
}

bool Command::checkSign(const std::string& username, const std::string& password, const std::string& sessionId)
{
    unsigned char digiest[SHA224_DIGEST_LENGTH];
    char buff[8];
    SHA256_CTX ctx;
    SHA224_Init(&ctx);
    toBigEndian<uint64_t>(sequenceNum, buff);
    SHA224_Update(&ctx, buff, 0x08); // sequence number
    SHA224_Update(&ctx, username.data(), username.length()); // user name
    SHA224_Update(&ctx, "_", 1); // separator    
    SHA224_Update(&ctx, password.data(), password.length()); // password
    SHA224_Update(&ctx, "_", 1); // separator    
    SHA224_Update(&ctx, sessionId.data(), sessionId.length()); // session id
    SHA224_Update(&ctx, data.data(), data.size());
    SHA224_Final(digiest, &ctx);
    return memcmp(digiest, sign, SHA224_DIGEST_LENGTH) == 0;
}
