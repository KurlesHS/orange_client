/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   uuid.cpp
 * Author: Alexey
 * 
 * Created on 10 июля 2016 г., 15:37
 */

#include <string.h>
#include <string>

#include "uuid.h"

Uuid::Uuid(const uuid_t data)
{
    memcpy(mUuidBytes, data, sizeof (uuid_t));
}

Uuid::Uuid(const Uuid& orig)
{    
    if (this != &orig) {
        memcpy(mUuidBytes, orig.mUuidBytes, sizeof (uuid_t));
    }
}

Uuid& Uuid::operator=(const Uuid& orig)
{
    // возможно оператор этот лишний - компилятор и так создаёт код
    // копирования массивов при копировании объектов класса/структур
    if (this != &orig) {
        memcpy(mUuidBytes, orig.mUuidBytes, sizeof (uuid_t));
    }
    return *this;
}

Uuid::~Uuid()
{
}

bool Uuid::operator==(const Uuid &other) const
{
    return memcmp(other.mUuidBytes, this->mUuidBytes, sizeof (uuid_t)) == 0;
}

bool Uuid::operator!=(const Uuid& other) const
{
    return !operator==(other);
}

Uuid Uuid::createUuid()
{
    uuid_t uuid;
    uuid_generate(uuid);
    return Uuid(uuid);
}

Uuid Uuid::fromString(const std::string &uuid, bool *ok)
{
    uuid_t _uuid;
    bool _ok = false;
    memset(_uuid, 0, sizeof (uuid_t));
    if (uuid.length() == 36) {
        uuid_parse(uuid.data(), _uuid);
        _ok = true;
    }
    if (ok) {
        *ok = _ok;
    }
    return Uuid(_uuid);
}

std::string Uuid::toString() const
{
    char buff[37];
    uuid_unparse(mUuidBytes, buff);
    return std::string(buff);
}


