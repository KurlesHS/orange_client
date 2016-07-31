/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   resolver.h
 * Author: Alexey
 *
 * Created on 17 июля 2016 г., 22:10
 */

#ifndef RESOLVER_H
#define RESOLVER_H

#include <unordered_map>
#include <typeinfo>
#include <mutex>

#define di_inject(_type, _var_name) _type *_var_name = \
                Resolver::resolveDi<_type>();

#define di_register_type(_base_type, _type, ...) \
                Resolver::registerDi<_base_type, _type>(__VA_ARGS__);
#define di_register_object(_type, _onbject) \
                Resolver::registerDiObject<_type>(_onbject);

class Resolver {
public:    

    template <typename BaseT, typename T, typename ...Args>
    static void registerDi(Args ... args) {
        std::lock_guard<std::mutex> locker(mutex());
        registeredTypes()[std::string(typeid (BaseT).name())] = new T(args...);
    }
    
    template <typename T>
    static void registerDiObject(T *object) {
        std::lock_guard<std::mutex> locker(mutex());
        registeredTypes()[std::string(typeid (T).name())] = object;
    }

    template <typename T>
    static T* resolveDi() {
        std::lock_guard<std::mutex> locker(mutex());
        T *result = nullptr;
        try {
            result = (T *) registeredTypes().at(std::string(typeid (T).name()));
        } catch (std::out_of_range) {       
            
        }
        return  result;
    }
    
    

private:
    static inline std::unordered_map<std::string, void*> &registeredTypes() {
        static std::unordered_map<std::string, void*> mRegisteredTypes;
        return mRegisteredTypes;        
    }
    
    static inline std::mutex &mutex() {
        static std::mutex mMutex;
        return mMutex;
    }
    
};

#endif /* RESOLVER_H */

