// main.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "AdventGUI/AdventGUI.h"

#include "ACUtils/Algorithm.h"
#include "ACUtils/IntVec.h"
#include "ACUtils/Hash.h"
#include "ACUtils/AABB.h"
#include "ACUtils/StringUtil.h"
#include <vector>
#include <queue>
#include <unordered_set>
#include "z3++.h"

class AdventDay : public AdventGUIInstance
{
public:
	AdventDay(const AdventGUIParams& params)
		: AdventGUIInstance(params)
	{};
private:
	struct Hail
	{
		Hail():Pos(0), Dir(0){}
		Hail(const Int64Vec3& _pos, const Int64Vec3& _dir): Pos(_pos), Dir(_dir){}
		Int64Vec3 Pos;
		Int64Vec3 Dir;
	};
	virtual void ParseInput(FileStreamReader& fileReader) override
	{
		// Parse Input. Input never changes between parts of a problem.
		std::string line;
		std::vector<std::string> tokens;

		int64_t dirV[3];
		int64_t posV[3];
		while (!fileReader.IsEOF())
		{
			line = fileReader.ReadLine();
			size_t split = line.find("@");
			assert(split != std::string::npos);
			std::string pos = line.substr(0, split);
			std::string dir = line.substr(split + 1);

			StringUtil::SplitBy(pos, ",", tokens);
			assert(tokens.size() == 3);
			posV[0] = StringUtil::AtoiI64(tokens[0].c_str());
			posV[1] = StringUtil::AtoiI64(tokens[1].c_str());
			posV[2] = StringUtil::AtoiI64(tokens[2].c_str());

			StringUtil::SplitBy(dir, ",", tokens);
			assert(tokens.size() == 3);
			dirV[0] = StringUtil::AtoiI64(tokens[0].c_str());
			dirV[1] = StringUtil::AtoiI64(tokens[1].c_str());
			dirV[2] = StringUtil::AtoiI64(tokens[2].c_str());

			m_Hail.emplace_back(Int64Vec3(posV[0], posV[1], posV[2]), Int64Vec3(dirV[0], dirV[1], dirV[2]));
		}
	}

	bool RayToRayXY(const Hail& LHS, const Hail& RHS, double& outIntersectX, double& outIntersectY) const
	{
		int64_t dx = RHS.Pos.x - LHS.Pos.x;
		int64_t dy = RHS.Pos.y - LHS.Pos.y;
		int64_t det = RHS.Dir.x * LHS.Dir.y - RHS.Dir.y * LHS.Dir.x;

		if (det != 0 )
		{
			double u = (double)(dy * RHS.Dir.x - dx * RHS.Dir.y) / (double)det;
			double v = (double)(dy * LHS.Dir.x - dx * LHS.Dir.y) / (double)det;

			if (u >= 0 && v >= 0)
			{
				double posX = (double)LHS.Pos.x;
				double posY = (double)LHS.Pos.y;
				double dirX = (double)LHS.Dir.x * u;
				double dirY = (double)LHS.Dir.y * u;

				outIntersectX = posX + dirX;
				outIntersectY = posY + dirY;

				return true;
			}
		}

		return false;
	}

	virtual void PartOne(const AdventGUIContext& context) override
	{
		// Part One
		Int64AABB2D testArea(Int64Vec2(200000000000000LL), Int64Vec2(400000000000000LL));

		double collX = 0.0;
		double collY = 0.0;

		uint32_t totalHits = 0;

		for (size_t i = 0; i < m_Hail.size() - 1; ++i)
		{
			const Hail& LHS = m_Hail[i];
			for (size_t j = i + 1; j < m_Hail.size(); ++j)
			{
				const Hail& RHS = m_Hail[j];
				if (RayToRayXY(LHS, RHS, collX, collY))
				{
					if (testArea.Contains(Int64Vec2((int64_t)collX, (int64_t)collY)))
					{
						++totalHits;
					}
				}
			}
		}

		Log("Total Hits: %u", totalHits);

		// Done.
		AdventGUIInstance::PartOne(context);
	}

	virtual void PartTwo(const AdventGUIContext& context) override
	{	
		// Part Two
		z3::context solverContext;
		z3::solver solver(solverContext);

		// For this we need to find out a Pos xyz and Velocity xyz that at time (t) equals:
		// pos.x + vel.x * t = hailPos.x + hailVel.x * t
		// pos.y + vel.y * t = hailPos.y + hailVel.y * t
		// pos.z + vel.z * t = hailPos.z + hailVel.z * t

		z3::expr px = solverContext.int_const("px");
		z3::expr py = solverContext.int_const("py");
		z3::expr pz = solverContext.int_const("pz");
		z3::expr vx = solverContext.int_const("vx");
		z3::expr vy = solverContext.int_const("vy");
		z3::expr vz = solverContext.int_const("vz");

		for (size_t i = 0; i < 3; ++i)
		{
			const Hail& current = m_Hail[i];
			z3::expr t = solverContext.int_const("t" + ('0' + i));
			z3::expr hx = solverContext.int_val(current.Pos.x);
			z3::expr hy = solverContext.int_val(current.Pos.y);
			z3::expr hz = solverContext.int_val(current.Pos.z);

			z3::expr hvx = solverContext.int_val(current.Dir.x);
			z3::expr hvy = solverContext.int_val(current.Dir.y);
			z3::expr hvz = solverContext.int_val(current.Dir.z);

			solver.add(t > 0);
			solver.add(hx + t * hvx == px + t * vx);
			solver.add(hy + t * hvy == py + t * vy);
			solver.add(hz + t * hvz == pz + t * vz);
		}

		z3::check_result rslt = solver.check();
		z3::model mdl = solver.get_model();

		z3::expr evalX = mdl.eval(px);
		z3::expr evalY = mdl.eval(py);
		z3::expr evalZ = mdl.eval(pz);

		Log("%lld", evalX.as_int64() + evalY.as_int64() + evalZ.as_int64());
		// Done.
		AdventGUIInstance::PartTwo(context);
	}
	std::vector<Hail> m_Hail;
};

int main()
{
	AdventGUIParams newParams;
	newParams.day = 24;
	newParams.year = 2023;
	newParams.puzzleTitle = "Never Tell Me The Odds";
	newParams.inputFilename = "input.txt";

	AdventGUIInstance::InstantiateAndExecute<AdventDay>(newParams);

	return 0;
}
