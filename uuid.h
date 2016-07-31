/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   uuid.h
 * Author: Alexey
 *
 * Created on 10 июля 2016 г., 15:37
 */

#ifndef UUID_H
#define UUID_H

#include <string>
#include "uuid/uuid.h"


class Uuid {
public:    
    // 16 байт данных uuid
    Uuid(const uuid_t data);    
    Uuid(const Uuid& orig);
    virtual ~Uuid();
    
    static Uuid createUuid();
    static Uuid fromString(const std::string &uuid, bool *ok=0);
    
    std::string toString() const;
    
    bool operator==(const Uuid &other) const;
    bool operator!=(const Uuid &other) const;
    Uuid &operator =(const Uuid &orig);
    
private:
    uuid_t mUuidBytes;

};

#endif /* UUID_H */

