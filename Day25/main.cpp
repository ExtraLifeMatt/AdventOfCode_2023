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
#include <unordered_map>
#include <random>

struct Vert
{
	Vert():name{0}, hash(0){}
	Vert(const std::string& _name)
	{
		assert(_name.length() < 10);
		strcpy_s(name, 10, _name.c_str());
		hash = (size_t)Hash::HashString32(_name.c_str());
	}
	Vert(size_t _hash): hash(_hash){}

	size_t GetHash() const { return hash; }
	char name[10];
	size_t hash;
	std::vector<size_t> superNodeHistory;

	bool operator==(const Vert& RHS) const
	{
		return hash == RHS.hash;
	}
};

struct Edge
{
	Edge():A(0), B(0), removed(false){}
	Edge(size_t _a, size_t _b):A(_a), B(_b), removed(false) { }
	size_t A;
	size_t B;
	bool removed;
};

ENABLE_STL_HASH(Vert, GetHash);

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
		std::string left;
		std::string right;
		while (!fileReader.IsEOF())
		{
			line = fileReader.ReadLine();
			size_t splitIdx = line.find(':');
			assert(splitIdx != std::string::npos);
			left = line.substr(0, splitIdx);
			right = line.substr(splitIdx + 1);

			Vert leftWire(left);
			m_Wires.insert( leftWire);
			
			if (m_HashToEdgeIndices.find(leftWire.GetHash()) == m_HashToEdgeIndices.end())
			{
				m_HashToEdgeIndices.insert(std::make_pair(leftWire.GetHash(), std::vector<size_t>()));
			}

			StringUtil::SplitBy(right, " ", tokens);
			for (const std::string& tok : tokens)
			{
				if (tok.empty())
				{
					continue;
				}

				Vert rightWire(tok);
				m_Wires.insert(rightWire);

				std::vector<Edge>::const_iterator itFind = std::find_if(m_Edges.begin(), m_Edges.end(), [&](const Edge& LHS)
				{ 
					return (LHS.A == leftWire.GetHash() && LHS.B == rightWire.GetHash()) || 
						   (LHS.A == rightWire.GetHash() && LHS.B == leftWire.GetHash()); 
				});

				if (itFind == m_Edges.end())
				{
					m_Edges.emplace_back(leftWire.GetHash(), rightWire.GetHash());
				}

				if (m_HashToEdgeIndices.find(rightWire.GetHash()) == m_HashToEdgeIndices.end())
				{
					m_HashToEdgeIndices.insert(std::make_pair(rightWire.GetHash(), std::vector<size_t>()));
				}

				m_HashToEdgeIndices[leftWire.GetHash()].push_back(m_Edges.size() - 1);
				m_HashToEdgeIndices[rightWire.GetHash()].push_back(m_Edges.size() - 1);
			}
		}

		Log("Total Wires: %zd", m_Wires.size());
	}

	bool Krager(const std::unordered_set<Vert>& verts, const std::vector<Edge>& edges, const std::unordered_map<size_t, std::vector<size_t>>& hashToEdgeMap, Vert& outGroupOne, Vert& outGroupTwo)
	{
		std::unordered_set<Vert> vertCopy(verts);
		std::vector<Edge> edgeCopy(edges);
		std::unordered_map<size_t, std::vector<size_t>> hashToEdgeCopy(hashToEdgeMap);

		outGroupOne.superNodeHistory.clear();
		outGroupTwo.superNodeHistory.clear();

		std::random_device rd;  // a seed source for the random number engine
		std::mt19937 rngGen(rd()); 

		int32_t superVertCounter = 0;
		std::string sVertName;
		while (vertCopy.size() > 2)
		{
			// Krager's Algo.
			
			// Grab a random edge.
			int32_t edgeIndex = rngGen() % edgeCopy.size();

			// Collapse the verts on this edge into one "super vert".
			Edge& edgeToCollapse = edgeCopy[edgeIndex];
			
			if (edgeToCollapse.A == edgeToCollapse.B)
			{
				edgeToCollapse.removed = true;
			}
			
			if (edgeToCollapse.removed)
			{
				continue;
			}


			//Log("Collapsing Edge Idx [%zd] [%zd] - [%zd]", edgeIndex, edgeToCollapse.A, edgeToCollapse.B);

			// Create our new Vert with a random unique name.
			Vert newVert;
			sVertName.clear();
			do 
			{
				for (size_t i = 0; i < 3; ++i)
				{
					sVertName += ('A' + (rngGen() % 26));
				}

				newVert = Vert(sVertName);
			} while (vertCopy.find(newVert) != vertCopy.end());

			//Log("\t- New Super Vert [%s][%zd] created to replace [%zd] and [%zd]", sVertName.c_str(), newVert.GetHash(), edgeToCollapse.A, edgeToCollapse.B);

			// Add our new vert.
			std::unordered_set<Vert>::const_iterator itA = vertCopy.find(Vert(edgeToCollapse.A));
			std::unordered_set<Vert>::const_iterator itB = vertCopy.find(Vert(edgeToCollapse.B));
			assert(itA != vertCopy.end() && itB != vertCopy.end());
			
			if (itA->superNodeHistory.size() != 0)
			{
				newVert.superNodeHistory.insert(newVert.superNodeHistory.end(), itA->superNodeHistory.begin(), itA->superNodeHistory.end());
			}
			else
			{
				newVert.superNodeHistory.push_back(edgeToCollapse.A);
			}

			if (itB->superNodeHistory.size() != 0)
			{
				newVert.superNodeHistory.insert(newVert.superNodeHistory.end(), itB->superNodeHistory.begin(), itB->superNodeHistory.end());
			}
			else
			{
				newVert.superNodeHistory.push_back(edgeToCollapse.B);
			}

			vertCopy.insert(newVert);
			hashToEdgeCopy.insert(std::make_pair(newVert.GetHash(), std::vector<size_t>()));

			std::vector<size_t>& newVertEdges = hashToEdgeCopy[newVert.GetHash()];
			// Replace any connections.
			// A -> whatever becomes sV -> whatever
			// B -> whatever becomes sV -> whatever
			// A -> B is deleted.

			std::vector<size_t>& vertAConnections = hashToEdgeCopy[edgeToCollapse.A];
			std::vector<size_t>& vertBConnections = hashToEdgeCopy[edgeToCollapse.B];

			//Log("\tReplacing connections to [%zd] with [%zd]", edgeToCollapse.A, newVert.GetHash());

			for (size_t idx : vertAConnections)
			{
				if (idx == edgeIndex)
				{
					continue;
				}

				if (edgeCopy[idx].removed)
				{
					continue;
				}

				if (edgeCopy[idx].A == edgeToCollapse.A)
				{
					//Log("\tEdge Idx [%zd] Replaced A [%zd] with [%zd]", idx, edgeCopy[idx].A, newVert.GetHash());
					edgeCopy[idx].A = newVert.GetHash();
				} 
				else if (edgeCopy[idx].B == edgeToCollapse.A)
				{
					//Log("\tEdge Idx [%zd] Replaced B [%zd] with [%zd]", idx, edgeCopy[idx].B, newVert.GetHash());
					edgeCopy[idx].B = newVert.GetHash();
				}

				newVertEdges.push_back(idx);
			}

			for (size_t idx : vertBConnections)
			{
				if (idx == edgeIndex)
				{
					continue;
				}

				if (edgeCopy[idx].removed)
				{
					continue;
				}

				if (edgeCopy[idx].A == edgeToCollapse.B)
				{
					//Log("\tEdge Idx [%zd] Replaced A [%zd] with [%zd]", idx, edgeCopy[idx].A, newVert.GetHash());
					edgeCopy[idx].A = newVert.GetHash();
				}
				else if (edgeCopy[idx].B == edgeToCollapse.B)
				{
					//Log("\tEdge Idx [%zd] Replaced B [%zd] with [%zd]", idx, edgeCopy[idx].B, newVert.GetHash());
					edgeCopy[idx].B = newVert.GetHash();
				}

				newVertEdges.push_back(idx);
			}

			//Log("Erasing Verts [%zd] and [%zd]", edgeToCollapse.A, edgeToCollapse.B);
			vertCopy.erase(Vert(edgeToCollapse.A));
			vertCopy.erase(Vert(edgeToCollapse.B));

			edgeToCollapse.removed = true;
		}

		Vert* outVerts[] = { &outGroupOne, &outGroupTwo};
		int32_t outVertIdx = 0;
		for (const Vert& superVerts : vertCopy)
		{
			assert(outVertIdx < 2);
			*outVerts[outVertIdx++] = superVerts;
		}

		std::vector<int32_t> allValidEdges = Algorithm::find_all_indices(edgeCopy.begin(), edgeCopy.end(), [](const Edge& LHS) { return LHS.removed == false; });
		return outGroupOne.superNodeHistory.size() != 0 && outGroupTwo.superNodeHistory.size() != 0;
	}

	virtual void PartOne(const AdventGUIContext& context) override
	{
		// Part One
		bool valid = false;
		Vert groupOne;
		Vert groupTwo;
		do 
		{
		  valid = Krager(m_Wires, m_Edges, m_HashToEdgeIndices, groupOne, groupTwo);
		} while (!valid);

		Log("Group 1 [%zd] Group 2 [%zd] Product [%zd]", groupOne.superNodeHistory.size(), groupTwo.superNodeHistory.size(), groupOne.superNodeHistory.size() * groupTwo.superNodeHistory.size());

		// Done.
		AdventGUIInstance::PartOne(context);
	}

	virtual void PartTwo(const AdventGUIContext& context) override
	{	
		// Part Two

		// Done.
		AdventGUIInstance::PartTwo(context);
	}

	std::unordered_set<Vert> m_Wires;
	std::vector<Edge> m_Edges;
	std::unordered_map<size_t, std::vector<size_t>> m_HashToEdgeIndices;
};

int main()
{
	AdventGUIParams newParams;
	newParams.day = 25;
	newParams.year = 2023;
	newParams.puzzleTitle = "Snowverload";
	newParams.inputFilename = "input.txt";

	AdventGUIInstance::InstantiateAndExecute<AdventDay>(newParams);

	return 0;
}
