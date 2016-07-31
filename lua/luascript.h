/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   luascript.h
 * Author: Alexey
 *
 * Created on 9 июля 2016 г., 18:01
 */



#ifndef LUASCRIPT_H
#define LUASCRIPT_H

#include <string>
#include <SimpleSignal/simplesignal.h>

class LuaScriptPrivate;

class ILuaPendingFunc {
public:

    virtual ~ILuaPendingFunc() {
    }

};

class LuaScript {
public:
    LuaScript(const std::string &scriptPath);
    virtual ~LuaScript();

    bool run();
    void cancel();

    std::string id() const;

    bool isValid() const;
    bool isFinished() const;
    std::string lastError() const;

    int priority() const;
    std::string group() const;

    void onRelayChanged(const int relayNum, const int newState);

    std::string getStringVar(const std::string &varName);
    int getIntegerVar(const std::string &varName);
    float getFloatVar(const std::string &varName);

    int pendingIfHanpensFunctionCount() const;
    int relayChangedActiveConditionsCount() const;

private:
    friend class LuaScriptPrivate;
    LuaScriptPrivate *d;
    declare_signal(finishScript, void(const std::string & scriptId));
    declare_signal(logMessage, void(const std::string &));
    declare_signal(playNetworkAduioRequest, void(const std::string &));
    declare_signal(stopNetworkAduioRequest, void());
    declare_signal(playLocalFileByHashRequest, void(const std::string &, const std::string &));
    declare_signal(playLocalFileByPathRequest, void(const std::string &, const std::string &));
    declare_signal(stopLocalAduioRequest, void(const std::string &));




};

#endif /* LUASCRIPT_H */


