/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   testluascript.cpp
 * Author: Alexey
 *
 * Created on 10 июля 2016 г., 11:46
 */

#include <stdlib.h>
#include <iostream>
#include <sys/stat.h>
#include <gtest/gtest.h>

#include "lua/luascript.h"
#include "ioc/resolver.h"
#include "timer/itimer.h"
#include "testtimerfactory.h"
#include "sol2.hpp"

/*
 * Simple C++ Test Suite
 */

static const char nonExistFile[] = "file_does_not_exist.lua";
static const char scriptWhitoutStartFuncFile[] = "test_files/lua/test_01.lua";
static const char nonValidLuaFile[] = "test_files/lua/test_00.lua";
static const char validLuaFile[] = "test_files/lua/test_02.lua";
static const char validLuaFileWithErrorInStart[] = "test_files/lua/test_03.lua";
static const char luaWithDelay[] = "test_files/lua/test_04.lua";
static const char luaWithIfHappensAndRelayChanged[] = "test_files/lua/test_05.lua";

class TestLuaScript : public ::testing::Test
{
public:

    void SetUp() override
    {

    }

    void TearDown() override
    {

    }

    bool testIsFilePresent(const std::string &filepath)
    {
        struct stat buffer;
        return (stat(filepath.c_str(), &buffer) == 0);
    }
};

TEST_F(TestLuaScript, testNonValidLuaScript)
{
    ASSERT_TRUE(testIsFilePresent(nonValidLuaFile));
    LuaScript script(nonValidLuaFile);
    ASSERT_FALSE(script.isValid());
}

TEST_F(TestLuaScript, testValidLuaScript)
{
    ASSERT_TRUE(testIsFilePresent(validLuaFile));
    LuaScript script(validLuaFile);
    ASSERT_TRUE(script.isValid());
    ASSERT_EQ(script.priority(), 10);
    ASSERT_EQ(script.group(), "group");
    ASSERT_TRUE(script.run());
}

TEST_F(TestLuaScript, testLuaScriptLoad)
{
    ASSERT_TRUE(testIsFilePresent(scriptWhitoutStartFuncFile));
    LuaScript script(scriptWhitoutStartFuncFile);
    ASSERT_FALSE(script.isValid());
}

TEST_F(TestLuaScript, testNonExistFile)
{
    ASSERT_FALSE(testIsFilePresent(nonExistFile));
    LuaScript script(scriptWhitoutStartFuncFile);
    ASSERT_FALSE(script.isValid());
}

TEST_F(TestLuaScript, testValidLuaWithRuntimeErrorInStartFunc)
{
    ASSERT_TRUE(testIsFilePresent(validLuaFileWithErrorInStart));
    LuaScript script(validLuaFileWithErrorInStart);
    ASSERT_TRUE(script.isValid());
    ASSERT_FALSE(script.run());
    ASSERT_FALSE(script.isValid());
    ASSERT_TRUE(script.isFinished());
}

TEST_F(TestLuaScript, testDelayFunc)
{
    TestTimerFactory *factory = (TestTimerFactory*)Resolver::resolveDi<ITimerFactory>();
    ASSERT_TRUE(testIsFilePresent(luaWithDelay));
    LuaScript script(luaWithDelay);
    ASSERT_TRUE(script.isValid());
    int test_var = script.getIntegerVar("test_var");
    ASSERT_EQ(test_var, 0);
    ASSERT_TRUE(script.run());
    ASSERT_FALSE(script.isFinished());
    factory->forward(999);
    script.onRelayChanged(10, 20);
    ASSERT_EQ(test_var, 0);
    ASSERT_FALSE(script.isFinished());
    factory->forward(1);
    test_var = script.getIntegerVar("test_var");
    ASSERT_EQ(test_var, 1);
    ASSERT_TRUE(script.isFinished());
    ASSERT_TRUE(script.isValid());
}

TEST_F(TestLuaScript, testIfHappensAndRelayChangedEvent)
{
    TestTimerFactory *factory = (TestTimerFactory*)Resolver::resolveDi<ITimerFactory>();
    ASSERT_TRUE(testIsFilePresent(luaWithIfHappensAndRelayChanged));
    LuaScript script(luaWithIfHappensAndRelayChanged);
    ASSERT_TRUE(script.isValid());
    int ok_count1, ok_count2, ok_count3, ok_count4;
    int error_count1, error_count2, error_count3, error_count4;

    int relay_num1, relay_num2, relay_state;
    int is_canceled = -1;
    int finish_count = 0;

    struct Finish
    {

        void onSctiptFinished(const std::string &scriptId)
        {
            if (scriptId == mScriptId) {
                (*finish_count)++;
            }
        }

        std::string mScriptId;
        int *finish_count;
    };

    Finish finList;
    finList.finish_count = &finish_count;
    finList.mScriptId = script.id();
    script.connect_finishScript([&finList]
            (const std::string & id) {
        finList.onSctiptFinished(id);
    });

    auto updateVars = [&]() {
        error_count1 = script.getIntegerVar("on_error1_count");
        error_count2 = script.getIntegerVar("on_error2_count");
        error_count3 = script.getIntegerVar("on_error3_count");
        error_count4 = script.getIntegerVar("on_error4_count");

        ok_count1 = script.getIntegerVar("on_func1_count");
        ok_count2 = script.getIntegerVar("on_func2_count");
        ok_count3 = script.getIntegerVar("on_func3_count");
        ok_count4 = script.getIntegerVar("on_func4_count");

        relay_num1 = script.getIntegerVar("relay_num1");
        relay_num2 = script.getIntegerVar("relay_num2");
        relay_state = script.getIntegerVar("relay_state");

        is_canceled = script.getIntegerVar("is_canceled");
    };

    updateVars();
    ASSERT_EQ(ok_count1, 0);
    ASSERT_EQ(ok_count2, 0);
    ASSERT_EQ(ok_count3, 0);
    ASSERT_EQ(ok_count4, 0);

    ASSERT_EQ(error_count1, 0);
    ASSERT_EQ(error_count2, 0);
    ASSERT_EQ(error_count3, 0);
    ASSERT_EQ(error_count4, 0);

    ASSERT_TRUE(script.run());
    ASSERT_FALSE(script.isFinished());

    ASSERT_EQ(script.pendingIfHanpensFunctionCount(), 4);
    ASSERT_EQ(script.relayChangedActiveConditionsCount(), 4);

    factory->forward(500);

    ASSERT_EQ(script.pendingIfHanpensFunctionCount(), 3);
    ASSERT_EQ(script.relayChangedActiveConditionsCount(), 3);
    updateVars();

    ASSERT_EQ(ok_count1, 0);
    ASSERT_EQ(ok_count2, 0);
    ASSERT_EQ(ok_count3, 0);
    ASSERT_EQ(ok_count4, 0);

    ASSERT_EQ(error_count1, 0);
    ASSERT_EQ(error_count2, 0);
    ASSERT_EQ(error_count3, 0);
    ASSERT_EQ(error_count4, 1);

    script.onRelayChanged(10, 20);
    updateVars();

    ASSERT_EQ(ok_count1, 1);
    ASSERT_EQ(ok_count2, 1);
    ASSERT_EQ(ok_count3, 1);
    ASSERT_EQ(ok_count4, 0);

    ASSERT_EQ(error_count1, 0);
    ASSERT_EQ(error_count2, 0);
    ASSERT_EQ(error_count3, 0);
    ASSERT_EQ(error_count4, 1);

    ASSERT_EQ(relay_num1, 10);
    ASSERT_EQ(relay_num2, 10);
    ASSERT_EQ(relay_state, 20);

    ASSERT_EQ(is_canceled, 0);

    ASSERT_EQ(script.pendingIfHanpensFunctionCount(), 0);
    ASSERT_EQ(script.relayChangedActiveConditionsCount(), 0);

    factory->forward(10000);

    updateVars();

    ASSERT_EQ(ok_count1, 1);
    ASSERT_EQ(ok_count2, 1);
    ASSERT_EQ(ok_count3, 1);
    ASSERT_EQ(ok_count4, 0);

    ASSERT_EQ(error_count1, 0);
    ASSERT_EQ(error_count2, 0);
    ASSERT_EQ(error_count3, 0);
    ASSERT_EQ(error_count4, 1);

    ASSERT_EQ(relay_num1, 10);
    ASSERT_EQ(relay_num2, 10);
    ASSERT_EQ(relay_state, 20);

    ASSERT_EQ(is_canceled, 0);
    ASSERT_FALSE(script.isFinished());

    script.cancel();

    updateVars();
    ASSERT_TRUE(script.isFinished());
    ASSERT_EQ(is_canceled, 1);
    ASSERT_TRUE(script.isValid());
    ASSERT_EQ(*finList.finish_count, 1);
}

int main(int argc, char *argv[])
{
    di_register_type(ITimerFactory, TestTimerFactory);
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
