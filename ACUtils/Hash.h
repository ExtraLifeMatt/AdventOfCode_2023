#pragma once

#include <cstdint>
#include <functional>

#define ENABLE_STL_HASH(x, func) \
namespace std \
{\
	template<>struct hash<x>\
	{\
		std::size_t operator()(x const& val) const \
		{ \
			return val.##func##();\
		}\
	};\
}\

namespace Hash
{
	// https://stackoverflow.com/questions/664014/what-integer-hash-function-are-good-that-accepts-an-integer-hash-key
	constexpr uint32_t Hash32(uint32_t x)
	{
		x = ((x >> 16) ^ x) * 0x45d9f3b;
		x = ((x >> 16) ^ x) * 0x45d9f3b;
		x = (x >> 16) ^ x;
		return x;
	}

	constexpr uint64_t Hash64(uint64_t x)
	{
		x = (x ^ (x >> 30)) * UINT64_C(0xbf58476d1ce4e5b9);
		x = (x ^ (x >> 27)) * UINT64_C(0x94d049bb133111eb);
		x = x ^ (x >> 31);
		return x;
	}

	// Seed is the output value
	// https://stackoverflow.com/questions/2590677/how-do-i-combine-hash-values-in-c0x
	template <typename T, typename... Rest>
	inline void HashCombine(std::size_t& seed, T const& v, Rest &&... rest) {
		std::hash<T> hasher;
		seed ^= (hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2));
		int i[] = { 0, (HashCombine(seed, std::forward<Rest>(rest)), 0)... };
		(void)(i);
	}

	// Simple version that uses uint64's for our hashes.
	inline uint64_t HashCombineU64(uint64_t seed, uint64_t const& v) {
		return seed ^ (v + 0x9e3779b9 + (seed << 6) + (seed >> 2));
	}

	inline size_t HashCombineSize(size_t seed, size_t const& v) {
		return seed ^ (v + 0x9e3779b9 + (seed << 6) + (seed >> 2));
	}

	namespace FNV1A
	{
		constexpr uint32_t val_32_const = 0x811c9dc5;
		constexpr uint32_t prime_32_const = 0x1000193;
		constexpr uint64_t val_64_const = 0xcbf29ce484222325;
		constexpr uint64_t prime_64_const = 0x100000001b3;

		inline constexpr uint32_t hash_32_fnv1a_const(const char* str, int32_t len, const uint32_t value = val_32_const)
		{
			return (len == 0 || *str == '\0') ? value
				: hash_32_fnv1a_const(str + 1, len - 1, (value ^ uint32_t(*str)) * prime_32_const);
		}

		inline constexpr uint64_t hash_64_fnv1a_const(const char* str, int32_t len, const uint64_t value = val_64_const)
		{
			return (len == 0 || *str == '\0') ? value
				: hash_64_fnv1a_const(str + 1, len - 1, (value ^ uint64_t(*str)) * prime_64_const);
		}

		template<size_t MAX_STRING_LENGTH>
		inline constexpr uint32_t string_compile_hash_fnv1a32(const char* str)
		{
			return hash_32_fnv1a_const(str, MAX_STRING_LENGTH);
		}

		template<size_t MAX_STRING_LENGTH>
		inline constexpr uint64_t string_compile_hash_fnv1a64(const char* str)
		{
			return hash_64_fnv1a_const(str, MAX_STRING_LENGTH);
		}
	}

	constexpr uint32_t DEFAULT_MAX_HASH_STRING_LENGTH = 64;

	// Uses the "DEFAULT_MAX_HASH_STRING_LENGTH" for the maximum name length. If you need a longer (or shorter) max length, you can just directly call the methods in the Hash namespace.
	constexpr uint32_t HashString32(const char* string)
	{
		return FNV1A::string_compile_hash_fnv1a32< DEFAULT_MAX_HASH_STRING_LENGTH >(string);
	}

	// Uses the "DEFAULT_MAX_HASH_STRING_LENGTH" for the maximum name length. If you need a longer (or shorter) max length, you can just directly call the methods in the Hash namespace.
	constexpr uint64_t HashString64(const char* string)
	{
		return FNV1A::string_compile_hash_fnv1a64< DEFAULT_MAX_HASH_STRING_LENGTH >(string);
	}
}