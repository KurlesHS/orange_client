#include <gtest/gtest.h>

#include "protocol/command.h"


TEST(TestCommandParser, MainTest)
{
    Command cmd;
    cmd.command = 0x0001;
    cmd.data = {1, 2, 3, 4};
    cmd.sequenceNum = 1;
    cmd.updateSign("test", "test", "test");
    
    auto data = CommandParser::bulidPacket(cmd);
    
    CommandParser parser;
    auto result = parser.parse(data.data(), data.size());
    ASSERT_EQ(result, CommandParser::Result::Ok);
    ASSERT_EQ(parser.cmd().command, cmd.command);
    ASSERT_EQ(parser.cmd().data, cmd.data);
    ASSERT_EQ(parser.cmd().sequenceNum, cmd.sequenceNum);
    ASSERT_EQ(memcmp(parser.cmd().sign, cmd.sign, sizeof(cmd.sign)), 0);
    ASSERT_EQ(sizeof(cmd.sign), 28);
    ASSERT_EQ(cmd.commandTotalLen(), command_header_lenght + 4);
        
    ASSERT_TRUE(parser.cmd().checkSign("test", "test", "test"));    
}

int main(int argc, char *argv[])
{

    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}