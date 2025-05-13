#include "chat/common/Result.hpp"

#include <catch2/catch_test_macros.hpp>

namespace
{
constexpr int SUCCESS_VALUE = 42;
constexpr int NEW_SUCCESS_VALUE = 43;
constexpr int ERROR_VALUE = 24;
constexpr int NEW_ERROR_VALUE = 25;
}

TEST_CASE("Creating a success result", "[Result]")
{
    chat::common::Result<int, int> result{SUCCESS_VALUE};
    REQUIRE(result.hasValue());
    REQUIRE(result.getValue() == SUCCESS_VALUE);
    REQUIRE_THROWS(result.getError());
}

TEST_CASE("Assigning a new success value to a success result", "[Result]")
{
    chat::common::Result<int, int> result{SUCCESS_VALUE};
    result = NEW_SUCCESS_VALUE;
    REQUIRE(result.hasValue());
    REQUIRE(result.getValue() == NEW_SUCCESS_VALUE);
    REQUIRE_THROWS(result.getError());
}

TEST_CASE("Assigning an error value to a success result", "[Result]")
{
    chat::common::Result<int, int> result{SUCCESS_VALUE};
    result = chat::common::Error{ERROR_VALUE};
    REQUIRE(!result.hasValue());
    REQUIRE_THROWS(result.getValue());
    REQUIRE(result.getError() == ERROR_VALUE);
}

TEST_CASE("Creating an error result", "[Result]")
{
    chat::common::Result<int, int> result{chat::common::Error{ERROR_VALUE}};
    REQUIRE(!result.hasValue());
    REQUIRE_THROWS(result.getValue());
    REQUIRE(result.getError() == ERROR_VALUE);
}

TEST_CASE("Assigning a success value to an error result", "[Result]")
{
    chat::common::Result<int, int> result{chat::common::Error{ERROR_VALUE}};
    result = SUCCESS_VALUE;
    REQUIRE(result.hasValue());
    REQUIRE(result.getValue() == SUCCESS_VALUE);
    REQUIRE_THROWS(result.getError());
}

TEST_CASE("Assigning a new error value to an error result", "[Result]")
{
    chat::common::Result<int, int> result{chat::common::Error{ERROR_VALUE}};
    result = chat::common::Error{NEW_ERROR_VALUE};
    REQUIRE(!result.hasValue());
    REQUIRE_THROWS(result.getValue());
    REQUIRE(result.getError() == NEW_ERROR_VALUE);
}

TEST_CASE("Creating a simple success result", "[Result]")
{
    chat::common::Result<int> result{SUCCESS_VALUE};
    REQUIRE(result.hasValue());
    REQUIRE(result.getValue() == SUCCESS_VALUE);
}

TEST_CASE("Assigning a success value to a simple success result", "[Result]")
{
    chat::common::Result<int> result{SUCCESS_VALUE};
    result = NEW_SUCCESS_VALUE;
    REQUIRE(result.hasValue());
    REQUIRE(result.getValue() == NEW_SUCCESS_VALUE);
}

TEST_CASE("Creating a simple error result", "[Result]")
{
    chat::common::Result<int> result;
    REQUIRE(!result.hasValue());
    REQUIRE_THROWS(result.getValue());
}

TEST_CASE("Assigning a success value to a simple error result", "[Result]")
{
    chat::common::Result<int> result;
    result = SUCCESS_VALUE;
    REQUIRE(result.hasValue());
    REQUIRE(result.getValue() == SUCCESS_VALUE);
}
