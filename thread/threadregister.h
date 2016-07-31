/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   threadregister.h
 * Author: Alexey
 *
 * Created on 28 июля 2016 г., 22:34
 */

#ifndef THREADREGISTER_H
#define THREADREGISTER_H

#include <thread>

#include <ev++.h>
#include <unordered_map>

class ThreadRegister {
public:
    static void registerThread(std::thread::id threadId, ev::loop_ref loop) {
        registeredThreads().emplace(std::make_pair(threadId, loop));
    }
    
    static void unregisterThread(std::thread::id threadId) {
        registeredThreads().erase(threadId);
    }
    
    static ev::loop_ref loopForThread(std::thread::id threadId) {
        try {
            return registeredThreads().at(threadId);
        } catch (std::out_of_range) {
            return ev::loop_ref(nullptr);
        }
    }
    
    static ev::loop_ref loopForCurrentThread() {
        return loopForThread(std::this_thread::get_id());
    }
    
private:
    static std::unordered_map<std::thread::id, ev::loop_ref> &registeredThreads() {
        static std::unordered_map<std::thread::id, ev::loop_ref> t;
        return t;
    }
};
#endif /* THREADREGISTER_H */

