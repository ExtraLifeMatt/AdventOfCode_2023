#pragma once

#include <fstream>
#include <vector>
#include <string.h>

#include "StringUtil.h"

class FileStreamReader
{
public:
	FileStreamReader(const char* filename)
	{
		m_fileStream.open(filename, std::ifstream::in);
	}

	~FileStreamReader()
	{
		if (m_fileStream.is_open())
		{
			m_fileStream.close();
		}
	}

	bool IsOpened() const { return m_fileStream.is_open(); }
	bool IsEOF() const { return m_fileStream.eof(); }

	void SeekRelative(size_t Pos) { m_fileStream.seekg(Pos, std::ios_base::cur); }
	void SeekAbsolute(size_t Pos) { m_fileStream.seekg(Pos, std::ios_base::beg); }
	size_t GetCurrentPos() { return m_fileStream.tellg(); }

	char GetChar() { char out; m_fileStream.get(out); return out; }

	std::string ReadLine(bool trim = true)
	{
		std::string line;
		std::getline(m_fileStream, line);

		if (trim)
		{
			return StringUtil::Trim(line);
		}

		return line;
	}

	void ReadAllLines(bool trim, std::vector<std::string>& outValues)
	{
		outValues.clear();
		while (!IsEOF())
		{
			outValues.push_back(ReadLine(trim));
		}
	}

	std::string ReadUntil(char delim, bool stopAtNewline = true)
	{
		std::string tmp;
		while (!IsEOF())
		{
			char ch = GetChar();
			if (ch == delim || (ch == '\\n' && stopAtNewline))
			{
				break;
			}

			tmp.push_back(ch);
		}

		return tmp;
	}

	void ReadLineSplitBy(bool trim, const char* delimiters, std::vector<std::string>& outValues)
	{
		std::string line = ReadLine(trim);
		StringUtil::SplitBy(line, delimiters, outValues);
	}

	void ReadAllLinesSplitBy(bool trim, const char* delimiters, std::vector<std::string>& outValues)
	{
		outValues.clear();
		std::vector<std::string> temp;
		while (!IsEOF())
		{
			temp.clear();
			ReadLineSplitBy(trim, delimiters, temp);
			outValues.insert(outValues.end(), temp.begin(), temp.end());
		}
	}

private:
	std::ifstream m_fileStream;
};
