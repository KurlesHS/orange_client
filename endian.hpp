/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   endian.hpp
 * Author: Alexey
 *
 * Created on 22 июля 2016 г., 18:28
 */

#ifndef ENDIAN_HPP
#define ENDIAN_HPP

#include <endian.h>

template<typename T>
void toBigEndian(T val, char *data);

// unsigned
template<>
void toBigEndian<uint16_t>(uint16_t value, char *data) {
    value = htobe16(value);
    memcpy(data, &value, sizeof(uint16_t));
}

template<>
void toBigEndian<uint32_t>(uint32_t value, char *data) {
    value = htobe32(value);
    memcpy(data, &value, sizeof(uint32_t));
}

template<>
void toBigEndian<uint64_t>(uint64_t value, char *data) {
    value = htobe64(value);
    memcpy(data, &value, sizeof(uint64_t));
}

// signed
template<>
void toBigEndian<int16_t>(int16_t value, char *data) {
    value = htobe16(value);
    memcpy(data, &value, sizeof(int16_t));
}

template<>
void toBigEndian<int32_t>(int32_t value, char *data) {
    value = htobe32(value);
    memcpy(data, &value, sizeof(int32_t));
}

template<>
void toBigEndian<int64_t>(int64_t value, char *data) {
    value = htobe64(value);
    memcpy(data, &value, sizeof(int64_t));
}


template<typename T>
void toLittleEndian(T val, char *data);

// unsigned
template<>
void toLittleEndian<uint16_t>(uint16_t value, char *data) {
    value = htole16(value);
    memcpy(data, &value, sizeof(uint16_t));
}

template<>
void toLittleEndian<uint32_t>(uint32_t value, char *data) {
    value = htole32(value);
    memcpy(data, &value, sizeof(uint32_t));
}

template<>
void toLittleEndian<uint64_t>(uint64_t value, char *data) {
    value = htole64(value);
    memcpy(data, &value, sizeof(uint64_t));
}

// signed
template<>
void toLittleEndian<int16_t>(int16_t value, char *data) {
    value = htole16(value);
    memcpy(data, &value, sizeof(int16_t));
}

template<>
void toLittleEndian<int32_t>(int32_t value, char *data) {
    value = htole32(value);
    memcpy(data, &value, sizeof(int32_t));
}

template<>
void toLittleEndian<int64_t>(int64_t value, char *data) {
    value = htole64(value);
    memcpy(data, &value, sizeof(int64_t));
}

template<typename T> 
T fromLittleEndian(const char *data);

// unsigned
template<> 
uint16_t fromLittleEndian(const char *data) {
    uint16_t value;
    memcpy(&value, data, sizeof(uint16_t));
    return le16toh(value);
}

template<> 
uint32_t fromLittleEndian(const char *data) {
    uint32_t value;
    memcpy(&value, data, sizeof(uint32_t));
    return le32toh(value);
}

template<> 
uint64_t fromLittleEndian(const char *data) {
    uint64_t value;
    memcpy(&value, data, sizeof(uint64_t));
    return le64toh(value);
}

// signed
template<> 
int16_t fromLittleEndian(const char *data) {
    int16_t value;
    memcpy(&value, data, sizeof(int16_t));
    return le16toh(value);
}

template<> 
int32_t fromLittleEndian(const char *data) {
    int32_t value;
    memcpy(&value, data, sizeof(int32_t));
    return le32toh(value);
}

template<> 
int64_t fromLittleEndian(const char *data) {
    int64_t value;
    memcpy(&value, data, sizeof(int64_t));
    return le64toh(value);
}
template<typename T> 
T fromBigEndian(const char *data);

// unsigned
template<> 
uint16_t fromBigEndian(const char *data) {
    uint16_t value;
    memcpy(&value, data, sizeof(uint16_t));
    return be16toh(value);
}

template<> 
uint32_t fromBigEndian(const char *data) {
    uint32_t value;
    memcpy(&value, data, sizeof(uint32_t));
    return be32toh(value);
}

template<> 
uint64_t fromBigEndian(const char *data) {
    uint64_t value;
    memcpy(&value, data, sizeof(uint64_t));
    return be64toh(value);
}

// signed
template<> 
int16_t fromBigEndian(const char *data) {
    int16_t value;
    memcpy(&value, data, sizeof(int16_t));
    return be16toh(value);
}

template<> 
int32_t fromBigEndian(const char *data) {
    int32_t value;
    memcpy(&value, data, sizeof(int32_t));
    return be32toh(value);
}

template<> 
int64_t fromBigEndian(const char *data) {
    int64_t value;
    memcpy(&value, data, sizeof(int64_t));
    return be64toh(value);
}

#endif /* ENDIAN_HPP */

