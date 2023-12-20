// main.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "AdventGUI/AdventGUI.h"

#include "ACUtils/IntVec.h"
#include "ACUtils/Hash.h"
#include "ACUtils/StringUtil.h"
#include <vector>
#include <unordered_set>
#include <cinttypes>
#include <stack>

enum class ResultCode : uint8_t
{
	RC_None = 0,
	RC_Rejected,
	RC_Accepted,
	RC_Jump,
};

enum class TestAttribute : uint8_t
{
	TA_X = 0,
	TA_M,
	TA_A,
	TA_S,
};

enum class TestOp : uint8_t
{
	OP_LT,
	OP_GT,
	OP_CONSTANT,
};

struct Statement
{
	Statement(const std::string& rawStr)
		:RawString(rawStr),
		Attribute(TestAttribute::TA_A),
		Op(TestOp::OP_CONSTANT),
		ResultOnSuccess(ResultCode::RC_Rejected),
		Param(0)
	{
		size_t colonIdx = rawStr.find(':');
		if (colonIdx == std::string::npos)
		{
			Op = TestOp::OP_CONSTANT;
			if (rawStr.size() == 1)
			{
				switch (rawStr[0])
				{
				case 'R': ResultOnSuccess = ResultCode::RC_Rejected; break;
				case 'A': ResultOnSuccess = ResultCode::RC_Accepted; break;
				default: assert(false); break;
				}
			}
			else
			{
				ResultOnSuccess = ResultCode::RC_Jump;
				JumpParam = Hash::HashString32(rawStr.c_str());
			}
		}
		else
		{
			switch (rawStr[0])
			{
			case 'x': Attribute = TestAttribute::TA_X; break;
			case 'm': Attribute = TestAttribute::TA_M; break;
			case 'a': Attribute = TestAttribute::TA_A; break;
			case 's': Attribute = TestAttribute::TA_S; break;
			default: assert(false); break;
			}
			assert(rawStr[1] == '<' || rawStr[1] == '>');
			Op = (rawStr[1] == '<') ? TestOp::OP_LT : TestOp::OP_GT;
			std::string compValue = rawStr.substr(2, colonIdx - 2);
			std::string resultValue = rawStr.substr(colonIdx + 1);

			Param = (uint32_t)atoi(compValue.c_str());
			if (resultValue.size() == 1)
			{
				switch (resultValue[0])
				{
				case 'R': ResultOnSuccess = ResultCode::RC_Rejected; break;
				case 'A': ResultOnSuccess = ResultCode::RC_Accepted; break;
				default: assert(false); break;
				}
			}
			else
			{
				ResultOnSuccess = ResultCode::RC_Jump;
				JumpParam = Hash::HashString32(resultValue.c_str());
			}
		}
	}
	std::string RawString;
	TestAttribute Attribute;
	TestOp Op;
	ResultCode ResultOnSuccess;
	uint32_t Param;
	uint32_t JumpParam;
};

struct Workflow
{
	Workflow(uint32_t _nameHash): NameHash(_nameHash){};
	Workflow(const std::string& _rawString)
		: NameHash(0),
		RawString(_rawString)
	{
		std::vector<std::string> tokens;
		StringUtil::SplitBy(RawString, "{|}|,", tokens);
		NameHash = Hash::HashString32(tokens[0].c_str());
		for (size_t i = 1; i < tokens.size(); ++i)
		{
			LogicStatements.emplace_back(tokens[i]);
		}
	}

	size_t GetHash() const { return (size_t)NameHash; }
	bool operator==(const Workflow& RHS) const
	{
		return NameHash == RHS.NameHash;
	}

	uint32_t NameHash;
	std::string RawString;
	std::vector<Statement> LogicStatements;
};

ENABLE_STL_HASH(Workflow, GetHash);

struct TreeNode
{
	TreeNode(const Statement* statement) : Parent(nullptr), Children{ nullptr, nullptr }, NodeStatement(statement) {};
	TreeNode* Parent;
	TreeNode* Children[2];
	const Statement* NodeStatement;
};

class AdventDay : public AdventGUIInstance
{
public:
	AdventDay(const AdventGUIParams& params)
		: AdventGUIInstance(params)
	{};
private:
	uint32_t GetAttributeValue(const IntVec4& part, TestAttribute attrib) const
	{
		return (uint32_t)part[static_cast<uint8_t>(attrib)];
	}

	ResultCode ExecuteWorkflow(const std::unordered_set<Workflow>& workflows, const IntVec4& part) const
	{
		ResultCode rc = ResultCode::RC_None;
		uint32_t currentStatement = Hash::HashString32("in");

		while (rc != ResultCode::RC_Accepted && rc != ResultCode::RC_Rejected)
		{
			std::unordered_set<Workflow>::const_iterator itFind = workflows.find(Workflow(currentStatement));
			assert(itFind != workflows.end());

			for (const Statement& statement : itFind->LogicStatements)
			{
				bool statementSuccess = false;
				if (statement.Op == TestOp::OP_CONSTANT)
				{
					statementSuccess = true;
				}
				else if (statement.Op == TestOp::OP_LT)
				{
					statementSuccess = GetAttributeValue(part, statement.Attribute) < statement.Param;
				}
				else
				{
					assert(statement.Op == TestOp::OP_GT);
					statementSuccess = GetAttributeValue(part, statement.Attribute) > statement.Param;
				}

				if (statementSuccess)
				{
					if (statement.ResultOnSuccess == ResultCode::RC_Jump)
					{
						currentStatement = statement.JumpParam;
					}
					else
					{
						rc = statement.ResultOnSuccess;
					}
					break;
				}
			}
		}

		return rc;
	}

	struct RangeState
	{
		RangeState(const Workflow* _workFlow, uint8_t _statementIndex, IntVec4 _min, IntVec4 _max): CurrentWorkflow(_workFlow), StatementIndex(_statementIndex), RangeMin(_min), RangeMax(_max) {}
		const Workflow* CurrentWorkflow;
		uint8_t StatementIndex;
		IntVec4 RangeMin;
		IntVec4 RangeMax;
	};

	uint64_t GetRangeStateScore(const RangeState& rs) const
	{
		IntVec4 Delta = (rs.RangeMax - rs.RangeMin) + IntVec4(1);
		return (int64_t)Delta.x * (int64_t)Delta.y * (int64_t)Delta.z * (int64_t)Delta.w;
	}


	uint64_t GetValueSpace(std::stack<RangeState>& inOutStack, const RangeState& currentRangedState) const
	{
		const Statement& statement = currentRangedState.CurrentWorkflow->LogicStatements[currentRangedState.StatementIndex];
		uint8_t attributeIndex = (uint8_t)statement.Attribute;

		RangeState FailedCase(currentRangedState.CurrentWorkflow, currentRangedState.StatementIndex + 1, currentRangedState.RangeMin, currentRangedState.RangeMax);

		if (statement.Op == TestOp::OP_CONSTANT)
		{
			if (statement.ResultOnSuccess == ResultCode::RC_Accepted)
			{
				return GetRangeStateScore(currentRangedState);
			}
			else if (statement.ResultOnSuccess == ResultCode::RC_Jump)
			{
				std::unordered_set<Workflow>::const_iterator itFind = m_Workflows.find(Workflow(statement.JumpParam));
				assert(itFind != m_Workflows.end());
				inOutStack.emplace(&(*itFind), 0,  currentRangedState.RangeMin, currentRangedState.RangeMax);
			}
			else
			{
				return 0;
			}
		}
		else if (statement.Op == TestOp::OP_LT)
		{
			Log("Splitting to Ranges based on statement '%s'", statement.RawString.c_str());
			if (currentRangedState.RangeMin[attributeIndex] >= statement.Param || currentRangedState.RangeMax[attributeIndex] < statement.Param)
			{
				// Impossible, toss on our failure state.
				inOutStack.push(FailedCase);
			}
			else
			{
				IntVec4 newSuccessMax = currentRangedState.RangeMax;
				newSuccessMax[attributeIndex] = statement.Param - 1;

				IntVec4 newFailureMin = currentRangedState.RangeMin;
				newFailureMin[attributeIndex] = statement.Param;



				RangeState SuccessStateLow(nullptr, 0, currentRangedState.RangeMin, newSuccessMax);
				RangeState SuccessStateHigh(currentRangedState.CurrentWorkflow, currentRangedState.StatementIndex + 1, newFailureMin, currentRangedState.RangeMax);

				inOutStack.emplace(SuccessStateHigh); // Any failure / remainder.

				if (statement.ResultOnSuccess == ResultCode::RC_Jump)
				{
					std::unordered_set<Workflow>::const_iterator itFind = m_Workflows.find(Workflow(statement.JumpParam));
					assert(itFind != m_Workflows.end());
					SuccessStateLow.CurrentWorkflow = &(*itFind);
					inOutStack.emplace(SuccessStateLow);
				}
				else if (statement.ResultOnSuccess == ResultCode::RC_Accepted)
				{
					return GetRangeStateScore(SuccessStateLow);
				}
			}
		}
		else
		{
			assert(statement.Op == TestOp::OP_GT);

			Log("Splitting to Ranges based on statement '%s'", statement.RawString.c_str());
			if (currentRangedState.RangeMax[attributeIndex] < statement.Param)
			{
				// Impossible, toss on our failure state.
				inOutStack.push(FailedCase);
			}
			else
			{
				IntVec4 newSuccessMin = currentRangedState.RangeMin;
				newSuccessMin[attributeIndex] = statement.Param + 1;

				IntVec4 newFailureMax = currentRangedState.RangeMax;
				newFailureMax[attributeIndex] = statement.Param;

				RangeState SuccessStateLow(nullptr, 0, newSuccessMin, currentRangedState.RangeMax);
				RangeState SuccessStateHigh(currentRangedState.CurrentWorkflow, currentRangedState.StatementIndex + 1, currentRangedState.RangeMin, newFailureMax);

				inOutStack.emplace(SuccessStateHigh); // Any failure / remainder.

				if (statement.ResultOnSuccess == ResultCode::RC_Jump)
				{
					std::unordered_set<Workflow>::const_iterator itFind = m_Workflows.find(Workflow(statement.JumpParam));
					assert(itFind != m_Workflows.end());

					SuccessStateLow.CurrentWorkflow = &(*itFind);
					inOutStack.emplace(SuccessStateLow);
				}
				else if (statement.ResultOnSuccess == ResultCode::RC_Accepted)
				{
					return GetRangeStateScore(SuccessStateLow);
				}
			}
		}

		return 0;
	}

	virtual void ParseInput(FileStreamReader& fileReader) override
	{
		// Parse Input. Input never changes between parts of a problem.
		std::string line;
		std::vector<std::string> tokens;
		IntVec4 partTemp(0);
		while (!fileReader.IsEOF())
		{
			line = fileReader.ReadLine();
			if (line.size() == 0)
			{
				continue;
			}

			if (line[0] == '{')
			{
				StringUtil::SplitBy(line, ",", tokens);
				assert(tokens.size() == 4);
				uint8_t currentIndex = 0;
				for (const std::string& tok : tokens)
				{
					size_t eqIdx = tok.find('=');
					assert(eqIdx != std::string::npos);
					partTemp[currentIndex] = atoi(tok.substr(eqIdx + 1).c_str());
					++currentIndex;
				}

				m_Parts.push_back(partTemp);
			}
			else
			{
				m_Workflows.emplace(line.c_str());
			}
		}
	}

	virtual void PartOne(const AdventGUIContext& context) override
	{
		// Part One
		uint64_t partTotal = 0;
		for (const IntVec4& part : m_Parts)
		{
			if (ExecuteWorkflow(m_Workflows, part) == ResultCode::RC_Accepted)
			{
				partTotal += (uint64_t)(part.x + part.y + part.z + part.w);
			}
		}

		Log("Total Successful Parts: %" PRIu64, partTotal);
		// Done.
		AdventGUIInstance::PartOne(context);
	}

	virtual void PartTwo(const AdventGUIContext& context) override
	{	
		// Part Two
		std::unordered_set<Workflow>::const_iterator itFind = m_Workflows.find(Workflow(Hash::HashString32("in")));
		RangeState startState(&(*itFind), 0, IntVec4(1), IntVec4(4000));
		std::stack<RangeState> rangeStack;
		rangeStack.push(startState);
		uint64_t totalArea = 0;
		
		while (!rangeStack.empty())
		{
			const RangeState currentState = rangeStack.top();
			rangeStack.pop();

			totalArea += GetValueSpace(rangeStack, currentState );
		}
		
		Log("Perms %" PRIu64, totalArea);

		// Done.
		AdventGUIInstance::PartTwo(context);
	}

	std::unordered_set<Workflow> m_Workflows;
	std::vector<IntVec4> m_Parts;
};

int main()
{
	AdventGUIParams newParams;
	newParams.day = 19;
	newParams.year = 2023;
	newParams.puzzleTitle = "Aplenty";
	newParams.inputFilename = "input.txt";

	AdventGUIInstance::InstantiateAndExecute<AdventDay>(newParams);

	return 0;
}
