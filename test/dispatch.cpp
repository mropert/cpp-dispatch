#include <dispatch/dispatch.hpp>

#include <gtest/gtest.h>

using namespace dispatch;
using dispatch::matchers::integer;
using dispatch::matchers::word;

TEST(dispatch, matcher_expr_string) {
	auto e = match("/foo");
	ASSERT_EQ("^/foo$", e.regex());
}

TEST(dispatch, matcher_expr_int) {
	auto e = match(integer());
	ASSERT_EQ("^([0-9]+)$", e.regex());
}

TEST(dispatch, matcher_expr_word) {
	auto e = match(word());
	ASSERT_EQ("^([[:w:]]+)$", e.regex());
}


TEST(dispatch, matcher_expr_compose_simple) {
	auto e = match("/foo/") + integer();
	ASSERT_EQ("^/foo/([0-9]+)$", e.regex());
}

TEST(dispatch, matcher_expr_compose_advanced) {
	auto e = match("/foo/") + word() + "/bar/" + integer();
	ASSERT_EQ("^/foo/([[:w:]]+)/bar/([0-9]+)$", e.regex());
}

TEST(dispatch, matcher_tuple) {
	matcher<std::string, int> m("^/foo/([[:w:]]+)/bar/([0-9]+)$");
	auto t = m.match_tuple("/foo/john_doe/bar/6789");
	ASSERT_EQ("john_doe", std::get<0>(t));
	ASSERT_EQ(6789, std::get<1>(t));
}

TEST(dispatch, make_matcher_string) {
	auto m = make_matcher(match("/foo/"));
	ASSERT_TRUE(m.match("/foo/"));
	ASSERT_FALSE(m.match("/bar/"));
}

TEST(dispatch, make_matcher_composed) {
	auto m = make_matcher(match("/foo/") + word() + "/bar/" + integer());
	ASSERT_TRUE(m.match("/foo/john_doe/bar/6789"));
	ASSERT_FALSE(m.match("/bar/"));
	ASSERT_FALSE(m.match("/foo/foo/bar/bar"));
}

