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
	static_assert(std::is_same_v<std::tuple<int>, decltype(e)::data_type>, "matcher data type mismatch");
}

TEST(dispatch, matcher_expr_word) {
	auto e = match(word());
	ASSERT_EQ("^([[:w:]]+)$", e.regex());
	static_assert(std::is_same_v<std::tuple<std::string>, decltype(e)::data_type>, "matcher data type mismatch");
}


TEST(dispatch, matcher_expr_compose_simple) {
	auto e = match("/foo/") + integer();
	ASSERT_EQ("^/foo/([0-9]+)$", e.regex());
}

TEST(dispatch, matcher_expr_compose_advanced) {
	auto e = match("/foo/") + word() + "/bar/" + integer();
	ASSERT_EQ("^/foo/([[:w:]]+)/bar/([0-9]+)$", e.regex());
	static_assert(std::is_same_v<std::tuple<std::string, int>, decltype(e)::data_type>, "matcher data type mismatch");
}

TEST(dispatch, matcher_tuple) {
	matcher<std::string, int> m("^/foo/([[:w:]]+)/bar/([0-9]+)$");
	auto p = m.match_tuple("/foo/john_doe/bar/6789");
	ASSERT_TRUE(p.first);
	ASSERT_EQ("john_doe", std::get<0>(p.second));
	ASSERT_EQ(6789, std::get<1>(p.second));
}

TEST(dispatch, make_matcher_string) {
	auto m = make_matcher(match("/foo/"));
	static_assert(std::is_same_v<std::tuple<>, decltype(m)::data_type>, "matcher data type mismatch");
	ASSERT_TRUE(m.match("/foo/"));
	ASSERT_FALSE(m.match("/bar/"));
}

TEST(dispatch, make_matcher_composed) {
	auto m = make_matcher(match("/foo/") + word() + "/bar/" + integer());
	ASSERT_TRUE(m.match("/foo/john_doe/bar/6789"));
	ASSERT_FALSE(m.match("/bar/"));
	ASSERT_FALSE(m.match("/foo/foo/bar/bar"));
}

TEST(dispatch, make_matcher_tuple) {
	auto m = make_matcher(match("/foo/") + word() + "/bar/" + integer());
	auto p = m.match_tuple("/foo/john_doe/bar/6789");
	ASSERT_TRUE(p.first);
	ASSERT_EQ("john_doe", std::get<0>(p.second));
	ASSERT_EQ(6789, std::get<1>(p.second));
}

TEST(dispatch, rule) {
	rules::rule<std::string, int> r(match("/foo/") + word() + "/bar/" + integer(), [](auto s, auto i) {
		ASSERT_EQ("john_doe", s);
		ASSERT_EQ(6789, i);
	});
	;
	ASSERT_TRUE(r.dispatch("/foo/john_doe/bar/6789"));
	ASSERT_FALSE(r.dispatch("/foo/foo/bar/bar"));
}

TEST(dispatch, rule_as_function) {
	rules::rule<std::string, int> r(match("/foo/") + word() + "/bar/" + integer(), [](auto s, auto i) {
		ASSERT_EQ("john_doe", s);
		ASSERT_EQ(6789, i);
	});
	;
	ASSERT_TRUE(r("/foo/john_doe/bar/6789"));
	ASSERT_FALSE(r("/foo/foo/bar/bar"));
}

TEST(dispatch, make_rule) {
	auto r = rules::make_rule(match("/foo/") + word() + "/bar/" + integer(), [](auto s, auto i) {
		ASSERT_EQ("john_doe", s);
		ASSERT_EQ(6789, i);
	});
	ASSERT_TRUE(r.dispatch("/foo/john_doe/bar/6789"));
	ASSERT_FALSE(r.dispatch("/foo/foo/bar/bar"));
}

TEST(dispatch, dispatch_rule) {
	dispatch_rule dr(match("/foo/") + word() + "/bar/" + integer(), [](auto s, auto i) {
		ASSERT_EQ("john_doe", s);
		ASSERT_EQ(6789, i);
	});
	ASSERT_TRUE(dr.dispatch("/foo/john_doe/bar/6789"));
	ASSERT_FALSE(dr.dispatch("/foo/foo/bar/bar"));
}

TEST(dispatch, dispatcher) {
	dispatcher d;
	d.add(match("/foo/") + word() + "/bar/" + integer(), [](auto s, auto i) {
		ASSERT_EQ("john_doe", s);
		ASSERT_EQ(6789, i);
	});
	d.add(match("/bar/") + integer() + "/foo/" + word(), [](auto i, auto s) {
		ASSERT_EQ(1234, i);
		ASSERT_EQ("jane_smith", s);
	});
	ASSERT_TRUE(d.dispatch("/foo/john_doe/bar/6789"));
	ASSERT_FALSE(d.dispatch("/foo/foo/bar/bar"));
	ASSERT_TRUE(d.dispatch("/bar/1234/foo/jane_smith"));
}

TEST(dispatch, dispatcher_as_function) {
	dispatcher d;
	d.add(match("/foo/") + word() + "/bar/" + integer(), [](auto s, auto i) {
		ASSERT_EQ("john_doe", s);
		ASSERT_EQ(6789, i);
	});
	d.add(match("/bar/") + integer() + "/foo/" + word(), [](auto i, auto s) {
		ASSERT_EQ(1234, i);
		ASSERT_EQ("jane_smith", s);
	});
	ASSERT_TRUE(d("/foo/john_doe/bar/6789"));
	ASSERT_FALSE(d("/foo/foo/bar/bar"));
	ASSERT_TRUE(d("/bar/1234/foo/jane_smith"));
}