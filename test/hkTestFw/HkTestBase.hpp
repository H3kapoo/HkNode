#pragma once

#include <iostream>
#include <stdio.h>
#include <string.h>
#include <functional>
#include <vector>

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

/*Testing macros*/
#define EXPECT_EQ(x, y) if (!__EXPECT_EQ_COND(x, y)) return;
#define EXPECT_NOT_EQ(x, y) if (!__EXPECT_NOT_EQ__COND(x, y)) return;


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

            if (!lastTestFailed)
            {
                printf("%s[%s] Running \"%s\" ==> ..PASSED%s\n", GREEN, SUITE_NAME, name, CLR);
            }
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
                printf("\n%s[%s] Suite failed!%s\n", RED, SUITE_NAME, CLR);
            }
            else
            {
                printf("%s[%s] Suite failed!%s\n", RED, SUITE_NAME, CLR);

            }
        }
    }

    template<typename A, typename B>
    bool __expect_sgn_impl(A x, B y, const char* sgn, long line, const char* func)
    {
        bool failed{ false };
        if (strcmp(sgn, "==") == 0 && !(x == y))
        {
            failed = true;
            std::cout << RED << "[" << SUITE_NAME << "] Running \"" << func << "\" ==> ..FAILED\n\tEXPECT_EQ: Expected \"" << y << "\" to be equal to \"" << x << "\"";
        }
        else if (strcmp(sgn, "!=") == 0 && !(x != y))
        {
            failed = true;
            printf("\e[1;31m");
            std::cout << RED << "[" << SUITE_NAME << "] Running \"" << func << "\" ==> ..FAILED\n\tEXPECT_NOT_EQ: Expected \"" << y << "\" to NOT be equal to \"" << x << "\"";
        }

        if (failed)
        {
            suiteFailed = true;
            lastTestFailed = true;
            std::cout << "\n\tFailed in \"" << func << "\" at line " << line << "\n";
            printf("\e[0m");
            return false;
        }
        return true;
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