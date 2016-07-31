#include <gtest/gtest.h>

#include "ioc/resolver.h"

class TestClassBase
{
public:
    virtual int get() const = 0;

    virtual ~TestClassBase()
    {

    }



};

class TestClass : public TestClassBase
{
public:

    TestClass(int i = 10) :
        i(i)
    {

    }

    int get() const override
    {
        return i;
    }

    int i = 10;

};

TEST(TestResolver, MainTest)
{
    di_register_type(TestClassBase, TestClass, 22);
    
    di_inject(TestClassBase, tcb);
    ASSERT_TRUE(tcb);    
    ASSERT_EQ(tcb->get(), 22);
    
    di_inject(TestClass, tc);
    ASSERT_FALSE(tc);    
}

int main(int argc, char *argv[])
{

    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}