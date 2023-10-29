#include "catch2/catch_all.hpp"
#include "FreqCounter.hpp"
#include <sstream>
#include "ThreadPool.hpp"
#include <fstream>
#include "Logger.hpp"

TEST_CASE("test1")
{
    std::string input = "The time has come, the Walrus said,"
                        "to talk of many things...";

    std::istringstream stream(input);

    freq::FreqCounter<> fc;

    auto result = fc.FromStream(stream);

    REQUIRE(result);

    std::ostringstream out_stream;

    result = fc.ToStream(out_stream);
    REQUIRE(result);

    std::string cmp = "2 the\n"
                      "1 come\n"
                      "1 has\n"
                      "1 many\n"
                      "1 of\n"
                      "1 said\n"
                      "1 talk\n"
                      "1 things\n"
                      "1 time\n"
                      "1 to\n"
                      "1 walrus\n";

    REQUIRE(cmp == out_stream.str());
}

TEST_CASE("test2")
{
    std::string input = "The&time&has&come,&the&Walrus&said,"
                        "to&talk&of&many&many&many&things...";

    std::istringstream stream(input);

    freq::FreqCounter<> fc;

    auto result = fc.FromStream(stream);

    REQUIRE(result);

    std::ostringstream out_stream;

    result = fc.ToStream(out_stream);
    REQUIRE(result);

    std::string cmp = "3 many\n"
                      "2 the\n"
                      "1 come\n"
                      "1 has\n"
                      "1 of\n"
                      "1 said\n"
                      "1 talk\n"
                      "1 things\n"
                      "1 time\n"
                      "1 to\n"
                      "1 walrus\n";

    REQUIRE(cmp == out_stream.str());
}

TEST_CASE("thread pool test")
{
    freq::ThreadPool pool(2);

    std::string input_one = "The&time&has&come,&the&Walrus&said,"
                            "to&talk&of&many&many&many&things...";

    std::string input_two = "The time has come, the Walrus said,"
                            "to talk of many things...";

    std::istringstream stream_one(input_one);
    std::istringstream stream_two(input_two);

    freq::FreqCounter<freq::MultiThreadPolicy<>> fc;

    auto future_one = pool.enqueue([&fc, &stream_one]()
                                   { return fc.FromStream(stream_one); });
    auto future_two = pool.enqueue([&fc, &stream_two]()
                                   { return fc.FromStream(stream_two); });

    auto result = future_one.get();
    REQUIRE(result);
    result = future_two.get();
    REQUIRE(result);

    std::ostringstream out_stream;

    auto out_future = pool.enqueue([&fc, &out_stream]()
                                   { return fc.ToStream(out_stream); });
    result = out_future.get();
    REQUIRE(result);

    std::string cmp = "4 many\n"
                      "4 the\n"
                      "2 come\n"
                      "2 has\n"
                      "2 of\n"
                      "2 said\n"
                      "2 talk\n"
                      "2 things\n"
                      "2 time\n"
                      "2 to\n"
                      "2 walrus\n";

    REQUIRE(cmp == out_stream.str());
}

TEST_CASE("empty test")
{
    std::ifstream input;

    freq::FreqCounter<> fc;

    auto result = fc.FromStream(input);

    REQUIRE(!result);
    freq::LOG() << result.err_message;

    std::ostringstream out_stream;

    result = fc.ToStream(out_stream);
    REQUIRE(result);

    std::string cmp;

    REQUIRE(cmp == out_stream.str());
}

TEST_CASE("fail test")
{
    std::string input = "The time has come, the Walrus said,"
                        "to talk of many things...";

    std::istringstream stream(input);

    freq::FreqCounter<> fc;

    auto result = fc.FromStream(stream);

    REQUIRE(result);

    std::ofstream out_stream;

    result = fc.ToStream(out_stream);
    REQUIRE(!result);
    freq::LOG() << result.err_message;
}