// main.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "AdventGUI/AdventGUI.h"

#include "ACUtils/StringUtil.h"
#include <vector>
#include <cinttypes>

class AdventDay : public AdventGUIInstance
{
public:
	AdventDay(const AdventGUIParams& params) 
	: AdventGUIInstance(params)
	{};
private:
	virtual void ParseInput(FileStreamReader& fileReader) override
	{
		// Parse Input. Input never changes between parts of a problem.
		std::string line;
		std::vector<std::string> tokens;
		while (!fileReader.IsEOF())
		{
			line = fileReader.ReadLine();
			StringUtil::SplitBy(line, ",", tokens);
			m_Entries.insert(m_Entries.end(), tokens.begin(), tokens.end());
		}
	}

	static uint32_t Hash(const std::string& str, bool labelOnly = false)
	{
		uint32_t outHash = 0;
		for (char c : str)
		{
			outHash += (uint32_t)c;
			outHash *= 17;
			outHash %= 256;
		}
		return outHash;
	}

	virtual void PartOne(const AdventGUIContext& context) override
	{
		// Part One
		assert(Hash("HASH") == 52);

		uint32_t totalValue = 0;
		for (const std::string& entry : m_Entries)
		{
			totalValue += Hash(entry);
		}

		Log("Total = %u", totalValue);

		// Done.
		AdventGUIInstance::PartOne(context);
	}

	struct LensBox
	{
		LensBox(): label{0}, focalLength(0) {}
		LensBox(const char* _label): label{0}, focalLength(0) { strcpy_s(label, 16, _label); }
		char label[16];
		uint32_t focalLength;
		bool operator ==(const LensBox& RHS) const { return _stricmp(label, RHS.label) == 0; }
		bool operator !=(const LensBox& RHS) const { return _stricmp(label, RHS.label) != 0; }
	};

	virtual void PartTwo(const AdventGUIContext& context) override
	{	
		// Part Two
		uint64_t totalValue = 0;
		uint32_t labelHash = 0;
		std::string subString;
		for (const std::string& entry : m_Entries)
		{
			if (entry.back() == '-')
			{
				subString = entry.substr(0, entry.size() - 1);
				
				labelHash = Hash(subString.c_str());
				LensBox newLens(subString.c_str());

				LensBoxVector& box = m_HashMap[labelHash % 256];
				LensBoxVector::const_iterator itFind = std::find(box.begin(), box.end(), newLens);
				if (itFind != box.end())
				{
					box.erase(itFind);
				}
			}
			else if (entry[entry.size() - 2] == '=')
			{
				subString = entry.substr(0, entry.size() - 2);

				labelHash = Hash(subString.c_str());
				LensBox newLens(subString.c_str());
				newLens.focalLength = entry[entry.size() - 1] - '0';

				LensBoxVector& box = m_HashMap[labelHash % 256];
				LensBoxVector::iterator itFind = std::find(box.begin(), box.end(), newLens);
				if (itFind != box.end())
				{
					itFind->focalLength = newLens.focalLength;
				}
				else
				{
					box.push_back(newLens);
				}
			}
			else
			{
				assert(false);
			}
		}

		for (uint32_t i = 0; i < 256; ++i)
		{
			uint64_t boxValue = 0;
			for (size_t j = 0; j < m_HashMap[i].size(); ++j)
			{
				boxValue += (i + 1) * (j + 1) * m_HashMap[i][j].focalLength;
			}
			totalValue += boxValue;
		}

		Log("Total Value in Hashmap: %llu", totalValue);

		// Done.
		AdventGUIInstance::PartTwo(context);
	}

	std::vector<std::string> m_Entries;
	typedef std::vector<LensBox> LensBoxVector;
	LensBoxVector m_HashMap[256];
};

int main()
{
	AdventGUIParams newParams;
	newParams.day = 15;
	newParams.year = 2023;
	newParams.puzzleTitle = "Lens Library";
	newParams.inputFilename = "input.txt";

	AdventGUIInstance::InstantiateAndExecute<AdventDay>(newParams);

	return 0;
}
