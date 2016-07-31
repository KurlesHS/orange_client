/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   tst_endian.cpp
 * Author: Alexey
 *
 * Created on 22 июля 2016 г., 20:07
 */
#include <gtest/gtest.h>

// #include "endian.hpp"

// #include "endian.hpp"

template<typename T>
void toBigEndian(T val, char *data);

template<typename T>
void toLittleEndian(T val, char *data);

template<typename T> 
T fromBigEndian(const char *data);

template<typename T> 
T fromLittleEndian(const char *data);




TEST(TestEndian, Test64L)
{
    char buff[0x08];
    char expected[] = {0x78, 0x56, 0x34, 0x12, 0x78, 0x56, 0x34, 0x12};
    
    uint64_t uvalue = 0x1234567812345678;
    int64_t value = 0x1234567812345678;
    
    toLittleEndian<uint64_t>(uvalue, buff);
    ASSERT_TRUE(memcmp(buff, expected, sizeof(uint64_t) )== 0);
    toLittleEndian<int64_t>(value, buff);
    ASSERT_TRUE(memcmp(buff, expected, sizeof(uint64_t) )== 0);
    
    uint64_t ucomparedValue = fromLittleEndian<uint64_t>(expected);
    int64_t comparedValue = fromLittleEndian<int64_t>(expected);
    ASSERT_EQ(ucomparedValue, uvalue);
    ASSERT_EQ(comparedValue, value);
}

TEST(TestEndian, Test32L)
{
    char buff[0x04];
    char expected[] = {0x78, 0x56, 0x34, 0x12};
    
    uint32_t uvalue = 0x12345678;
    int32_t value = 0x12345678;
    
    toLittleEndian<uint32_t>(uvalue, buff);
    ASSERT_TRUE(memcmp(buff, expected, sizeof(uint32_t) )== 0);
    toLittleEndian<int32_t>(value, buff);
    ASSERT_TRUE(memcmp(buff, expected, sizeof(int32_t) )== 0);
    
    uint32_t ucomparedValue = fromLittleEndian<uint32_t>(expected);
    int32_t comparedValue = fromLittleEndian<int32_t>(expected);
    ASSERT_EQ(ucomparedValue, uvalue);
    ASSERT_EQ(comparedValue, value);
}

TEST(TestEndian, Test16L)
{
    char buff[0x02];
    char expected[] = {0x34, 0x12};
    
    uint16_t uvalue = 0x1234;
    int16_t value = 0x1234;
    
    toLittleEndian<uint16_t>(uvalue, buff);
    ASSERT_TRUE(memcmp(buff, expected, sizeof(uint16_t) )== 0);
    toLittleEndian<int16_t>(value, buff);
    ASSERT_TRUE(memcmp(buff, expected, sizeof(int16_t) )== 0);
    
    uint16_t ucomparedValue = fromLittleEndian<uint16_t>(expected);
    int16_t comparedValue = fromLittleEndian<int16_t>(expected);
    ASSERT_EQ(ucomparedValue, uvalue);
    ASSERT_EQ(comparedValue, value);
}

TEST(TestEndian, Test64B)
{
    char buff[0x08];
    char expected[] = {0x12, 0x34, 0x56, 0x78, 0x12, 0x34, 0x56, 0x78};
    
    uint64_t uvalue = 0x1234567812345678;
    int64_t value = 0x1234567812345678;
    
    toBigEndian<uint64_t>(uvalue, buff);
    ASSERT_TRUE(memcmp(buff, expected, sizeof(uint64_t) )== 0);
    toBigEndian<int64_t>(value, buff);
    ASSERT_TRUE(memcmp(buff, expected, sizeof(uint64_t) )== 0);
    
    uint64_t ucomparedValue = fromBigEndian<uint64_t>(expected);
    int64_t comparedValue = fromBigEndian<int64_t>(expected);
    ASSERT_EQ(ucomparedValue, uvalue);
    ASSERT_EQ(comparedValue, value);
}

TEST(TestEndian, Test32B)
{
    char buff[0x04];
    char expected[] = {0x12, 0x34, 0x56, 0x78};
    
    uint32_t uvalue = 0x12345678;
    int32_t value = 0x12345678;
    
    toBigEndian<uint32_t>(uvalue, buff);
    ASSERT_TRUE(memcmp(buff, expected, sizeof(uint32_t) )== 0);
    toBigEndian<int32_t>(value, buff);
    ASSERT_TRUE(memcmp(buff, expected, sizeof(int32_t) )== 0);
    
    uint32_t ucomparedValue = fromBigEndian<uint32_t>(expected);
    int32_t comparedValue = fromBigEndian<int32_t>(expected);
    ASSERT_EQ(ucomparedValue, uvalue);
    ASSERT_EQ(comparedValue, value);
}

TEST(TestEndian, Test16B)
{
    char buff[0x02];
    char expected[] = {0x12, 0x34};
    
    uint16_t uvalue = 0x1234;
    int16_t value = 0x1234;
    
    toBigEndian<uint16_t>(uvalue, buff);
    ASSERT_TRUE(memcmp(buff, expected, sizeof(uint16_t) )== 0);
    toBigEndian<int16_t>(value, buff);
    ASSERT_TRUE(memcmp(buff, expected, sizeof(int16_t) )== 0);
    
    uint16_t ucomparedValue = fromBigEndian<uint16_t>(expected);
    int16_t comparedValue = fromBigEndian<int16_t>(expected);
    ASSERT_EQ(ucomparedValue, uvalue);
    ASSERT_EQ(comparedValue, value);
}

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}