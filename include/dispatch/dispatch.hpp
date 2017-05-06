#ifndef DISPATCH_HPP
#define DISPATCH_HPP

#include <functional>
#include <regex>
#include <string>
#include <sstream>
#include <tuple>
#include <type_traits>
#include <utility>

// Glue
#if defined(__clang__) && __clang_major__ < 4 && __clang_minor__ < 9
namespace std {
	template< class T >
	constexpr std::size_t tuple_size_v = tuple_size<T>::value;
}
#endif

namespace dispatch {

// Tools: tuple_for_each, apply..;
namespace details {
template <class F, class Tuple, std::size_t... I>
constexpr decltype(auto) apply_impl(F &&f, Tuple &&t, std::index_sequence<I...>) {
	return std::invoke(std::forward<F>(f), std::get<I>(std::forward<Tuple>(t))...);
}
} 

template <class F, class Tuple>
constexpr decltype(auto) apply(F &&f, Tuple &&t) {
	return details::apply_impl(
		std::forward<F>(f), std::forward<Tuple>(t),
		std::make_index_sequence<std::tuple_size_v<std::decay_t<Tuple>>>{});
}

template <typename Tuple, typename Fn, std::size_t index>
inline std::enable_if_t<index == std::tuple_size_v<Tuple>> tuple_for_each(Tuple&, Fn&&) {}

template <typename Tuple, typename Fn, std::size_t index = 0>
inline std::enable_if_t<index < std::tuple_size_v<Tuple>> tuple_for_each(Tuple& t, Fn&& f) {
	f(std::get<index>(t));
	tuple_for_each<Tuple, Fn, index + 1>(t, std::forward<Fn>(f));
}

namespace matchers {

struct integer {};
struct word {};

inline std::ostream& operator<<(std::ostream& o, integer) {
	return (o << "([0-9]+)");
}

inline std::ostream& operator<<(std::ostream& o, word) {
	return (o << "([[:w:]]+)");
}

template <typename T>
inline std::string to_regex(const T& v) {
	std::ostringstream oss;
	oss << v;
	return oss.str();
}

template <typename T>
struct traits {
	using data_type = T;
};

template <>
struct traits<integer> {
	using data_type = int;
};

template <>
struct traits<word> {
	using data_type = std::string;
};

template <typename ... Types>
class matcher_expr {
public:
	using data_type = std::tuple<typename traits<Types>::data_type...>;

	matcher_expr(const std::string& regex, Types... ts)
		: matcher_expr(regex, std::make_tuple(ts...)) { }

	matcher_expr(const std::string& regex, const std::tuple<Types...>&  t)
		: m_regex(regex)
		, m_components(t) {	}

	template <typename U>
	matcher_expr(const U& u)
		: m_regex(to_regex(u))
		, m_components(std::make_tuple(u)) { }

	std::string regex() const {
		std::ostringstream oss;
		oss << '^' << m_regex << '$';
		return oss.str();
	}

	template <typename U>
	auto combine(const U& u) const {
		std::ostringstream oss;
		oss << m_regex << to_regex(u);
		return matcher_expr<Types..., U>(oss.str(), std::tuple_cat(m_components, std::make_tuple(u)));
	}

	matcher_expr combine(const std::string& s) const {
		std::ostringstream oss;
		oss << m_regex << s;
		return matcher_expr(oss.str(), m_components);
	}

private:
	std::string m_regex;
	std::tuple<Types...> m_components;
};

template <typename U, typename ... Types>
inline auto operator+(const matcher_expr<Types...>& m, const U& o) {
	return m.combine(o);
}

template <typename ... Types>
inline auto operator+(const matcher_expr<Types...>& m, const char* s) {
	return m.combine(std::string(s));
}

}

template <typename T>
auto match(const T& base) {
	return matchers::matcher_expr<T>(base);
}

template <>
inline auto match<std::string>(const std::string& base) {
	return matchers::matcher_expr<>(base);
}

inline auto match(const char* base) {
	return match<std::string>(base);
}

template <typename ... Types>
class matcher {
public:
	using data_type = std::tuple<Types...>;

	matcher(const std::string& regex)
		: m_regex(regex)  { }

	bool match(const std::string& s) const {
		return std::regex_match(s, m_regex);
	}

	auto match_tuple(const std::string& s) const {
		std::tuple<Types...> t;
		std::smatch match;
		if (std::regex_match(s, match, m_regex)) {
			auto it = ++match.begin();
			tuple_for_each(t, [&it](auto& v) {
				std::istringstream iss(*it++);
				iss >> v;
			});
		}
		return t;
	}
		
private:
	std::regex m_regex;
};

template <typename ... Types>
inline auto make_matcher(matchers::matcher_expr<Types...>&& m) {
	return matcher<typename matchers::traits<Types>::data_type...>(m.regex());
}

}

#endif // !DISPATCH_HPP
