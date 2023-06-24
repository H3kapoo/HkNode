#pragma once

#include <iostream>
#include <stdio.h>
#include <string.h>
#include <functional>
#include <vector>
#include <sstream>

/*Utils*/
#define CLR "\e[0m"
#define RED "\e[1;31m"
#define GREEN "\e[1;32m"
#define SUITE_NAME suiteName.c_str()

#define PREPROC_START() void __start()
#define PREPROC_END() void __end()

/*Test declarations macros*/
#define TEST_DECLARE_AUX(suiteName, sep, funcName) tcs.push_back({&suiteName::funcName, #funcName})
#define TEST_DECLARE(suiteName, funcName) TEST_DECLARE_AUX(suiteName, _ , funcName);

#define DEFINE_SUITE(suiteName_, ...)\
void declareSuite()\
{\
    suiteName = #suiteName_;\
    __VA_ARGS__\
}\

#define __EXPECT_EQ_COND(x, y) __expect_sgn_impl(x, y, "==" ,__LINE__, __func__)
#define __EXPECT_NOT_EQ__COND(x, y) __expect_sgn_impl(x, y, "!=", __LINE__, __func__)
#define __EXPECT_GT__COND(x, y) __expect_sgn_impl(x, y, ">", __LINE__, __func__)
#define __EXPECT_LT__COND(x, y) __expect_sgn_impl(x, y, "<", __LINE__, __func__)
#define __EXPECT_OUTPUT__COND(x, y)\
    std::stringstream buffer;\
    std::streambuf* old = std::cout.rdbuf(buffer.rdbuf());\
    x;\
    std::string text = buffer.str();\
    std::cout.rdbuf(old);\
    return __expect_output_impl(text, y, __LINE__, __func__);\


/*Testing macros*/
#define EXPECT_EQ(x, y) if (!__EXPECT_EQ_COND(x, y)) return;
#define EXPECT_NOT_EQ(x, y) if (!__EXPECT_NOT_EQ__COND(x, y)) return;
#define EXPECT_GT(x, y) if (!__EXPECT_GT__COND(x, y)) return;
#define EXPECT_LT(x, y) if (!__EXPECT_LT__COND(x, y)) return;
#define EXPECT_OUTPUT(x, y)\
    std::stringstream buffer;\
    std::streambuf* old = std::cout.rdbuf(buffer.rdbuf());\
    x;\
    std::string text = buffer.str();\
    std::cout.rdbuf(old);\
    if (!__expect_output_impl(text, y, __LINE__, __func__)) return;\


template <typename T>
class IHkTCS
{
public:
    void runTests()
    {
        declareSuite();
        uint32_t cnt = 0;
        for (auto& [func, name] : tcs)
        {
            cnt++;
            if (cnt == tcs.size())
            {
                isLastTest = true;
            }

            setup();
            lastTestFailed = false;
            ((T*)this->*func)();
            teardown();
        }


        if (suiteFailed)
        {
            if (!isLastTest)
            {
                printf("\n%s[%s] Suite failed!%s\n", RED, SUITE_NAME, CLR);
            }
            else
            {
                printf("%s[%s] Suite failed!%s\n", RED, SUITE_NAME, CLR);

            }
        }
        else
        {
            if (!isLastTest)
            {
                printf("\n%s[%s] Suite Passed!%s\n", GREEN, SUITE_NAME, CLR);
            }
            else
            {
                printf("%s[%s] Suite Passed!%s\n", GREEN, SUITE_NAME, CLR);

            }
        }
    }

    // template<typename A, typename B>
    bool __expect_output_impl(std::string x, std::string y, long line, const char* func)
    {
        bool failed{ false };
        if (!(x == y))
        {
            failed = true;
            printFailExpectation(func, "EXPECT_EQ: Expected\n", x, " to be equal to\n", y);
        }
        if (failed)
        {
            suiteFailed = true;
            lastTestFailed = true;
            std::cout << "\n\tFailed at line " << line << "\n";
            return false;
        }
        return true;
    }

    template<typename A, typename B>
    bool __expect_sgn_impl(A x, B y, const char* sgn, long line, const char* func)
    {
        bool failed{ false };
        if (strcmp(sgn, "==") == 0 && !(x == y))
        {
            failed = true;
            printFailExpectation(func, "EXPECT_EQ: Expected ", x, " to be equal to ", y);
        }
        else if (strcmp(sgn, "!=") == 0 && !(x != y))
        {
            failed = true;
            printFailExpectation(func, "EXPECT_NOT_EQ: Expected ", x, " to NOT be equal to ", y);
        }
        else if (strcmp(sgn, ">") == 0 && !(x > y))
        {
            failed = true;
            printFailExpectation(func, "EXPECT_GT: Expected ", x, " to be greater than ", y);
        }
        else if (strcmp(sgn, "<") == 0 && !(x < y))
        {
            failed = true;
            printFailExpectation(func, "EXPECT_LT: Expected ", x, " to be less than ", y);
        }

        if (failed)
        {
            suiteFailed = true;
            lastTestFailed = true;
            std::cout << "\n\tFailed at line " << line << "\n";
            return false;
        }
        return true;
    }

    template<typename A, typename B>
    void printFailExpectation(const char* func, const char* expectType, A x, const char* text, B y)
    {
        std::cout << RED << "[" << SUITE_NAME << "] Running " << CLR << "\"" << func
            << "\"" << RED << " ==> ..FAILED\n  ----> " << expectType << CLR
            << "\"" << RED << x << CLR << "\"" << RED << text << CLR << "\"" << GREEN << y << CLR << "\"" << RED;
    }

    virtual void declareSuite() {}
    virtual void setup() {}
    virtual void teardown() {}
    virtual ~IHkTCS() = default;

    std::vector<std::pair<void(T::*)(), const char*>> tcs;
    bool suiteFailed{ false };
    bool lastTestFailed{ false };
    bool isLastTest{ false };
    std::string suiteName;
};