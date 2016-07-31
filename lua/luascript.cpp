/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   luascript.cpp
 * Author: Alexey
 * 
 * Created on 9 июля 2016 г., 18:01
 */

#include <uuid/uuid.h>
#include <sol2.hpp>

#include "luascript.h"
#include "timer/timerfactory.h"
#include "timer/itimer.h"
#include "ioc/resolver.h"

#include <list>
#include <string>
#include <unordered_map>

#include "uuid.h"



static const char priorityVarName[] = "priority";
static const char groupVarName[] = "group";

static const char startFuncName[] = "start";
static const char cleanupOnCancelFuncName[] = "cleanup_on_cancel";
static const char finishFuncName[] = "finish";
static const char logMessageFuncName[] = "log_message";
static const char playLocalFileByFilepathFuncName[] = "play_local_file_by_filepath";
static const char playLocalFileByFilehashFuncName[] = "play_local_file_by_filehash";
static const char stopPlayLocalFileFuncName[] = "stop_play_local_file";
static const char playNetworkAudioFuncName[] = "play_network_audio";
static const char stopPlayNetworkAudioFuncName[] = "stop_play_network_audio";
static const char ifHappensFuncName[] = "if_happens";
static const char delayFuncName[] = "delay";
static const char cancelPendingFuncFuncName[] = "cancel_pending_func";

static const char onRelayChagedFuncName[] = "on_relay_changed";


/*
static const char FuncName[] = "";
static const char FuncName[] = "";
static const char FuncName[] = "";
 */

class IfHappensPendingFunc;

typedef std::shared_ptr<IfHappensPendingFunc> IfHappensPendingFuncSharedPtr;

class IfHappensPendingFunc 
{
public:
    using onExecuteFunc = std::function<void(const std::string &)>;

    IfHappensPendingFunc(const std::string &pendingId,
            const std::string &ifHappensId,
            const int delay, const sol::function &okFunc,
            const sol::function &errorFunc) :
        mPendingId(pendingId),
        mIfHappensId(ifHappensId),
        mDelay(delay),
        mOkFunc(okFunc),
        mErrorFunc(errorFunc)
    {
        mTimer = Resolver::resolveDi<ITimerFactory>()->getTimer();
    }

    ~IfHappensPendingFunc()
    {
        stop();
    }

    virtual void start()
    {
        mTimer->setSingleShot(true);
        mTimer->connect_timeout([this]() {onTimeout(); });
        mTimer->start(mDelay);
    }

    void onTimeout() 
    {
        if (mOnTimeoutFunc) {
            mOnTimeoutFunc(mPendingId);
        }
    }

    virtual void stop()
    {
        mTimer->stop();
    }

    template<typename ...Args>
    void executeOkFunc(Args ...args)
    {
        stop();
        if (mOkFunc) {
            mOkFunc(args...);
        }
    }

    void executeErrorFunc()
    {
        if (mErrorFunc) {
            mErrorFunc();
        }       
    }

    std::string ifHappensId() const
    {
        return mIfHappensId;
    }

    void setOnTimeoutFunc(onExecuteFunc func)
    {
        mOnTimeoutFunc = func;
    }

private:
    const std::string mPendingId;
    const std::string mIfHappensId;
    const int mDelay;
    const sol::function mOkFunc;
    const sol::function mErrorFunc;
    ITimerSharedPtr mTimer;

    onExecuteFunc mOnTimeoutFunc;
};

struct OnRelayChangedParams
{
    sol::object relayNum;
    sol::object relayState;
};

class LuaScriptPrivate
{
    friend class LuaScript;
    LuaScript *q;
    std::string scriptPath;
    std::string group = "default_group";
    sol::state state;
    bool isValid = true;
    bool isFinished = false;
    bool isFinishedEmited = false;
    std::string lastError;
    int priority = 0;
    std::string id;
    
    ITimerFactory *timerFactory;
    std::unordered_map<std::string, IfHappensPendingFuncSharedPtr> mIfHappensFunc;
    std::unordered_map<std::string, OnRelayChangedParams> mRelayChangeParams;

    void removeIfHappensParams(const std::string &idHappensId)
    {
        mRelayChangeParams.erase(idHappensId);
    }   

    void onRelayChanged(const int relayNum, const int newState)
    {
        for (auto it = mRelayChangeParams.begin();it != mRelayChangeParams.end();) {
            bool relayIsSame = true;
            bool stateIsSame = true;
            if (it->second.relayNum.is<int>()) {
                relayIsSame = relayNum == it->second.relayNum.as<int>();
            }
            if (it->second.relayState.is<int>()) {
                stateIsSame = newState == it->second.relayState.as<int>();
            }
            if (relayIsSame && stateIsSame) {
                // совпали как номер реле так и его состояние
                // ищем функции с этим if happens id и запускаем их
                findAndExecute(it->first, relayNum, newState);
                // удаляем эти параметры
                it = mRelayChangeParams.erase(it);
            } else {
                ++it;
            }
        }
    }

    template<typename ...Args>
    void findAndExecute(const std::string &ifHappensId, Args ...args)
    {
        for (auto it = mIfHappensFunc.begin();
                it != mIfHappensFunc.end();) {
            if (it->second->ifHappensId() == ifHappensId) {
                // идентификаторы совпали
                try {
                    // пытаемся запустить
                    it->second->executeOkFunc(args...);
                } catch (sol::error error) {
                    // если ошибка - помечаем скрипт как ошибочный
                    setLastError(error.what());
                    break;
                }
                it = mIfHappensFunc.erase(it);
            } else {
                ++it;
            }
        }
        // проверка скрипта на завершённость
        checkIfFinished();
    }

    std::string onRelayChangedFuncImpl(sol::object relayNum, sol::object newState)
    {
        auto id = Uuid::createUuid().toString();
        mRelayChangeParams[id] = {relayNum, newState};
        return id;
    }

    void checkIfFinished()
    {
        if (isFinished && !isFinishedEmited) {
            isFinishedEmited = true;
            q->emit_finishScript(id);
        }
    }

    void setLastError(const std::string &lastError)
    {
        this->lastError = lastError;
        isValid = false;
        isFinished = true;
    }

    void cancelPendingFuncImpl(const std::string &pendingId)
    {
        try {
            auto func = mIfHappensFunc.at(pendingId);
            // при удалении penfing'a нужно удалить так же и привязанные к
            // нему условиия if happens
            removeIfHappensParams(func->ifHappensId());
            mIfHappensFunc.erase(pendingId);
        } catch (std::out_of_range) {

        }
    }

    void createId()
    {
        id = Uuid::createUuid().toString();
    }

    /* functions */

    void finishFuncImpl()
    {
        isFinished = true;
    }

    std::string delayFuncImpl(const double &seconds, const sol::function &func)
    {
        // delay - тот же ифхаппенс, с пустой ok функцией 
        return ifHappensFuncImpl(seconds, std::string(), sol::function(), func);
    }

    std::string ifHappensFuncImpl(
            const double &second, const std::string &ifHappensId,
            const sol::function &okFunc, const sol::function &errorFunc)
    {
        std::string id;
        if (second <= 0) {
            if (errorFunc) {
                // если кол-во секунд меньше или равно нулю и присутствует 
                // функция ошибки - сразу вызываем ее
                errorFunc();
                checkIfFinished();
            }
        } else {
            if (okFunc || errorFunc) {
                // одна из функций должна обязательно присутствовать
                id = Uuid::createUuid().toString();
                // создаем экземпляр
                auto func = std::make_shared<IfHappensPendingFunc>(
                        id, ifHappensId, second * 1000, okFunc, errorFunc);
                // сохраняем его
                mIfHappensFunc[id] = func;
                func->setOnTimeoutFunc([this](const std::string & pendingId) {
                    // функция, вызываемая по таймауту 
                    try {
                        auto func = mIfHappensFunc.at(pendingId);
                        try {
                            // пытаемся вызвать функцию ошибки
                            func->executeErrorFunc();
                        } catch (sol::error error) {
                            setLastError(error.what());
                        }
                    } catch (std::out_of_range) {

                    }
                    // удаляем инфу об функции
                    cancelPendingFuncImpl(pendingId);
                    checkIfFinished();
                });
                // запускаем таймер ожидания события
                func->start();
            }
        }
        return id;
    }

    void logMessageFuncImpl(const std::string &message)
    {
        q->emit_logMessage(message);
    }

    std::string playLocalFileByFilepathFuncImpl(const std::string &filepath)
    {
        std::string id = Uuid::createUuid().toString();
        q->emit_playLocalFileByPathRequest(id, filepath);        
        return id;
    }

    std::string playLocalFileByFilehashFuncImpl(const std::string &filehash)
    {
        std::string id = Uuid::createUuid().toString();
        q->emit_playLocalFileByHashRequest(id, filehash);        
        return id;
    }

    void stopPlayLocalFileFuncImpl(const std::string &id)
    {
        q->emit_stopLocalAduioRequest(id);        
    }

    void playNetworkAudioFuncImpl(const std::string &url)
    {
        q->emit_playNetworkAduioRequest(url);
    }

    void stopPlayNetworkAudioFuncImpl()
    {
        q->emit_stopNetworkAduioRequest();
    }
};

std::string LuaScript::id() const
{
    return d->id;
}

float LuaScript::getFloatVar(const std::string& varName)
{
    return d->state.get<float>(varName);
}

int LuaScript::getIntegerVar(const std::string& varName)
{
    return d->state.get<int>(varName);
}

std::string LuaScript::getStringVar(const std::string& varName)
{
    return d->state.get<std::string>(varName);
}

LuaScript::LuaScript(const std::string &scriptPath) :
    d(new LuaScriptPrivate)
{
    d->timerFactory = Resolver::resolveDi<ITimerFactory>();
    d->createId();
    d->q = this;
    d->scriptPath = scriptPath;
    d->state.open_libraries(sol::lib::base, sol::lib::string, sol::lib::math);
    try {
        d->state.script_file(scriptPath);
        sol::function startFunc = d->state[startFuncName];
        // если в скрипте нет функции старт - беда, счиатем что в скрипте 
        // ошибка
        if (!startFunc) {
            d->setLastError(std::string(R"(function "start" is missing)"));
            return;
        }
        // попытка прочитать приоритет (по умолчанию - 0).
        sol::object priorityObject = d->state[priorityVarName];
        if (priorityObject.is<int>()) {
            // если переменная приоритета присутствует - назначаем
            // указанный приоритет
            d->priority = priorityObject.as<int>();
        }
        // попытка прочитать группу (по умолчанию - 0).
        sol::object groupObject = d->state[groupVarName];
        if (groupObject.is<std::string>()) {
            // если переменная приоритета присутствует - назначаем
            // указанный приоритет
            d->group = groupObject.as<std::string>();
        }

        d->state.set_function(finishFuncName,
                &LuaScriptPrivate::finishFuncImpl, d);
        d->state.set_function(delayFuncName,
                &LuaScriptPrivate::delayFuncImpl, d);
        d->state.set_function(ifHappensFuncName,
                &LuaScriptPrivate::ifHappensFuncImpl, d);
        d->state.set_function(cancelPendingFuncFuncName,
                &LuaScriptPrivate::cancelPendingFuncImpl, d);
        d->state.set_function(finishFuncName,
                &LuaScriptPrivate::finishFuncImpl, d);
        d->state.set_function(logMessageFuncName,
                &LuaScriptPrivate::logMessageFuncImpl, d);
        d->state.set_function(playLocalFileByFilehashFuncName,
                &LuaScriptPrivate::playLocalFileByFilehashFuncImpl, d);
        d->state.set_function(playLocalFileByFilepathFuncName,
                &LuaScriptPrivate::playLocalFileByFilepathFuncImpl, d);
        d->state.set_function(stopPlayLocalFileFuncName,
                &LuaScriptPrivate::stopPlayLocalFileFuncImpl, d);
        d->state.set_function(playNetworkAudioFuncName,
                &LuaScriptPrivate::playNetworkAudioFuncImpl, d);
        d->state.set_function(stopPlayNetworkAudioFuncName,
                &LuaScriptPrivate::stopPlayNetworkAudioFuncImpl, d);
        
        d->state.set_function(onRelayChagedFuncName, 
                &LuaScriptPrivate::onRelayChangedFuncImpl, d);
    } catch (sol::error error) {
        d->setLastError(error.what());
    }
}

LuaScript::~LuaScript()
{
    delete d;
}

int LuaScript::priority() const
{
    return d->priority;
}

std::string LuaScript::group() const
{
    return d->group;
}

bool LuaScript::isValid() const
{
    return d->isValid;
}

std::string LuaScript::lastError() const
{
    return d->lastError;
}

bool LuaScript::isFinished() const
{
    return d->isFinished;
}

bool LuaScript::run()
{
    bool result = false;
    if (isValid()) {
        try {
            d->state.script(std::string(startFuncName) + "()");
            d->checkIfFinished();
            result = true;
        } catch (sol::error error) {
            d->setLastError(error.what());
            d->checkIfFinished();
        }
    } else {
        d->setLastError("attempt to run invalid script");
    }
    return result;
}

void LuaScript::cancel()
{
    if (!isFinished() && isValid()) {
        // имеет смысл только когда скрипт в работе
        sol::function cleanOnCancel = d->state[cleanupOnCancelFuncName];
        if (cleanOnCancel) {
            // в скрипте присутствует функция cleanup_on_cancel
            try {
                cleanOnCancel();
            } catch (sol::error error) {
                d->setLastError(error.what());
            }
        }
        d->isFinished = true;
        d->checkIfFinished();
    }
}

void LuaScript::onRelayChanged(const int relayNum, const int newState)
{
    d->onRelayChanged(relayNum, newState);
}


int LuaScript::pendingIfHanpensFunctionCount() const
{
    return d->mIfHappensFunc.size();
}

int LuaScript::relayChangedActiveConditionsCount() const
{
    return d->mRelayChangeParams.size();
}
