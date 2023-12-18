// main.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "AdventGUI/AdventGUI.h"

#include "ACUtils/Algorithm.h"
#include "ACUtils/AStar.h"
#include "ACUtils/Hash.h"
#include "ACUtils/IntVec.h"
#include "ACUtils/StringUtil.h"


class CrucibleGridNode : public AStar::AStarNodeBase<uint32_t>
{
public:
	CrucibleGridNode(const IntVec2& pos, const IntVec2& dir, const IntVec2& goal, uint32_t totalHeatLoss, uint32_t timeSinceLastTurn, uint32_t minTimeForTurn, uint32_t maxTimeForTurn)
	:m_hash(0),
	m_pos(pos), 
	m_goalPos(goal), 
	m_dir(dir),
	m_timeSinceLastTurn(timeSinceLastTurn),
	m_totalHeatLoss(totalHeatLoss),
	m_minTimeForTurn(minTimeForTurn),
	m_maxTimeForTurn(maxTimeForTurn)
	{
		m_hash |= pos.x << 0;
		m_hash |= pos.y << 8;
		
		if (m_dir.x != 0)
		{
			m_hash |= 1 << (m_dir.x > 0 ? 16 : 17); 
		}

		if (m_dir.y != 0)
		{
			m_hash |= 1 << (m_dir.y > 0 ? 18 : 19);
		}

		m_hash |= m_timeSinceLastTurn << 20;
	}

	virtual size_t GetHash() const override { return m_hash; }
	virtual bool IsAtGoal() const override { return m_pos == m_goalPos && m_timeSinceLastTurn >= m_minTimeForTurn; }
	const IntVec2& GetPos() const { return m_pos; }
	const IntVec2& GetDir() const { return m_dir; }
	uint32_t GetTimeSinceLastTurn() const { return m_timeSinceLastTurn; }
	uint32_t GetTotalHeatLoss() const { return m_totalHeatLoss; }
	uint32_t GetMinTimeForTurn() const { return m_minTimeForTurn; }
	uint32_t GetMaxTimeForTurn() const { return m_maxTimeForTurn; }
	void SetDir(const IntVec2& dir) { m_dir = dir; }
	IntVec2 GetForward() const { return m_dir; }
	IntVec2 GetLeft() const 
	{ 
		if (m_dir.x != 0)
		{
			if (m_dir.x > 0)
			{
				return IntVec2(0, -1);
			}
			else
			{
				return IntVec2(0, 1);
			}
		}
		else
		{
			if (m_dir.y > 0)
			{
				return IntVec2(1, 0);
			}
			else
			{
				return IntVec2(-1, 0);
			}
		}
	}
	IntVec2 GetRight() const 
	{ 
		if (m_dir.x != 0)
		{
			if (m_dir.x > 0)
			{
				return IntVec2(0, 1);
			}
			else
			{
				return IntVec2(0, -1);
			}
		}
		else
		{
			if (m_dir.y > 0)
			{
				return IntVec2(-1, 0);
			}
			else
			{
				return IntVec2(1, 0);
			}
		}
	}
public:
	size_t m_hash;
	IntVec2 m_pos;
	IntVec2 m_goalPos;
	IntVec2 m_dir;
	uint32_t m_timeSinceLastTurn;
	uint32_t m_totalHeatLoss;
	uint32_t m_minTimeForTurn;
	uint32_t m_maxTimeForTurn;
};

class CrucibleExecuter : public AStar::AStarExecuter<uint32_t>
{
public:
	CrucibleExecuter(const IntVec2& inGoal, const std::vector<uint32_t>& inMap, uint32_t mapWidth, AStar::AStarLogger logger = nullptr, uint32_t minGoalDistance = 0) 
	: AStarExecuter<uint32_t>(logger),
	m_map(inMap), 
	m_mapWidth(mapWidth), 
	m_goalPos(inGoal) 
	{ 
		m_mapHeight = (uint32_t)m_map.size() / m_mapWidth; 
	};

	static uint32_t GetManhattanDistance(const struct IntVec2& a, const IntVec2& b)
	{
		return abs(a.x - b.x) + abs(a.y - b.y);
	}

	uint32_t GetHeatLoss(const IntVec2& pos) const
	{
		return m_map[pos.y * m_mapWidth + pos.x];
	}

	bool IsValidMove(const CrucibleGridNode* node, const IntVec2& dir) const
	{
		IntVec2 nextPos = node->GetPos() + dir;

		if (nextPos.x < 0 || nextPos.y < 0 || nextPos.x >= (int32_t)m_mapWidth || nextPos.y >= (int32_t)m_mapHeight)
		{
			return false;
		}

		if (node->GetDir() == dir)
		{
			return node->GetTimeSinceLastTurn() < node->GetMaxTimeForTurn();
		}

		if (dir == node->GetLeft() || dir == node->GetRight())
		{
			return node->GetTimeSinceLastTurn() >= node->GetMinTimeForTurn();
		}

		return false;
	}

	virtual void OnProcessNode(AStar::AStarNodeBase<uint32_t>& CurrentNode) override
	{
		const CrucibleGridNode* currentNode = CurrentNode.As<CrucibleGridNode>();
		IntVec2 currentPos;

		//Log("Evaluating Node [%d, %d]", currentNode->GetPos().x, currentNode->GetPos().y);

		const IntVec2 possibleChoices[] = { currentNode->GetForward(), currentNode->GetLeft(), currentNode->GetRight() };

		for (const IntVec2& choice : possibleChoices)
		{
			if (!IsValidMove(currentNode, choice))
			{
				continue;
			}

			CrucibleGridNode* newNode = new CrucibleGridNode(
			currentNode->GetPos() + choice, 
			choice, 
			m_goalPos, 
			currentNode->GetTotalHeatLoss() + GetHeatLoss(currentNode->GetPos() + choice),
			(choice == currentNode->GetForward() ? currentNode->GetTimeSinceLastTurn() + 1 : 1),
			currentNode->GetMinTimeForTurn(), currentNode->GetMaxTimeForTurn());
			newNode->SetParent(currentNode);
			newNode->SetHeuristic(1); // H
			newNode->SetCost(newNode->GetTotalHeatLoss()); // G

			const AStar::AStarNodeBase<uint32_t>* closedNode = GetClosedListNode(newNode);
			if (closedNode)
			{
				// Skip it.
				delete newNode;
				continue;
			}

			// See if we have a better cost than an existing entry in the open list.
			int32_t existingIndex = FindIndexInOpenList(newNode);
			if (existingIndex != -1)
			{
				CrucibleGridNode* existingNode = m_openList[existingIndex]->As<CrucibleGridNode>();
				if (newNode->GetCost() < existingNode->GetCost())
				{
					m_openList[existingIndex] = newNode;
					ReInsertNode(existingIndex);
					delete existingNode;
					continue;
				}
				else
				{
					delete newNode;
					continue;
				}
			}
			//Log("\t- New Node. Added [%d, %d] Dir [%d, %d], G [%u] H[%u] F [%u].",
			//	newNode->GetPos().x, newNode->GetPos().y,
			//	newNode->GetDir().x, newNode->GetDir().y,
			//	newNode->GetCost(), newNode->GetHeuristic(), newNode->GetTotalCost());
			// Insert into the open list
			InsertNode(newNode);
		}


	}
private:
	std::vector<uint32_t> m_map;
	uint32_t m_mapWidth;
	uint32_t m_mapHeight;
	IntVec2  m_goalPos;
};

void AStarLogFunc(const char* fmt, va_list args)
{
	AdventGUIInstance::Get()->VLog(fmt, args);
}

class AdventDay : public AdventGUIInstance
{
public:
	AdventDay(const AdventGUIParams& params) 
	: AdventGUIInstance(params),
	m_MapWidth(0),
	m_MapHeight(0)
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
			if (m_MapWidth == 0)
			{
				m_MapWidth = (uint32_t)line.size();
				m_Map.reserve(m_MapWidth * m_MapWidth); 
			}
			assert(m_MapWidth == (uint32_t)line.size());
			for (char c : line)
			{
				m_Map.push_back(c - '0');
			}
		}

		m_MapHeight = (uint32_t)m_Map.size() / m_MapWidth;
	}

	void DrawMapAndPath(const std::vector<const CrucibleGridNode*>& path) const
	{
		char displayBuffer[2048] = { 0 };
		char* writePtr = displayBuffer;
		for (uint32_t i = 0; i < m_MapHeight; ++i)
		{
			for (uint32_t j = 0; j < m_MapWidth; ++j)
			{
				int32_t pathIndex = Algorithm::find_index_of(path.begin(), path.end(), [&](const CrucibleGridNode* RHS) { return RHS->GetPos() == IntVec2(j, i); });
				if (pathIndex != -1 && path[pathIndex]->m_dir != IntVec2::Zero)
				{
					const IntVec2& dir = path[pathIndex]->m_dir;
					if (dir.x != 0)
					{
						*writePtr = dir.x > 0 ? '>' : '<';
					}
					else
					{
						*writePtr = dir.y > 0 ? 'v' : '^';
					}
				}
				else
				{
					*writePtr = '0' + m_Map[i * m_MapWidth + j];
				}
				++writePtr;
			}
			*writePtr = '\n';
			++writePtr;
			*writePtr = '\t';
			++writePtr;
		}
		*writePtr = '\0';

		Log("Map:\n\t%s", displayBuffer);
	}

	virtual void PartOne(const AdventGUIContext& context) override
	{
		// Part One
		const IntVec2 goal(m_MapWidth - 1, m_MapHeight - 1);

		CrucibleExecuter executer(goal, m_Map, m_MapWidth, &AStarLogFunc);
		CrucibleGridNode* initialNode = new CrucibleGridNode(IntVec2::Zero, IntVec2(1,0), goal, 0, 1, 0, 3);
		executer.InsertNode(initialNode);

		std::vector<const CrucibleGridNode*> solvedPath;
		const AStar::AStarNodeBase<uint32_t>* pathNode = nullptr;

		uint32_t totalHeatLoss = 0;
		if (executer.Solve(pathNode))
		{
			// Chain up for the path route
			while (pathNode)
			{
				solvedPath.push_back(pathNode->As<CrucibleGridNode>());
				IntVec2 nodePos = solvedPath.back()->GetPos();
				if (pathNode->GetParent())
				{
					totalHeatLoss += m_Map[nodePos.y * m_MapWidth + nodePos.x];
				}

				pathNode = pathNode->GetParent();
			}
			
			
			//Log("Completed Path:");
			for (const CrucibleGridNode* path : solvedPath)
			{
				//Log("\t- Pos [%d, %d] Dir[%d, %d] HL [%u]", path->GetPos().x, path->GetPos().y, path->GetDir().x, path->GetDir().y, m_Map[path->GetPos().y * m_MapWidth + path->GetPos().x]);
			}

			//DrawMapAndPath(solvedPath);

			Log("Total Heat Loss: %u", totalHeatLoss);
		}
		else
		{
			// No path found
			Log("No Path Found.");
		}

		// Done.
		AdventGUIInstance::PartOne(context);

	}

	virtual void PartTwo(const AdventGUIContext& context) override
	{	
		// Part Two
		const IntVec2 goal(m_MapWidth - 1, m_MapHeight - 1);

		CrucibleExecuter executer(goal, m_Map, m_MapWidth, &AStarLogFunc);
		CrucibleGridNode* initialEastNode = new CrucibleGridNode(IntVec2::Zero, IntVec2(1, 0), goal, 0, 1, 4, 10);
		executer.InsertNode(initialEastNode);
		CrucibleGridNode* initialSouthNode = new CrucibleGridNode(IntVec2::Zero, IntVec2(0, 1), goal, 0, 1, 4, 10);
		executer.InsertNode(initialSouthNode);

		std::vector<const CrucibleGridNode*> solvedPath;
		const AStar::AStarNodeBase<uint32_t>* pathNode = nullptr;
		uint32_t totalHeatLoss = 0;
		if (executer.Solve(pathNode))
		{
			// Chain up for the path route
			while (pathNode)
			{
				solvedPath.push_back(pathNode->As<CrucibleGridNode>());
				IntVec2 nodePos = solvedPath.back()->GetPos();
				if (pathNode->GetParent())
				{
					totalHeatLoss += m_Map[nodePos.y * m_MapWidth + nodePos.x];
				}

				pathNode = pathNode->GetParent();
			}

			//Log("Completed Path:");
			for (const CrucibleGridNode* path : solvedPath)
			{
				//Log("\t- Pos [%d, %d] Dir[%d, %d] HL [%u]", path->GetPos().x, path->GetPos().y, path->GetDir().x, path->GetDir().y, m_Map[path->GetPos().y * m_MapWidth + path->GetPos().x]);
			}

			//DrawMapAndPath(solvedPath);

			Log("Total Heat Loss: %u", totalHeatLoss);
		}
		else
		{
			Log("No Path Found.");
		}

		// Done.
		AdventGUIInstance::PartTwo(context);
	}

	std::vector<uint32_t> m_Map;
	uint32_t m_MapWidth;
	uint32_t m_MapHeight;
};

int main()
{
	AdventGUIParams newParams;
	newParams.day = 17;
	newParams.year = 2023;
	newParams.puzzleTitle = "Clumsy Crucible";
	newParams.inputFilename = "input.txt";

	AdventGUIInstance::InstantiateAndExecute<AdventDay>(newParams);

	return 0;
}
