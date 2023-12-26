// main.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "AdventGUI/AdventGUI.h"

#include "ACUtils/Algorithm.h"
#include "ACUtils/Hash.h"
#include "ACUtils/AABB.h"
#include "ACUtils/StringUtil.h"
#include <vector>
#include <queue>
#include <unordered_set>

struct Brick
{
	Brick(const std::string& _name, const IntVec3& _min, const IntVec3& _max): Name(_name), Min(_min), Max(_max){}

	bool Intersects(const Brick& RHS) const
	{
		return Min.AllLessThanOrEqual(RHS.Max) && Max.AllGreaterThanOrEqual(RHS.Min);
	}

	bool Touches(const Brick& RHS) const
	{
		IntAABB2D LHSBox(Min.XY(), Max.XY());
		IntAABB2D RHSBox(RHS.Min.XY(), RHS.Max.XY());

		if (LHSBox.Intersects(RHSBox) || LHSBox.Contains(RHSBox.GetMin()))
		{
			return abs(Max.z - RHS.Min.z) == 1;
		}

		return false;
	}

	void Drop(int32_t dist)
	{
		Min.z -= dist;
		Max.z -= dist;
	}

	bool operator==(const Brick& RHS) const
	{
		return _stricmp(Name.c_str(), RHS.Name.c_str()) == 0 && Min == RHS.Min && Max == RHS.Max;
	}

	std::string Name;
	IntVec3 Min;
	IntVec3 Max;
	std::vector<int32_t> Supports;
	std::vector<int32_t> SupportedBy;
};

void ToBase26(size_t value, std::string& outString)
{
	if (value >= 26)
	{
		ToBase26(value / 26 - 1, outString);
	}

	outString += ('A' + (char)(value % 26));
}

class AdventDay : public AdventGUIInstance
{
public:
	AdventDay(const AdventGUIParams& params)
		: AdventGUIInstance(params)
	{};
private:
	struct FindCollidingBricks
	{
		FindCollidingBricks(const Brick& _brick) : brick(_brick)
		{
			brick.Min = brick.Min - IntVec3(0, 0, 1);
			brick.Max = brick.Max + IntVec3(0, 0, 1);
		}

		bool operator()(const Brick& RHS) const
		{
			if (_stricmp(brick.Name.c_str(), RHS.Name.c_str()) == 0)
			{
				return false;
			}

			return brick.Intersects(RHS);
		}

		Brick brick;
	};

	virtual void ParseInput(FileStreamReader& fileReader) override
	{
		// Parse Input. Input never changes between parts of a problem.
		std::string line;
		std::vector<std::string> tokens;

		IntVec3 left;
		IntVec3 right;
		std::string name;

		int32_t maxZ = 0;
		while (!fileReader.IsEOF())
		{
			line = fileReader.ReadLine();
			size_t tildeIdx = line.find("~");
			assert(tildeIdx != std::string::npos);
			std::string vecA = line.substr(0, tildeIdx);
			std::string vecB = line.substr(tildeIdx + 1);

			StringUtil::SplitBy(vecA, ",", tokens);
			assert(tokens.size() == 3);
			left = IntVec3(atoi(tokens[0].c_str()), atoi(tokens[1].c_str()), atoi(tokens[2].c_str()));

			StringUtil::SplitBy(vecB, ",", tokens);
			assert(tokens.size() == 3);
			right = IntVec3(atoi(tokens[0].c_str()), atoi(tokens[1].c_str()), atoi(tokens[2].c_str()));

			assert(left.AllLessThanOrEqual(right));

			maxZ = std::max(maxZ, right.z);

			name.clear();
			size_t totalExisting = m_Bricks.size();

			ToBase26(m_Bricks.size(), name);

			m_Bricks.emplace_back(name, left, right);
		}

		// Sort by Z
		std::sort(m_Bricks.begin(), m_Bricks.end(), [](const Brick& LHS, const Brick& RHS) { return LHS.Min.z < RHS.Min.z; });

		// Fall blocks to stable positions.
		bool foundCollision = false;
		m_Bricks[0].Drop(m_Bricks[0].Min.z - 1); // Move to the ground.
		for (size_t i = 1; i < m_Bricks.size(); ++i)
		{
			const Brick& fallingBrick = m_Bricks[i];

			foundCollision = false;
			int32_t closestDistance = INT_MAX;
			for (size_t j = i; j > 0; --j)
			{
				// Move the block to our top.
				const Brick& brickAtRest = m_Bricks[j - 1];

				int32_t dist = fallingBrick.Min.z - brickAtRest.Max.z;
				Brick colTest = fallingBrick;
				colTest.Drop(dist);

				if (brickAtRest.Intersects(colTest))
				{
					foundCollision = true;
					closestDistance = std::min(closestDistance, dist - 1);
				}
			}

			if (!foundCollision)
			{
				m_Bricks[i].Drop(m_Bricks[i].Min.z - 1); // Move to the ground.
			}
			else
			{
				m_Bricks[i] = fallingBrick;
				m_Bricks[i].Drop(closestDistance);
			}
		}

		// Do one more pass to establish supports/supported by dependencies.
		for (size_t i = 0; i < m_Bricks.size(); ++i)
		{
			Brick& currentBrick = m_Bricks[i];

			std::vector<int> allColliders = Algorithm::find_all_indices(m_Bricks.begin(), m_Bricks.end(), FindCollidingBricks(currentBrick));
			for (int32_t colliderIdx : allColliders)
			{
				if (m_Bricks[colliderIdx].Min.z >= currentBrick.Max.z)
				{
					currentBrick.Supports.push_back(colliderIdx);
				}
				else
				{
					currentBrick.SupportedBy.push_back(colliderIdx);
				}
			}
		}

	}

	virtual void PartOne(const AdventGUIContext& context) override
	{
		// Part One

		// Determine who can be nuked
		size_t totalToSafelyDestroy = 0;
		for (const Brick& brick : m_Bricks)
		{
			bool valid = true;

			// Only nuke those who have more than one support.
			for (int32_t supportIdx : brick.Supports)
			{
				valid &= m_Bricks[supportIdx].SupportedBy.size() != 1;
			}

			if (valid)
			{
				++totalToSafelyDestroy;
			}
		}

		Log("Total: %zd", totalToSafelyDestroy);

		// Done.
		AdventGUIInstance::PartOne(context);
	}

	virtual void PartTwo(const AdventGUIContext& context) override
	{	
		// Part Two
		std::queue<const Brick*> fallQueue;
		size_t totalThatWouldFall = 0;
		for (const Brick& brick : m_Bricks)
		{
			fallQueue.push(&brick);
			std::unordered_set<const Brick*> fallingSet;
			while (!fallQueue.empty())
			{
				const Brick* fallingBrick = fallQueue.front();
				fallQueue.pop();

				fallingSet.insert(fallingBrick);

				// Check all our supports, if they have any supports that AREN'T falling yet, check again later. 
				for (int32_t supportIdx : fallingBrick->Supports)
				{
					bool canFall = true;
					
					for (int32_t supportedByIdx : m_Bricks[supportIdx].SupportedBy)
					{
						canFall &= (fallingSet.find(&m_Bricks[supportedByIdx]) != fallingSet.end());
					}
					
					if (canFall)
					{
						fallQueue.push(&m_Bricks[supportIdx]);
						++totalThatWouldFall;
					}
				}
			}

		}

		Log("Total: %zd", totalThatWouldFall);

		// Done.
		AdventGUIInstance::PartTwo(context);
	}
	std::vector<Brick> m_Bricks;
};

int main()
{
	AdventGUIParams newParams;
	newParams.day = 22;
	newParams.year = 2023;
	newParams.puzzleTitle = "Step Counter";
	newParams.inputFilename = "input.txt";

	AdventGUIInstance::InstantiateAndExecute<AdventDay>(newParams);

	return 0;
}
