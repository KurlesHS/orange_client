/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   authmanager.cpp
 * Author: Alexey
 * 
 * Created on 27 сентября 2016 г., 21:58
 */

#include "authmanager.h"

AuthManager::AuthManager()
{
}

AuthManager::~AuthManager()
{
}

void AuthManager::addUser(const string& userName, const string& userPassword)
{    
    mUsersInfo[userName] = userPassword;
}

string AuthManager::getUserPassword(const string &userName) const
{    
    auto it = mUsersInfo.find(userName);
    if (it == mUsersInfo.end()) {
        return string();
    }
    return it->second;
}


