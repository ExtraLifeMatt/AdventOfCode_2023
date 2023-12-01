#pragma once

#include <string>
#include <vector>

// String Manipulation
namespace StringUtil
{
	// Parse int64/uint64 values from a string
	uint64_t AtoiU64(const char* str);
	int64_t AtoiI64(const char* str);

	// Trims whitespace from the front and ends of a string.
	std::string Trim(const std::string& str);

	// Splits the provided string into a vector of substrings based on the delimiters. Optionally trims whitespace from entries. 
	void SplitBy(const std::string& str, const char* delimiters, std::vector<std::string>& outValues, bool trim = true);
}
