#pragma once

#include <algorithm>
#include <assert.h>
#include <queue>
#include <stack>
#include <unordered_set>
/*
	Breadth First Search / Depth First Search

	Overview: https://www.geeksforgeeks.org/difference-between-bfs-and-dfs/

	This class just handles some of the book keeping for you. T in all the templates is the state of your world / data structure / etc.
	
	BDFSNode<T> - Represents a single node in your traversal.
	BDFSNodeHashCompare<T> - Used by the search algorithms to compare a node to another. By default, it will just call BDFSNode::GetHash.
	BDFSExecuter<T> - The engine, so to speak, of the algorithm. This will manage the internal stack / queue, as well as put up the framework of the general algorithm.
					   BDFSExecuter<T>::ProcessNode will be called each time the current node is not at the goal / there's more work in the stack. This is where you want to toss new entries on to the stack/queue and any other node logic.

	Example:
	struct MySimpleState
	{
		uint32_t someState;
	};

	class SimpleBDFSNode : public BDFSNode<MySimpleState>
	{
	public:
		SimpleBDFSNode(const MySimpleState& state) : BDFSNode(state) { }

		virtual size_t GetHash() const override
		{
			const MySimpleState& state = GetState();
			return state.someState * 17; // Very basic hash, just multiply by a prime number.
		}
	};

	class SimpleExecutor : public BDFSExecuter<MySimpleState>
	{
	public:
		Emulator(uint32_t numStepsToDo, BDFSExecuterMode mode)
			: BDFSExecuter(mode)
		{
			MySimpleState rootState;
			rootState.SomeState = 0;
			QueueNode(new SimpleBDFSNode(rootState));
		}

		virtual bool ProcessNode(const BDFSNode<MySimpleState>* node) override
		{
			const MySimpleState& currentState = node->GetState();
			bool queuedNode = false;
			// Process this state in whatever fashion. Potentially queue new nodes. 
			return queuedNode;
		};
	};

	// Invoke it:
	SimpleExecutor Exec(totalIters, BDFSExecuterMode::BDFSExecuterMode_DepthFirst);
	Exec.Solve();
*/


namespace BDFS
{
	template<typename T>
	class BDFSNode
	{
	public:
		BDFSNode(T state) : m_state(state), m_parent(nullptr) {}

		virtual bool IsAtGoal() const { return false; }
		virtual size_t GetHash() const = 0;
		const BDFSNode* GetParent() const { return m_parent; }

		void SetParent(const BDFSNode* _Parent) { m_parent = _Parent; }

		const T& GetState() const { return m_state; }

		template<typename U>
		U* As() { return reinterpret_cast<U*>(this); }

		template<typename U>
		const U* As() const { return reinterpret_cast<const U*>(this); }
	protected:
		T m_state;
		const BDFSNode* m_parent;
	};

	template<typename T>
	class BDFSNodeHasher
	{
	public:
		std::size_t operator()(const BDFSNode<T>* LHS) const
		{
			return LHS->GetHash();
		}
	};

	template<typename T>
	class BDFSNodeHashCompare
	{
	public:
		bool operator()(const BDFSNode<T>* LHS, const BDFSNode<T>* RHS) const
		{
			return LHS->GetHash() == RHS->GetHash();
		}
	};

	template<typename T>
	class BDFSExecuter
	{
	public:
		enum class BDFSExecuterMode :uint8_t
		{
			BDFSExecuterMode_BreadthFirst,
			BDFSExecuterMode_DepthFirst,
		};

		typedef std::unordered_set <BDFSNode<T>*, BDFSNodeHasher<T>, BDFSNodeHashCompare<T>> DiscoveredNodeSet;

		BDFSExecuter(BDFSExecuterMode mode)
			: m_mode(mode)
		{
			m_discovered.reserve(DEFAULT_RESERVE_SIZE * DISCOVERED_SIZE_MULTIPLER);
			Reset();
		}

		~BDFSExecuter()
		{
			std::for_each(m_discovered.begin(), m_discovered.end(), [&](BDFSNode<T>* LHS)
				{
					delete LHS;
				});

			Reset();
		}

		// If in DFS mode, returning false will remove the node from the stack. Returning true, keeps the node and we just take the top of the stack again.
		virtual bool ProcessNode(const BDFSNode<T>* node) = 0;

		void Reset()
		{
			m_discovered.clear();
			while (!m_queue.empty()) { m_queue.pop(); }
			while (!m_stack.empty()) { m_stack.pop(); }
			m_goalEnd = nullptr;
		}
		void Solve()
		{
			if (m_mode == BDFSExecuterMode::BDFSExecuterMode_BreadthFirst)
			{
				SolveBFS();
			}
			else
			{
				SolveDFS();
			}
		}

		BDFSExecuterMode GetMode() const { return m_mode; }
		bool HasGoal() const { return m_goalEnd != nullptr; }
		const BDFSNode<T>* GetGoalEnd() const { return m_goalEnd; }
		bool IsDiscovered(BDFSNode<T>* node) const { return m_discovered.find(node) != m_discovered.end(); }

		// BFS requires the current node as well as the node for queue.
		void QueueNode(BDFSNode<T>* next, const BDFSNode<T>* current = nullptr)
		{
			if (m_mode == BDFSExecuterMode::BDFSExecuterMode_BreadthFirst)
			{
				//assert(current); // BFS requires the current node so we can chain up the path.
				if (!IsDiscovered(next))
				{
					next->SetParent(current);
					m_queue.push(next);
				}
			}
			else
			{
				assert(m_mode == BDFSExecuterMode::BDFSExecuterMode_DepthFirst);
				m_stack.push(next);
			}

		}

		const DiscoveredNodeSet& GetDiscoveredNodes() const { return m_discovered; }
	private:

		void SolveDFS()
		{
			while (!m_stack.empty())
			{
				BDFSNode<T>* current = m_stack.top();
				m_discovered.insert(current);

				if (!ProcessNode(current))
				{
					m_stack.pop();
				}
			}
		}

		void SolveBFS()
		{
			while (!m_queue.empty())
			{
				BDFSNode<T>* current = m_queue.front();
				m_queue.pop();

				if (current->IsAtGoal())
				{
					m_goalEnd = current;
					return;
				}

				ProcessNode(current);
			}
		}

		static constexpr size_t DEFAULT_RESERVE_SIZE = 1024;
		static constexpr size_t DISCOVERED_SIZE_MULTIPLER = 3;
		BDFSExecuterMode m_mode;

		std::stack<BDFSNode<T>*> m_stack; // For DFS
		std::queue<BDFSNode<T>*> m_queue; // For BFS

		const BDFSNode<T>* m_goalEnd;

		DiscoveredNodeSet m_discovered;
	};

}
