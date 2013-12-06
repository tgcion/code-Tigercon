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
    ResetArgs("-TGC");
    BOOST_CHECK(GetBoolArg("-TGC"));
    BOOST_CHECK(GetBoolArg("-TGC", false));
    BOOST_CHECK(GetBoolArg("-TGC", true));

    BOOST_CHECK(!GetBoolArg("-fo"));
    BOOST_CHECK(!GetBoolArg("-fo", false));
    BOOST_CHECK(GetBoolArg("-fo", true));

    BOOST_CHECK(!GetBoolArg("-TGCo"));
    BOOST_CHECK(!GetBoolArg("-TGCo", false));
    BOOST_CHECK(GetBoolArg("-TGCo", true));

    ResetArgs("-TGC=0");
    BOOST_CHECK(!GetBoolArg("-TGC"));
    BOOST_CHECK(!GetBoolArg("-TGC", false));
    BOOST_CHECK(!GetBoolArg("-TGC", true));

    ResetArgs("-TGC=1");
    BOOST_CHECK(GetBoolArg("-TGC"));
    BOOST_CHECK(GetBoolArg("-TGC", false));
    BOOST_CHECK(GetBoolArg("-TGC", true));

    // New 0.6 feature: auto-map -nosomething to !-something:
    ResetArgs("-noTGC");
    BOOST_CHECK(!GetBoolArg("-TGC"));
    BOOST_CHECK(!GetBoolArg("-TGC", false));
    BOOST_CHECK(!GetBoolArg("-TGC", true));

    ResetArgs("-noTGC=1");
    BOOST_CHECK(!GetBoolArg("-TGC"));
    BOOST_CHECK(!GetBoolArg("-TGC", false));
    BOOST_CHECK(!GetBoolArg("-TGC", true));

    ResetArgs("-TGC -noTGC");  // -TGC should win
    BOOST_CHECK(GetBoolArg("-TGC"));
    BOOST_CHECK(GetBoolArg("-TGC", false));
    BOOST_CHECK(GetBoolArg("-TGC", true));

    ResetArgs("-TGC=1 -noTGC=1");  // -TGC should win
    BOOST_CHECK(GetBoolArg("-TGC"));
    BOOST_CHECK(GetBoolArg("-TGC", false));
    BOOST_CHECK(GetBoolArg("-TGC", true));

    ResetArgs("-TGC=0 -noTGC=0");  // -TGC should win
    BOOST_CHECK(!GetBoolArg("-TGC"));
    BOOST_CHECK(!GetBoolArg("-TGC", false));
    BOOST_CHECK(!GetBoolArg("-TGC", true));

    // New 0.6 feature: treat -- same as -:
    ResetArgs("--TGC=1");
    BOOST_CHECK(GetBoolArg("-TGC"));
    BOOST_CHECK(GetBoolArg("-TGC", false));
    BOOST_CHECK(GetBoolArg("-TGC", true));

    ResetArgs("--noTGC=1");
    BOOST_CHECK(!GetBoolArg("-TGC"));
    BOOST_CHECK(!GetBoolArg("-TGC", false));
    BOOST_CHECK(!GetBoolArg("-TGC", true));

}

BOOST_AUTO_TEST_CASE(stringarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-TGC", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-TGC", "eleven"), "eleven");

    ResetArgs("-TGC -bar");
    BOOST_CHECK_EQUAL(GetArg("-TGC", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-TGC", "eleven"), "");

    ResetArgs("-TGC=");
    BOOST_CHECK_EQUAL(GetArg("-TGC", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-TGC", "eleven"), "");

    ResetArgs("-TGC=11");
    BOOST_CHECK_EQUAL(GetArg("-TGC", ""), "11");
    BOOST_CHECK_EQUAL(GetArg("-TGC", "eleven"), "11");

    ResetArgs("-TGC=eleven");
    BOOST_CHECK_EQUAL(GetArg("-TGC", ""), "eleven");
    BOOST_CHECK_EQUAL(GetArg("-TGC", "eleven"), "eleven");

}

BOOST_AUTO_TEST_CASE(intarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-TGC", 11), 11);
    BOOST_CHECK_EQUAL(GetArg("-TGC", 0), 0);

    ResetArgs("-TGC -bar");
    BOOST_CHECK_EQUAL(GetArg("-TGC", 11), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);

    ResetArgs("-TGC=11 -bar=12");
    BOOST_CHECK_EQUAL(GetArg("-TGC", 0), 11);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 12);

    ResetArgs("-TGC=NaN -bar=NotANumber");
    BOOST_CHECK_EQUAL(GetArg("-TGC", 1), 0);
    BOOST_CHECK_EQUAL(GetArg("-bar", 11), 0);
}

BOOST_AUTO_TEST_CASE(doubledash)
{
    ResetArgs("--TGC");
    BOOST_CHECK_EQUAL(GetBoolArg("-TGC"), true);

    ResetArgs("--TGC=verbose --bar=1");
    BOOST_CHECK_EQUAL(GetArg("-TGC", ""), "verbose");
    BOOST_CHECK_EQUAL(GetArg("-bar", 0), 1);
}

BOOST_AUTO_TEST_CASE(boolargno)
{
    ResetArgs("-noTGC");
    BOOST_CHECK(!GetBoolArg("-TGC"));
    BOOST_CHECK(!GetBoolArg("-TGC", true));
    BOOST_CHECK(!GetBoolArg("-TGC", false));

    ResetArgs("-noTGC=1");
    BOOST_CHECK(!GetBoolArg("-TGC"));
    BOOST_CHECK(!GetBoolArg("-TGC", true));
    BOOST_CHECK(!GetBoolArg("-TGC", false));

    ResetArgs("-noTGC=0");
    BOOST_CHECK(GetBoolArg("-TGC"));
    BOOST_CHECK(GetBoolArg("-TGC", true));
    BOOST_CHECK(GetBoolArg("-TGC", false));

    ResetArgs("-TGC --noTGC");
    BOOST_CHECK(GetBoolArg("-TGC"));

    ResetArgs("-noTGC -TGC"); // TGC always wins:
    BOOST_CHECK(GetBoolArg("-TGC"));
}

BOOST_AUTO_TEST_SUITE_END()
