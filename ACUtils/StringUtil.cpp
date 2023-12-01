#include "StringUtil.h"

uint64_t StringUtil::AtoiU64(const char* str)
{
	return std::stoull(str);
}

int64_t StringUtil::AtoiI64(const char* str)
{
	return std::stoll(str);
}

std::string StringUtil::Trim(const std::string& str)
{
	if (str.length() == 0)
	{
		return str;
	}

	size_t whitespaceStart = str.find_first_not_of(' ');
	size_t whitespaceEnd = str.find_last_not_of(' ');

	if (whitespaceEnd - whitespaceStart != str.length() - 1)
	{
		if (whitespaceEnd == str.length() - 1)
		{
			return str.substr(whitespaceStart);
		}

		return str.substr(whitespaceStart, whitespaceEnd - whitespaceStart + 1);
	}

	return str;
}

void StringUtil::SplitBy(const std::string& str, const char* delimiters, std::vector<std::string>& outValues, bool trim)
{
	outValues.clear();
	std::vector<std::string> delims;

	size_t lastDelimEnd = 0;
	for (size_t i = 0; i < strlen(delimiters); ++i)
	{
		if (delimiters[i] == '|')
		{
			delims.push_back(std::string(delimiters + lastDelimEnd, i - lastDelimEnd));
			lastDelimEnd = i + 1;
		}
	}
	delims.push_back(std::string(delimiters + lastDelimEnd, strlen(delimiters) - lastDelimEnd));

	size_t offset = 0;
	size_t delimLen = 0;

	while (offset != str.length())
	{
		size_t foundIndex = std::string::npos;
		for (const std::string& del : delims)
		{
			size_t d = str.find(del.c_str(), offset);
			if (d != std::string::npos && d < foundIndex)
			{
				foundIndex = std::min(d, foundIndex);
				if (foundIndex == d)
				{
					delimLen = del.length();
				}
			}
		}

		if (foundIndex != std::string::npos)
		{
			std::string ss = str.substr(offset, foundIndex - offset);
			if (trim)
			{
				outValues.push_back(Trim(ss));
			}
			else
			{
				outValues.push_back(ss);
			}

			offset = foundIndex + delimLen;
		}
		else
		{
			if (offset != str.length())
			{
				std::string ss = str.substr(offset);
				if (trim)
				{
					outValues.push_back(Trim(ss));
				}
				else
				{
					outValues.push_back(ss);
				}
			}
			offset = str.length();
		}
	}
}