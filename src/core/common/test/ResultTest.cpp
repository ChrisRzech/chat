#include "chat/common/Result.hpp"

#include <catch2/catch_test_macros.hpp>

namespace
{

constexpr int SUCCESS_VALUE = 42;
constexpr int NEW_SUCCESS_VALUE = 43;
constexpr int ERROR_VALUE = 24;
constexpr int NEW_ERROR_VALUE = 25;

}

SCENARIO("A result that can hold an error value", "[Result]")
{
    GIVEN("A result that is constructed with a success value")
    {
        chat::common::Result<int, int> result{SUCCESS_VALUE};

        THEN("The result holds the success value")
        {
            REQUIRE(result.isSuccess());
            CHECK(result.getSuccessValue() == SUCCESS_VALUE);
            CHECK_THROWS(result.getErrorValue());
        }

        WHEN("A new success value is assigned to the result")
        {
            result = NEW_SUCCESS_VALUE;

            THEN("The success value is replaced")
            {
                REQUIRE(result.isSuccess());
                CHECK(result.getSuccessValue() == NEW_SUCCESS_VALUE);
                CHECK_THROWS(result.getErrorValue());
            }
        }

        WHEN("A new error value is assigned to the result")
        {
            result = chat::common::Error{ERROR_VALUE};

            THEN(
                "The success value is destroyed and the new error value is "
                "held")
            {
                REQUIRE(!result.isSuccess());
                CHECK_THROWS(result.getSuccessValue());
                CHECK(result.getErrorValue() == ERROR_VALUE);
            }
        }
    }

    GIVEN("A result that is constructed with an error value")
    {
        chat::common::Result<int, int> result{chat::common::Error{ERROR_VALUE}};

        THEN("The result holds the error value")
        {
            REQUIRE(!result.isSuccess());
            CHECK_THROWS(result.getSuccessValue());
            CHECK(result.getErrorValue() == ERROR_VALUE);
        }

        WHEN("A new error value is assigned to the result")
        {
            result = chat::common::Error{NEW_ERROR_VALUE};

            THEN("The error value is replaced")
            {
                REQUIRE(!result.isSuccess());
                CHECK_THROWS(result.getSuccessValue());
                CHECK(result.getErrorValue() == NEW_ERROR_VALUE);
            }
        }

        WHEN("A new success value is assigned to the result")
        {
            result = SUCCESS_VALUE;

            THEN(
                "The error value is destroyed and the new success value is "
                "held")
            {
                REQUIRE(result.isSuccess());
                CHECK(result.getSuccessValue() == SUCCESS_VALUE);
                CHECK_THROWS(result.getErrorValue());
            }
        }
    }
}

SCENARIO("A result that cannot hold an error value", "[Result]")
{
    GIVEN("A result that is constructed without a success value")
    {
        chat::common::Result<int> result;

        THEN("The result does not hold a success value")
        {
            REQUIRE(!result.isSuccess());
            CHECK_THROWS(result.getSuccessValue());
        }

        WHEN("A new success value is assigned to the result")
        {
            result = SUCCESS_VALUE;

            THEN("The result holds the new success value")
            {
                REQUIRE(result.isSuccess());
                CHECK(result.getSuccessValue() == SUCCESS_VALUE);
            }
        }
    }

    GIVEN("A result that is constructed with a success value")
    {
        chat::common::Result<int> result{SUCCESS_VALUE};

        THEN("The result holds the success value")
        {
            REQUIRE(result.isSuccess());
            CHECK(result.getSuccessValue() == SUCCESS_VALUE);
        }

        WHEN("A new success value is assigned to the result")
        {
            result = NEW_SUCCESS_VALUE;

            THEN("The result holds the new success value")
            {
                REQUIRE(result.isSuccess());
                CHECK(result.getSuccessValue() == NEW_SUCCESS_VALUE);
            }
        }
    }
}
