#pragma once

#include <algorithm>
#include <unordered_set>
#include <vector>

/*
    Template A* Algorithm.
	
	Overview: https://www.geeksforgeeks.org/a-search-algorithm/

	This class just handles some of the book keeping for you. T in all the templates is the type for your heuristic/cost. This is likely float/int/uint.

	AStartNodeBase<T> - Represents a single node in your traversal.  
	AStarNodeHashCompare<T> - Used by the search algorithms to compare a node to another. By default, it will just call AStarNodeBase::GetHash.
	AStarExecuter<T> - The engine, so to speak, of the algorithm. This will manage the open/closed lists for you, as well as put up the framework of the general algorithm.
					   AStarExecuter<T>::ProcessNode will be called each time the current node is not at the goal. This is where you want to toss new entries on to the open list and any other node logic.

	Example:

	class SimpleGridNode : public AStarNodeBase<uint32_t>
	{
	public:
		SimpleGridNode(const IntVec2& pos, const IntVec2& goal)	: m_hash(std::hash<IntVec2>{}(pos)), m_pos(pos), m_goalPos(goal) {}
		virtual size_t GetHash() const override { return m_hash; }
		virtual bool IsAtGoal() const override { return m_pos == m_goalPos; }
		const IntVec2& GetPos() const { return m_pos; }
	public:
		size_t m_hash;
		IntVec2 m_pos;
		IntVec2 m_goalPos;
	};

	class SimpleExecuter : public AStarExecuter<uint32_t>
	{
	public:
		SimpleExecuter(const IntVec2& inGoal): m_goalPos(inGoal) {};

		virtual void OnProcessNode(AStarNodeBase<uint32_t>& CurrentNode)
		{
			const SimpleNode* currentNode = CurrentNode.As<SimpleNode>();

			// Populate our choices from this node. For example, all orthogonal neighbors:

			IntVec2 currentPos;
			static const IntVec2 neighbors[] =
			{
				{0, 1},
				{1, 0},
				{0, -1},
				{-1, 0}
			};

			for (const IntVec2& offset : neighbors)
			{
				currentPos = currentNode->GetPos() + offset;

				SimpleNode* newNode = new SimpleNode(currentPos, m_goalPos);
				newNode->SetParent(currentNode);
				newNode->SetHeuristic(GetManhattanDistance(currentPos, m_goalPos)); // H
				newNode->SetCost(currentNode->GetCost() + 1); // G

				// See if we're in the closed list at a better cost (and we thus this node is a dead end)
				const AStarNodeBase<uint32_t>* closedNode = GetClosedListNode(newNode);
				if (closedNode && closedNode->GetTotalCost() < newNode->GetTotalCost())
				{
					// Skip it.
					delete newNode;
					continue;
				}

				// See if we have a better cost than an existing entry in the open list.
				int32_t existingIndex = FindIndexInOpenList(newNode);
				if (existingIndex != -1)
				{
					AStarNodeBase<uint32_t>* existingNode = m_openList[existingIndex];
					if (newNode->GetTotalCost() > existingNode->GetCost())
					{
						delete newNode;
						continue;
					}
				}

				// Insert into the open list
				InsertNode(newNode);
			}
		}
	private:
		IntVec2  m_goalPos;
	};

	// Invoke it:
	SimpleExecuter executer(goalPos);
	SimpleNode* initialNode = new SimpleNode(startPos, goalPos);
	executer.InsertNode(initialNode);

	std::vector<const SimpleNode*> solvedPath;
	const AStarNodeBase<uint32_t>* pathNode = nullptr;
	if (executer.Solve(pathNode))
	{
		// Chain up for the path route
		while (pathNode)
		{
			solvedPath.push_back(pathNode->As<SimpleNode>());
			pathNode = pathNode->GetParent();
		}

		// Do whatever with the results
	}
	else
	{
	    // No path found
	}
*/


namespace AStar
{
	template<typename T>
	class AStarNodeBase
	{
	public:
		AStarNodeBase() : Parent(nullptr) {}
		AStarNodeBase(T _Cost, T _Heuristic) : Cost(_Cost), Heuristic(_Heuristic), Parent(nullptr) {}

		virtual bool IsValid() const { return true; }
		virtual bool IsAtGoal() const { return false; }
		virtual T GetTotalCost() const { return Cost + Heuristic; }
		virtual size_t GetHash() const = 0;

		T GetCost() const { return Cost; }
		T GetHeuristic() const { return Heuristic; }
		const AStarNodeBase* GetParent() const { return Parent; }

		void SetCost(T _G) { Cost = _G; }
		void SetHeuristic(T _H) { Heuristic = _H; }
		void SetParent(const AStarNodeBase* _Parent) { Parent = _Parent; }

		template<typename U>
		U* As() { return reinterpret_cast<U*>(this); }

		template<typename U>
		const U* As() const { return reinterpret_cast<const U*>(this); }
	protected:
		T Cost;
		T Heuristic;
		const AStarNodeBase* Parent;
	};

	template<typename T>
	class AStarNodeBaseHasher
	{
	public:
		std::size_t operator()(const AStarNodeBase<T>* LHS) const
		{
			return LHS->GetHash();
		}
	};

	template<typename T>
	class AStarNodeHashCompare
	{
	public:
		bool operator()(const AStarNodeBase<T>* LHS, const AStarNodeBase<T>* RHS) const
		{
			return LHS->GetHash() == RHS->GetHash();
		}
	};

	typedef void (*AStarLogger)(const char* fmt, va_list args);
	
	template<typename T, class ComparePred = AStarNodeHashCompare<T> >
	class AStarExecuter
	{
	public:
		AStarExecuter(AStarLogger logger = nullptr): m_logger(logger) { m_openList.reserve(DEFAULT_RESERVE_SIZE); m_closedList.reserve(DEFAULT_RESERVE_SIZE * CLOSED_LIST_SIZE_MULTIPLER); }

		void Log(const char* fmt, ...)
		{
			if (m_logger)
			{
				va_list args;
				va_start(args, fmt);
				(*m_logger)(fmt, args);
				va_end(args);
			}
		}

		virtual void OnProcessNode(AStarNodeBase<T>& CurrentNode) { };
		bool Solve(const AStarNodeBase<T>*& OutPath)
		{
			OutPath = nullptr;
			m_closedList.clear();

			while (!m_openList.empty())
			{
				AStarNodeBase<T>* CurrentNode = m_openList.back();
				m_openList.pop_back();

				m_closedList.insert(CurrentNode);

				if (CurrentNode->IsAtGoal())
				{
					OutPath = CurrentNode;
					return true;
				}

				OnProcessNode(*CurrentNode);
			}

			return false;
		}

		void InsertNode(AStarNodeBase<T>* NewNode)
		{
			// Compiler doesn't like marking this as a forward iterator
			auto itLower = std::find_if(m_openList.begin(), m_openList.end(), [=](const AStarNodeBase<T>* LHS)
				{
					return NewNode->GetTotalCost() > LHS->GetTotalCost();
				});

			if (itLower == m_openList.end())
			{
				m_openList.push_back(NewNode);
			}
			else
			{
				m_openList.insert(itLower, NewNode);
			}
		}

		void ReInsertNode(AStarNodeBase<T>* Node)
		{
			int32_t index = FindIndexInOpenList(Node);
			if (index != -1)
			{
				m_openList.erase(m_openList.begin() + index);
			}

			InsertNode(Node);
		}

		void ReInsertNode(int32_t index)
		{
			assert(index >= 0 && index < m_openList.size());

			AStarNodeBase<T>* Node = m_openList[index];
			m_openList.erase(m_openList.begin() + index);
			InsertNode(Node);
		}

		int32_t FindIndexInOpenList(const AStarNodeBase<T>* Node) const
		{
			auto itLower = std::find_if(m_openList.begin(), m_openList.end(), [=](const AStarNodeBase<T>* LHS)
				{
					return ComparePred()(LHS, Node);
				});

			if (itLower == m_openList.end())
			{
				return -1;
			}

			return (int32_t)(itLower - m_openList.begin());
		}

		bool IsInOpenList(AStarNodeBase<T>* Node) const
		{
			return FindIndexInOpenList(Node) != -1;
		}

		bool IsInClosedList(AStarNodeBase<T>* Node) const
		{
			return m_closedList.find(Node) != m_closedList.end();
		}

		const AStarNodeBase<T>* GetClosedListNode(AStarNodeBase<T>* Node) const
		{
			auto itFind = m_closedList.find(Node);
			if (itFind != m_closedList.end())
			{
				return *itFind;
			}

			return nullptr;
		}
	protected:
		static constexpr size_t DEFAULT_RESERVE_SIZE = 1024;
		static constexpr size_t CLOSED_LIST_SIZE_MULTIPLER = 3;

		typedef std::vector<AStarNodeBase<T>*> NodeList;
		NodeList m_openList;
		std::unordered_set<AStarNodeBase<T>*, AStarNodeBaseHasher<T>, ComparePred> m_closedList;
		AStarLogger m_logger;
	};
}
