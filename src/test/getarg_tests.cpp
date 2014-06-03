#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/test/unit_test.hpp>

#include "util.h"

BOOST_AUTO_TEST_SUITE(getarg_tests)

static void
ResetArgs(const std::string& strArg)
{
    std::vector<std::string> vecArg;
    boost::split(vecArg, strArg, boost::is_space(), boost::token_compress_on);

    // Insert dummy executable name:
    vecArg.insert(vecArg.begin(), "testbitcoin");

    // Convert to char*:
    std::vector<const char*> vecChar;
    BOOST_FOREACH(std::string& s, vecArg)
        vecChar.push_back(s.c_str());

    ParseParameters(vecChar.size(), &vecChar[0]);
}

BOOST_AUTO_TEST_CASE(boolarg)
{
    ResetArgs("-ACT");
    BOOST_CHECK(GetBoolArg("-ACT"));
    BOOST_CHECK(GetBoolArg("-ACT", false));
    BOOST_CHECK(GetBoolArg("-ACT", true));

    BOOST_CHECK(!GetBoolArg("-fo"));
    BOOST_CHECK(!GetBoolArg("-fo", false));
    BOOST_CHECK(GetBoolArg("-fo", true));

    BOOST_CHECK(!GetBoolArg("-ACTo"));
    BOOST_CHECK(!GetBoolArg("-ACTo", false));
    BOOST_CHECK(GetBoolArg("-ACTo", true));

    ResetArgs("-ACT=0");
    BOOST_CHECK(!GetBoolArg("-ACT"));
    BOOST_CHECK(!GetBoolArg("-ACT", false));
    BOOST_CHECK(!GetBoolArg("-ACT", true));

    ResetArgs("-ACT=1");
    BOOST_CHECK(GetBoolArg("-ACT"));
    BOOST_CHECK(GetBoolArg("-ACT", false));
    BOOST_CHECK(GetBoolArg("-ACT", true));

    // New 0.6 feature: auto-map -nosomething to !-something:
    ResetArgs("-noACT");
    BOOST_CHECK(!GetBoolArg("-ACT"));
    BOOST_CHECK(!GetBoolArg("-ACT", false));
    BOOST_CHECK(!GetBoolArg("-ACT", true));

    ResetArgs("-noACT=1");
    BOOST_CHECK(!GetBoolArg("-ACT"));
    BOOST_CHECK(!GetBoolArg("-ACT", false));
    BOOST_CHECK(!GetBoolArg("-ACT", true));

    ResetArgs("-ACT -noACT");  // -ACT should win
    BOOST_CHECK(GetBoolArg("-ACT"));
    BOOST_CHECK(GetBoolArg("-ACT", false));
    BOOST_CHECK(GetBoolArg("-ACT", true));

    ResetArgs("-ACT=1 -noACT=1");  // -ACT should win
    BOOST_CHECK(GetBoolArg("-ACT"));
    BOOST_CHECK(GetBoolArg("-ACT", false));
    BOOST_CHECK(GetBoolArg("-ACT", true));

    ResetArgs("-ACT=0 -noACT=0");  // -ACT should win
    BOOST_CHECK(!GetBoolArg("-ACT"));
    BOOST_CHECK(!GetBoolArg("-ACT", false));
    BOOST_CHECK(!GetBoolArg("-ACT", true));

    // New 0.6 feature: treat -- same as -:
    ResetArgs("--ACT=1");
    BOOST_CHECK(GetBoolArg("-ACT"));
    BOOST_CHECK(GetBoolArg("-ACT", false));
    BOOST_CHECK(GetBoolArg("-ACT", true));

    ResetArgs("--noACT=1");
    BOOST_CHECK(!GetBoolArg("-ACT"));
    BOOST_CHECK(!GetBoolArg("-ACT", false));
    BOOST_CHECK(!GetBoolArg("-ACT", true));

}

BOOST_AUTO_TEST_CASE(stringarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-ACT", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-ACT", "eleven"), "eleven");

    ResetArgs("-ACT -bar");
    BOOST_CHECK_EQUAL(GetArg("-ACT", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-ACT", "eleven"), "");

    ResetArgs("-ACT=");
    BOOST_CHECK_EQUAL(GetArg("-ACT", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-ACT", "eleven"), "");

    ResetArgs("-ACT=11");
    BOOST_CHECK_EQUAL(GetArg("-ACT", ""), "11");
    BOOST_CHECK_EQUAL(GetArg("-ACT", "eleven"), "11");

    ResetArgs("-ACT=eleven");
    BOOST_CHECK_EQUAL(GetArg("-ACT", ""), "eleven");
    BOOST_CHECK_EQUAL(GetArg("-ACT", "eleven"), "eleven");

}

BOOST_AUTO_TEST_CASE(intarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-ACT", 11), 11);
    BOOST_CHECK_EQUAL(GetArg("-ACT", 0), 0);

    ResetArgs("-ACT -bar");
    BOOST_CHECK_EQUAL(GetArg("-ACT", 11), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);

    ResetArgs("-ACT=11 -bar=12");
    BOOST_CHECK_EQUAL(GetArg("-ACT", 0), 11);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 12);

    ResetArgs("-ACT=NaN -bar=NotANumber");
    BOOST_CHECK_EQUAL(GetArg("-ACT", 1), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);
}

BOOST_AUTO_TEST_CASE(doubledash)
{
    ResetArgs("--ACT");
    BOOST_CHECK_EQUAL(GetBoolArg("-ACT"), true);

    ResetArgs("--ACT=verbose --bar=1");
    BOOST_CHECK_EQUAL(GetArg("-ACT", ""), "verbose");
    BOOST_CHECK_EQUAL(GetArg("-bar", 0), 1);
}

BOOST_AUTO_TEST_CASE(boolargno)
{
    ResetArgs("-noACT");
    BOOST_CHECK(!GetBoolArg("-ACT"));
    BOOST_CHECK(!GetBoolArg("-ACT", true));
    BOOST_CHECK(!GetBoolArg("-ACT", false));

    ResetArgs("-noACT=1");
    BOOST_CHECK(!GetBoolArg("-ACT"));
    BOOST_CHECK(!GetBoolArg("-ACT", true));
    BOOST_CHECK(!GetBoolArg("-ACT", false));

    ResetArgs("-noACT=0");
    BOOST_CHECK(GetBoolArg("-ACT"));
    BOOST_CHECK(GetBoolArg("-ACT", true));
    BOOST_CHECK(GetBoolArg("-ACT", false));

    ResetArgs("-ACT --noACT");
    BOOST_CHECK(GetBoolArg("-ACT"));

    ResetArgs("-noACT -ACT"); // ACT always wins:
    BOOST_CHECK(GetBoolArg("-ACT"));
}

BOOST_AUTO_TEST_SUITE_END()
