#pragma once

#include <cstdint>
#include <type_traits>

#define DECLARE_ENUM_OPERATORS(x, minValue, maxValue) \
inline x operator++(const x& value, int) { return value == maxValue ? minValue : static_cast<x>(static_cast<std::underlying_type<x>::type>(value) + 1); } \
inline x operator--(const x& value, int) { return value == minValue ? maxValue : static_cast<x>(static_cast<std::underlying_type<x>::type>(value) - 1); } \
inline x& operator++(x& value) { value = (value == maxValue ? minValue : static_cast<x>(static_cast<std::underlying_type<x>::type>(value) + 1)); return value; } \
inline x& operator--(x& value) { value = (value == minValue ? maxValue : static_cast<x>(static_cast<std::underlying_type<x>::type>(value) - 1)); return value; } \

#define DECLARE_ENUM_BITFIELD_OPERATORS(x) \
inline x operator|(const x& a, const x& b) { return (x)(static_cast<std::underlying_type<x>::type>(a) | static_cast<std::underlying_type<x>::type>(b)); } \
inline x operator&(const x& a, const x& b) { return (x)(static_cast<std::underlying_type<x>::type>(a) & static_cast<std::underlying_type<x>::type>(b)); } \
inline x operator^(const x& a, const x& b) { return (x)(static_cast<std::underlying_type<x>::type>(a) ^ static_cast<std::underlying_type<x>::type>(b)); } \
inline x operator~(const x& a) {  return (x)(~static_cast<std::underlying_type<x>::type>(a)); } \
inline x& operator|=(x& a, const x& b) { a = (x)(static_cast<std::underlying_type<x>::type>(a) | static_cast<std::underlying_type<x>::type>(b)); return a; } \
inline x& operator&=(x& a, const x& b) { a = (x)(static_cast<std::underlying_type<x>::type>(a) & static_cast<std::underlying_type<x>::type>(b)); return a; } \
inline x& operator^=(x& a, const x& b) { a = (x)(static_cast<std::underlying_type<x>::type>(a) ^ static_cast<std::underlying_type<x>::type>(b)); return a; } 
