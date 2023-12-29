// main.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "AdventGUI/AdventGUI.h"

#include "ACUtils/Algorithm.h"
#include "ACUtils/Hash.h"
#include "ACUtils/BDFS.h"
#include "ACUtils/IntVec.h"
#include "ACUtils/StringUtil.h"
#include <vector>
#include <queue>
#include <unordered_set>

class AdventDay : public AdventGUIInstance
{
public:
	AdventDay(const AdventGUIParams& params)
		: AdventGUIInstance(params), m_MapWidth(0), m_MapHeight(0)
	{};
private:
	struct Vert
	{
		Vert(const IntVec2& _pos):pos(_pos), connections{ nullptr }, numConnections(0) {};
		Vert():pos(0), connections{nullptr}, numConnections(0) {};

		void Connect(const Vert* to)
		{
			assert(numConnections < 4);
			connections[numConnections++] = to;
		}

		const Vert* connections[4];
		uint8_t numConnections;
		IntVec2 pos;
	};

	typedef std::vector<std::vector<std::pair<int32_t, int32_t>>> VertAdjLengthVector;
	typedef std::vector<const Vert*> VertVector;

	struct GraphWalkState
	{
		GraphWalkState(uint64_t oldVisited, const Vert* next, uint32_t inSteps): current(next), visited ( oldVisited ), totalSteps(inSteps), hash(0)
		{
			hash = visited | (uint64_t)totalSteps << 37UL;
		}

		const Vert* current;
		uint64_t visited;
		uint32_t totalSteps;
		size_t hash;
	};

	class GraphWalkBDFSNode : public BDFS::BDFSNode<GraphWalkState>
	{
	public:
		GraphWalkBDFSNode(const GraphWalkState& state) : BDFSNode(state) { }

		virtual size_t GetHash() const override
		{
			const GraphWalkState& state = GetState();
			return state.hash;
		}
	};

	class GraphWalkExecuter : public BDFS::BDFSExecuter<GraphWalkState>
	{
	public:
		GraphWalkExecuter(const VertVector* nodes, const VertAdjLengthVector* adjInfo, IntVec2 goalPos)
			: BDFSExecuter(BDFSExecuterMode::BDFSExecuterMode_BreadthFirst), m_Goal(goalPos), m_Nodes(nodes), m_AdjInfo(adjInfo)
		{
			GraphWalkState rootState(1, (*nodes)[0], 0);
			QueueNode(new GraphWalkBDFSNode(rootState));
		}

		virtual bool ProcessNode(const BDFS::BDFSNode<GraphWalkState>* node) override
		{
			const GraphWalkState& currentState = node->GetState();

			if (currentState.current->pos == m_Goal)
			{
				if (std::find_if(m_SolvedStates.begin(), m_SolvedStates.end(), [&](const GraphWalkState& LHS) { return LHS.totalSteps == currentState.totalSteps; }) == m_SolvedStates.end())
				{
					m_SolvedStates.push_back(currentState);
				}
			}
			else
			{
				// Keep going.

				int32_t nodeIdx = GetIndexForNode(currentState.current);
				assert(nodeIdx != -1);

				for ( const std::pair<int32_t, int32_t>& kvp : (*m_AdjInfo)[nodeIdx])
				{
					if ((currentState.visited & (1ULL << kvp.first)) == 0)
					{
						GraphWalkState newWalkState(currentState.visited | (1ULL << kvp.first), (*m_Nodes)[kvp.first], currentState.totalSteps + kvp.second);
						
						//std::unordered_map<uint64_t, uint32_t>::const_iterator itFind = m_Cache.find(newWalkState.visited);
						//if (itFind != m_Cache.end())
						//{
						//	if (itFind->second > newWalkState.totalSteps)
						//	{
						//		// Last one was better by this point.
						//		continue;
						//	}
						//}

						//m_Cache[newWalkState.visited] = newWalkState.totalSteps;
						
						QueueNode(new GraphWalkBDFSNode(newWalkState));
					}
				}
			}

			delete node;

			return false;
		};

		int32_t GetIndexForNode(const Vert* node) const
		{
			for (size_t i = 0; i < m_Nodes->size(); ++i)
			{
				if ((*m_Nodes)[i] == node)
				{
					return (int32_t)i;
				}
			}

			return -1;
		}

		IntVec2 m_Goal;
		std::vector<GraphWalkState> m_SolvedStates;
		const VertVector* m_Nodes;
		const VertAdjLengthVector* m_AdjInfo;
		std::unordered_map<uint64_t, uint32_t> m_Cache;
	};

	struct WalkState
	{
		WalkState(const IntVec2& _pos, const IntVec2& _dir, const std::unordered_set<IntVec2>& _stepHistory):pos(_pos), dir(_dir), stepHistory(_stepHistory), hash(0)
		{
			stepHistory.insert(pos);

			hash = pos.x | pos.y << 12;
			if (dir.x != 0)
			{
				hash |= (dir.x > 0 ? 1 : 2) << 24;
			}

			if (dir.y != 0)
			{
				hash |= (dir.y > 0 ? 1 : 2) << 26;
			}

			hash |= stepHistory.size() << 32UL;
		}

		bool HasPreviouslySteppedOn(const IntVec2& inPos) const { return stepHistory.find(inPos) != stepHistory.end(); }

		IntVec2 pos;
		IntVec2 dir;
		std::unordered_set<IntVec2> stepHistory;
		size_t hash;
	};

	class WalkBDFSNode : public BDFS::BDFSNode<WalkState>
	{
	public:
		WalkBDFSNode(const WalkState& state) : BDFSNode(state) { }

		virtual size_t GetHash() const override
		{
			const WalkState& state = GetState();
			return state.hash;
		}
	};

	class WalkExecuter : public BDFS::BDFSExecuter<WalkState>
	{
	public:
		WalkExecuter(const IntVec2& startPos, const IntVec2& goalPos, size_t mapWidth, size_t mapHeight, const std::vector<char>* map, bool allowSlopes = true)
			: BDFSExecuter(BDFSExecuterMode::BDFSExecuterMode_BreadthFirst), m_goal(goalPos), m_mapWidth(mapWidth), m_mapHeight(mapHeight), m_map(map), m_allowSlopes(allowSlopes)
		{
			WalkState rootState(startPos, IntVec2::Zero, std::unordered_set<IntVec2>());
			QueueNode(new WalkBDFSNode(rootState));
		}

		virtual bool ProcessNode(const BDFS::BDFSNode<WalkState>* node) override
		{
			const WalkState& currentState = node->GetState();
			bool queuedNode = false;

			if (currentState.pos == m_goal)
			{
				m_SolvedStates.push_back(currentState);
			}
			else if (m_allowSlopes && IsSlope(currentState.pos))
			{
				static constexpr char slopeChars[] = { '^', '>', 'v', '<' };
				static IntVec2 slopeDir[] = { {0, -1}, {1, 0}, {0, 1}, {-1, 0} };
				size_t slopeIdx = 0;
				for (slopeIdx = 0; slopeIdx < 4; ++slopeIdx)
				{
					if ((*m_map)[currentState.pos.y * m_mapWidth + currentState.pos.x] == slopeChars[slopeIdx])
					{
						break;
					}
				}

				if (!currentState.HasPreviouslySteppedOn(currentState.pos + slopeDir[slopeIdx]))
				{

					WalkState newWS(currentState.pos + slopeDir[slopeIdx], slopeDir[slopeIdx], currentState.stepHistory);
					QueueNode(new WalkBDFSNode(newWS));
				}

			}
			else
			{

				static IntVec2 offsets[] = { {0, 1}, {1, 0}, {0, -1}, {-1, 0}};
				for (const IntVec2& offset : offsets)
				{
					IntVec2 candidatePos = currentState.pos + offset;
					if (IsValidPos(candidatePos) && !currentState.HasPreviouslySteppedOn(candidatePos))
					{
						WalkState newWS(currentState.pos + offset, offset, currentState.stepHistory);
						QueueNode(new WalkBDFSNode(newWS));
					}
				}
			}
			
			delete node;

			return false;
		};

		bool IsValidPos(const IntVec2& pos) const
		{
			if (pos.x < 0 || pos.y < 0 || pos.x >= m_mapWidth || pos.y >= m_mapHeight)
			{
				return false;
			}

			return (*m_map)[pos.y * m_mapWidth + pos.x] != '#';
		}

		bool IsSlope(const IntVec2& pos) const
		{
			if (pos.x < 0 || pos.y < 0 || pos.x >= m_mapWidth || pos.y >= m_mapHeight)
			{
				return false;
			}

			char val = (*m_map)[pos.y * m_mapWidth + pos.x];
			return val != '#' && val != '.';
		}

		IntVec2 m_goal;
		bool m_allowSlopes;
		size_t m_mapWidth;
		size_t m_mapHeight;
		const std::vector<char>* m_map;

		std::vector<WalkState> m_SolvedStates;
	};

	void GetNextJunction(const IntVec2& start, const IntVec2& dir, IntVec2& outEnd)
	{
		outEnd = start;
		static IntVec2 offsets[] = { {0, 1},{0, -1}, {1, 0}, {-1, 0} };
		uint32_t sideIndex = dir.x == 0 ? 2 : 0;

		// Increment till junction or wall
		while (true)
		{
			IntVec2 nextStep = outEnd + dir;
			if (IsValidPos(nextStep))
			{
				for (uint32_t check = 0; check < 2; ++check)
				{
					if (IsValidPos(nextStep + offsets[sideIndex + check]))
					{
						// Junction here. Save and bounce.
						outEnd = nextStep;
						return;
					}
				}
				outEnd = nextStep;
			}
			else
			{
				break;
			}
		}		
	}

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
				m_MapWidth = line.size();
			}
			assert(line.size() == m_MapWidth);
			m_Map.insert(m_Map.end(), line.begin(), line.end());
		}

		m_MapHeight = m_Map.size() / m_MapWidth;

		m_allEdges.emplace_back(new Vert(IntVec2(1,0)));

		std::queue<Vert*> vertQueue;
		vertQueue.push(m_allEdges.back());
		while (!vertQueue.empty())
		{
			Vert* currentVert = vertQueue.front();
			vertQueue.pop();

			IntVec2 nextVert;
			static IntVec2 offsets[] = { {0, 1}, {1, 0}, {0, -1}, {-1, 0} };
			for (const IntVec2& offset : offsets)
			{
				
				if (!IsValidPos(currentVert->pos + offset))
				{
					continue;
				}

				GetNextJunction(currentVert->pos, offset, nextVert);

				std::vector<Vert*>::iterator itFind = std::find_if(m_allEdges.begin(), m_allEdges.end(), [&](const Vert* LHS) { return LHS->pos == nextVert; });

				if ( itFind == m_allEdges.end())
				{
					Vert* newVert = new Vert(nextVert);
					
					m_allEdges.push_back(newVert);

					// connect the two.
					newVert->Connect(currentVert);
					currentVert->Connect(newVert);

					vertQueue.push(newVert);
				}
				else
				{
					bool hasConnection = false;
					for (size_t i = 0; i < (*itFind)->numConnections; ++i)
					{
						hasConnection |= (*itFind)->connections[i] == currentVert;
					}

					if (!hasConnection)
					{
						(*itFind)->Connect(currentVert);
					}

				}
			}
		}

		Log("Read %zd points and constructed graph.", m_allEdges.size());

		// Collapse Nodes
		std::vector<Vert*> allNonSingleLane = Algorithm::find_all(m_allEdges.begin(), m_allEdges.end(), [&](const Vert* LHS){ return LHS->numConnections != 2;});

		m_condensedNodes.insert(m_condensedNodes.end(), allNonSingleLane.begin(), allNonSingleLane.end());

		Log("Total One way nodes [%zd]", allNonSingleLane.size());

		uint32_t totalNodesConsolidated = 0;
		for (size_t i = 0; i < allNonSingleLane.size(); ++i)
		{
			m_vertAdjAndLength.push_back(std::vector<std::pair<int32_t, int32_t>>());

			for (size_t j = 0; j < allNonSingleLane[i]->numConnections; ++j)
			{
				IntVec2 lastPos = allNonSingleLane[i]->pos;
				IntVec2 delta;
				const Vert* currentVert = allNonSingleLane[i]->connections[j];
				uint32_t totalLength = 0;
				std::vector<const Vert*> collectedVerts;
				collectedVerts.push_back(allNonSingleLane[i]);
				while (currentVert->numConnections == 2)
				{
					delta = currentVert->pos - lastPos;
					totalLength += abs(delta.x == 0 ? delta.y : delta.x);
					lastPos = currentVert->pos;

					collectedVerts.push_back(currentVert);
					currentVert = std::find(collectedVerts.begin(), collectedVerts.end(), currentVert->connections[0]) == collectedVerts.end() ? currentVert->connections[0] : currentVert->connections[1];
				}
				
				delta = currentVert->pos - lastPos;
				totalLength += abs(delta.x == 0 ? delta.y : delta.x);

				std::vector<const Vert*>::const_iterator itFind = std::find(m_condensedNodes.begin(), m_condensedNodes.end(),currentVert);
				assert(itFind != m_condensedNodes.end());
				
				m_vertAdjAndLength[i].push_back(std::make_pair((int32_t)(itFind - m_condensedNodes.begin()), (int32_t)totalLength));

				totalNodesConsolidated += (uint32_t)collectedVerts.size();
			}
		}

		Log("Consolidated [%u] nodes.", totalNodesConsolidated);
	}

	bool IsValidPos(const IntVec2& pos) const
	{
		if (pos.x < 0 || pos.y < 0 || pos.x >= m_MapWidth || pos.y >= m_MapHeight)
		{
			return false;
		}

		return m_Map[pos.y * m_MapWidth + pos.x] != '#';
	}

	virtual void PartOne(const AdventGUIContext& context) override
	{
		// Part One
		//WalkExecuter exec(IntVec2(1, 0), IntVec2((int32_t)m_MapWidth - 2, (int32_t)m_MapHeight - 1), m_MapWidth, m_MapHeight, &m_Map);

		//exec.Solve();

		//std::sort(exec.m_SolvedStates.begin(), exec.m_SolvedStates.end(), [&](const WalkState& LHS, const WalkState& RHS) { return LHS.stepHistory.size() > RHS.stepHistory.size(); });

		//Log("Most Steps: %u", exec.m_SolvedStates[0].stepHistory.size() - 1);

		// Done.
		AdventGUIInstance::PartOne(context);
	}

	virtual void PartTwo(const AdventGUIContext& context) override
	{	
		// Part Two
		GraphWalkExecuter graphExec(&m_condensedNodes, &m_vertAdjAndLength, IntVec2((int32_t)m_MapWidth - 2, (int32_t)m_MapHeight - 1));

		graphExec.Solve();

		std::sort(graphExec.m_SolvedStates.begin(), graphExec.m_SolvedStates.end(), [](const GraphWalkState& LHS, const GraphWalkState& RHS){ return LHS.totalSteps > RHS.totalSteps; });

		Log("Most Steps: %u", graphExec.m_SolvedStates[0].totalSteps);

		// Done.
		AdventGUIInstance::PartTwo(context);
	}
	size_t m_MapWidth;
	size_t m_MapHeight;
	std::vector<char> m_Map;
	Vert* m_graphRoot;
	std::vector<Vert*> m_allEdges;
	std::vector<const Vert*> m_condensedNodes;

	std::vector<IntVec2> m_verts;

	VertAdjLengthVector m_vertAdjAndLength;
};

int main()
{
	AdventGUIParams newParams;
	newParams.day = 23;
	newParams.year = 2023;
	newParams.puzzleTitle = "A Long Walk";
	newParams.inputFilename = "input.txt";

	AdventGUIInstance::InstantiateAndExecute<AdventDay>(newParams);

	return 0;
}
