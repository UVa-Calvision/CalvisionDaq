#pragma once

#include <string>

/*
 * Naming conventions for tree branches, filenames, etc.
 */

template <typename T>
std::string to_string_helper(T t) {
    return std::to_string(t);
}

template <> inline
std::string to_string_helper<const char*>(const char* c) {
    return std::string(c);
}

template <typename Arg>
std::string canonical_name(const Arg& arg) {
    return to_string_helper(arg);
}

template <typename Arg, typename Next, typename... Args>
std::string canonical_name(const Arg& a, const Next& next, const Args&... args) {
    return canonical_name(a) + "_" + canonical_name(next, args...);
}

inline
std::string name_timestamp(int g) {
    return canonical_name("timestamp", g);
}

inline
std::string name_time(int g) {
    return canonical_name("sample_time", g);
}

inline
std::string name_trigger(int g) {
    return canonical_name("trigger", g);
}

inline
std::string name_channel(int g, int c) {
    return canonical_name("channel", g, c);
}
