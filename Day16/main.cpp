// main.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "AdventGUI/AdventGUI.h"

#include "ACUtils/IntVec.h"
#include "ACUtils/Vec.h"
#include "ACUtils/StringUtil.h"
#include <unordered_set>
#include <vector>
#include <cinttypes>

class AdventDay : public AdventGUIInstance
{
public:
	AdventDay(const AdventGUIParams& params) 
	: AdventGUIInstance(params),
	m_lastStableStep(0),
	m_MapWidth(0),
	m_MapHeight(0),
	m_ActiveIndex(0),
	m_AutoStep(false)
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
			}
			assert(m_MapWidth == (uint32_t)line.size());
			m_Map.insert(m_Map.end(), line.begin(), line.end());
		}

		m_MapHeight = (uint32_t)m_Map.size() / m_MapWidth;

		m_ActiveLights[0].insert(IntVec4(-1, 0, 1, 0)); // (0,0) going (1,0)
		//m_TouchedTiles.insert(IntVec2(0, 0));
	}

	void Simulate(std::unordered_set<IntVec4>& InPosVel, std::unordered_set<IntVec4>& OutPosVel, std::unordered_set<IntVec2>& outTouchedTiles)
	{
		std::vector<IntVec4> newBeams;
		OutPosVel.clear();
		std::unordered_set<IntVec4>::const_iterator itIter = InPosVel.begin();
		while(itIter != InPosVel.end())
		{
			IntVec4 PosVel = *itIter;

			// Increment pos
			PosVel.x += PosVel.z;
			PosVel.y += PosVel.w;

			if (PosVel.x < 0 || PosVel.y < 0 || PosVel.x >= (int32_t)m_MapWidth || PosVel.y >= (int32_t)m_MapHeight)
			{
				// Out of bounds.
				++itIter;
				continue;
			}

			outTouchedTiles.insert(PosVel.XY());

			// Update velocities
			switch (m_Map[PosVel.y * m_MapWidth + PosVel.x])
			{
				case '|':
				{
					if (PosVel.z != 0) // If we have horizontal velocity
					{
						// Split and reflect in both directions.
						IntVec4 split = PosVel;
						split.w = PosVel.z;
						split.z = 0;

						OutPosVel.insert(split);

						PosVel.w = -PosVel.z;
						PosVel.z = 0;
						
					}
				}
				break;
				case '-':
				{
					if (PosVel.w != 0) // if we have vertical velocity
					{
						// Split and reflect in both directions.
						IntVec4 split = PosVel;
						split.z = PosVel.w;
						split.w = 0;

						OutPosVel.insert(split);

						PosVel.z = -PosVel.w;
						PosVel.w = 0;
					}
				}
				break;
				case '/':
				{
					if (PosVel.z != 0)
					{
						int32_t tmp = PosVel.z;
						PosVel.z = PosVel.w;
						PosVel.w = -tmp;
					}
					else
					{
						int32_t tmp = PosVel.w;
						PosVel.w = PosVel.z;
						PosVel.z = -tmp;
					}
				}
				break;
				case '\\':
				{
					int32_t tmp = PosVel.w;
					PosVel.w = PosVel.z;
					PosVel.z = tmp;
				}
				break;
				default:
				break;
			}
			OutPosVel.insert(PosVel);
			++itIter;
		}
	}

	bool DrawCaveState(std::unordered_set<IntVec4>& InOutPosVel, std::unordered_set<IntVec2>& outTouchedTiles)
	{
		bool shouldStep = false;

		ImGui::SetNextWindowSizeConstraints(ImVec2(768, 768), ImVec2(1024.0f, 1024.0f));

		if (!ImGui::Begin("Cave", nullptr, ImGuiWindowFlags_NoSavedSettings))
		{
			ImGui::End();
			return shouldStep;
		}

		if (ImGui::BeginChild("drawcanvas", ImVec2(768.0f, 768.0f), false, ImGuiWindowFlags_NoSavedSettings))
		{
			ImVec2 canvas_xy = ImGui::GetCursorScreenPos();
			ImVec2 canvas_sz = ImGui::GetContentRegionAvail();

			canvas_sz.x = std::max(canvas_sz.x, 768.0f);
			canvas_sz.y = std::max(canvas_sz.y, 768.0f);

			ImVec2 canvas_br(canvas_xy.x + canvas_sz.x, canvas_xy.y + canvas_sz.y);

			ImVec2 scrollOffset(0.0f, 0.0f);


			ImColor bgColor = IM_COL32(50, 50, 50, 255);
			ImColor emptyTileColor = IM_COL32(148, 148, 148, 255);
			ImColor energizedColor = IM_COL32(255, 255, 193, 255);
			ImColor occupiedColor = IM_COL32(255, 255, 255, 255);

			ImGuiIO& io = ImGui::GetIO();
			ImDrawList* draw_list = ImGui::GetWindowDrawList();

			// Draw background
			draw_list->AddRectFilled(canvas_xy, canvas_br, bgColor);
			draw_list->PushClipRect(canvas_xy, canvas_br, true);

			//ImGui::InvisibleButton("canvas", canvas_sz, ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight);
			const bool is_hovered = ImGui::IsItemHovered(); // Hovered
			const bool is_active = ImGui::IsItemActive();   // Held
			//const ImVec2 origin(canvas_p0.x + scrolling.x, canvas_p0.y + scrolling.y); // Lock scrolled origin
			//const ImVec2 mouse_pos_in_canvas(io.MousePos.x - origin.x, io.MousePos.y - origin.y);

			Vec2 tileSize(16.0f, 16.0f);
			Vec2 tileStart(canvas_xy.x, canvas_xy.y);
			Vec2 tileEnd(0.0f, 0.0f);
			Vec2 iconPadding(2.0f);

			uint32_t tileState = 0;
			const ImColor tileStates[] = { emptyTileColor, energizedColor, occupiedColor };

			for (uint32_t y = 0; y < m_MapHeight; ++y)
			{
				tileStart.x = canvas_xy.x;
				tileEnd = tileStart + tileSize;
				for (uint32_t x = 0; x < m_MapWidth; ++x)
				{
					IntVec2 cavePos(x, y);

					tileState = 0;

					std::unordered_set<IntVec4>::const_iterator itFind = std::find_if(InOutPosVel.begin(), InOutPosVel.end(), [&](const IntVec4& RHS) {return RHS.XY() == cavePos; });
					if (itFind != InOutPosVel.end())
					{
						tileState = 2;
					}
					else if (outTouchedTiles.find(cavePos) != outTouchedTiles.end())
					{
						tileState = 1;
					}

					draw_list->AddRectFilled(ImVec2(tileStart.x, tileStart.y), ImVec2(tileEnd.x, tileEnd.y), tileStates[tileState]);

					Vec2 tileCenter = tileStart + (tileSize * 0.5f);

					switch (m_Map[y * m_MapWidth + x])
					{
						default:
						case '.': break;
						case '-':
						{
							ImVec2 dashStart(tileStart.x + iconPadding.x, tileCenter.y);
							ImVec2 dashEnd(tileEnd.x - iconPadding.x, tileCenter.y);
							draw_list->AddLine(dashStart, dashEnd, IM_COL32(0,0,0,255), 2.0f);
						}
						break;
						case '|':
						{
							ImVec2 dashStart(tileCenter.x, tileStart.y + iconPadding.y);
							ImVec2 dashEnd(tileCenter.x, tileEnd.y - iconPadding.y);
							draw_list->AddLine(dashStart, dashEnd, IM_COL32(0, 0, 0, 255), 2.0f);
						}
						break;
						case '/':
						{
							ImVec2 dashStart(tileEnd.x - iconPadding.x, tileStart.y + iconPadding.y);
							ImVec2 dashEnd(tileStart.x + iconPadding.x, tileEnd.y - iconPadding.y);
							draw_list->AddLine(dashStart, dashEnd, IM_COL32(0, 0, 0, 255), 2.0f);
						}
						break;
						case '\\':
						{
							ImVec2 dashStart(tileStart.x + iconPadding.x, tileStart.y + iconPadding.y);
							ImVec2 dashEnd(tileEnd.x - iconPadding.x, tileEnd.y - iconPadding.y);
							draw_list->AddLine(dashStart, dashEnd, IM_COL32(0, 0, 0, 255), 2.0f);
						}
						break;
					}

					// Occupied, draw velocity.
					if (tileState == 2)
					{
						float triangleSize = 8.0f;
						Vec2 trianglePoints[3];
						if (itFind->z == 0) // Vertical 
						{
							if (itFind->w > 0) // v
							{
								trianglePoints[0].x = tileCenter.x;
								trianglePoints[0].y = tileCenter.y + (triangleSize * 0.5f);
								trianglePoints[1].x = tileEnd.x - iconPadding.x;
								trianglePoints[1].y = tileStart.y + iconPadding.y;
								trianglePoints[2] = tileStart + iconPadding;
							}
							else // ^
							{
								trianglePoints[0].x = tileCenter.x;
								trianglePoints[0].y = tileCenter.y - (triangleSize * 0.5f);
								trianglePoints[1] = tileEnd - iconPadding;
								trianglePoints[2].x = tileStart.x + iconPadding.x;
								trianglePoints[2].y = tileEnd.y - iconPadding.y;
							}

						}
						else // Horizontal
						{
							if (itFind->z > 0) // >
							{
								trianglePoints[0] = tileStart + iconPadding;
								trianglePoints[1].x = tileCenter.x + triangleSize * 0.5f;
								trianglePoints[1].y = tileCenter.y;
								trianglePoints[2].x = tileStart.x + iconPadding.x;
								trianglePoints[2].y = tileEnd.y - iconPadding.y;
							}
							else // <
							{
								trianglePoints[0].x = tileEnd.x - iconPadding.x;
								trianglePoints[0].y = tileStart.y + iconPadding.y;
								trianglePoints[1].x = tileCenter.x - triangleSize * 0.5f;
								trianglePoints[1].y = tileCenter.y;
								trianglePoints[2] = tileEnd - iconPadding;
							}

						}
						draw_list->AddTriangleFilled(ImVec2(trianglePoints[0].x, trianglePoints[0].y), ImVec2(trianglePoints[1].x, trianglePoints[1].y), ImVec2(trianglePoints[2].x, trianglePoints[2].y), ImColor(255, 51, 51, 255));
					}
					tileStart.x += tileSize.x;
					tileEnd.x += tileSize.x;
				}

				tileStart.y += tileSize.y;
			}

			// Draw Grid
			ImVec2 lineStart(canvas_xy.x, canvas_xy.y);
			ImVec2 lineSize((float)m_MapWidth * tileSize.x, (float)m_MapHeight * tileSize.y);

			for (uint32_t x = 0; x < m_MapWidth; ++x)
			{
				draw_list->AddLine(lineStart, ImVec2(lineStart.x, lineStart.y + lineSize.y), IM_COL32(0, 0, 0, 255));
				lineStart.x += tileSize.x;
			}

			lineStart.x = canvas_xy.x;
			for (uint32_t y = 0; y < m_MapHeight; ++y)
			{
				draw_list->AddLine(lineStart, ImVec2(lineStart.x + lineSize.x, lineStart.y), IM_COL32(0, 0, 0, 255));
				lineStart.y += tileSize.y;
			}

			// Draw Velocities


			draw_list->PopClipRect();

		}
		ImGui::EndChild();

		ImGui::SameLine();
		ImGui::BeginChild("Options", ImVec2(192.0f, 128.0f), false, ImGuiWindowFlags_NoSavedSettings);
		ImGui::Text("Simulation Steps: %u", m_totalSimSteps);
		ImGui::Text("Energized Tiles: %zd", outTouchedTiles.size());
		ImGui::Checkbox("Auto-Step", &m_AutoStep);
		ImGui::BeginDisabled(m_AutoStep);
		if (ImGui::Button("Step"))
		{
			shouldStep = true;
		}
		ImGui::EndDisabled();

		ImGui::EndChild();
		ImGui::End();

		return shouldStep || m_AutoStep;
	}

	virtual void PartOne(const AdventGUIContext& context) override
	{
		// Part One
		size_t previousTouchedTiles = m_TouchedTiles.size();
		bool forceStep = DrawCaveState(m_ActiveLights[m_ActiveIndex], m_TouchedTiles);


		if ( forceStep || m_AutoStep )
		{
			Simulate(m_ActiveLights[m_ActiveIndex], m_ActiveLights[m_ActiveIndex ^ 1], m_TouchedTiles);
			++m_totalSimSteps;
			m_ActiveIndex ^= 1;
		}

		if (previousTouchedTiles == m_TouchedTiles.size() )
		{
			++m_lastStableStep;
			Log("Stable Step Count: %zd, Actives: %zd for %u frames.", m_TouchedTiles.size(), m_ActiveLights[m_ActiveIndex].size(), m_lastStableStep);
			if (m_lastStableStep > 200)
			{
				m_AutoStep = false;
			}
		}
		else
		{
			Log("New Step Count: %zd, Actives: %zd", m_TouchedTiles.size(), m_ActiveLights[m_ActiveIndex].size());			
			m_lastStableStep = 0;
		}

		if (m_AutoStep && m_lastStableStep >= 200)
		{
			Log("Total touched tiles = %zd", m_TouchedTiles.size());

			// Done.
			AdventGUIInstance::PartOne(context);
		}
	}

	struct TestResults
	{
		TestResults() : Pos(0), Dir(0), TotalEnergy(0){};
		IntVec2 Pos;
		IntVec2 Dir;
		size_t TotalEnergy;
	};

	void GetEnergyForTest(TestResults& outResults)
	{
		m_ActiveIndex = 0;
		m_ActiveLights[0].clear();
		m_ActiveLights[1].clear();
		m_lastStableStep = 0;
		m_totalSimSteps = 0;
		m_TouchedTiles.clear();

		m_ActiveLights[m_ActiveIndex].insert(IntVec4(outResults.Pos.x, outResults.Pos.y, outResults.Dir.x, outResults.Dir.y));

		while (true)
		{
			size_t previousTouchedTiles = m_TouchedTiles.size();
			Simulate(m_ActiveLights[m_ActiveIndex], m_ActiveLights[m_ActiveIndex ^ 1], m_TouchedTiles);
			++m_totalSimSteps;
			m_ActiveIndex ^= 1;

			if (previousTouchedTiles == m_TouchedTiles.size())
			{
				++m_lastStableStep;
				//Log("Stable Step Count: %zd, Actives: %zd for %u frames.", m_TouchedTiles.size(), m_ActiveLights[m_ActiveIndex].size(), m_lastStableStep);
				if (m_lastStableStep > 200)
				{
					outResults.TotalEnergy = m_TouchedTiles.size();
					break;
				}
			}
		}
	}

	virtual void PartTwo(const AdventGUIContext& context) override
	{	
		// Part Two
		const IntVec2 directions[] = {{1, 0}, {0, 1}, {-1, 0}, {0, -1}};
		std::vector<TestResults> allTests;

		// All top/bottom rows, heading down/up.
		for (uint32_t i = 0; i < m_MapWidth; ++i)
		{
			// Up heading down
			TestResults resultsA;
			resultsA.Pos = IntVec2(i, -1);
			resultsA.Dir = directions[1];

			GetEnergyForTest(resultsA);

			Log("Test [%d, %d] with Direction [%d, %d] came back with %zd total energy.", resultsA.Pos.x, resultsA.Pos.y, resultsA.Dir.x, resultsA.Dir.y, resultsA.TotalEnergy);

			allTests.push_back(resultsA);

			// Down heading up
			TestResults resultsB;
			resultsB.Pos = (i, m_MapHeight);
			resultsB.Dir = directions[3];

			GetEnergyForTest(resultsB);

			Log("Test [%d, %d] with Direction [%d, %d] came back with %zd total energy.", resultsB.Pos.x, resultsB.Pos.y, resultsB.Dir.x, resultsB.Dir.y, resultsB.TotalEnergy);

			allTests.push_back(resultsB);
		}

		// East/West Rows
		// All top/bottom rows, heading down/up.
		for (uint32_t i = 0; i < m_MapHeight; ++i)
		{
			// Up heading down
			TestResults resultsA;
			resultsA.Pos = IntVec2(-1, i);
			resultsA.Dir = directions[0];

			GetEnergyForTest(resultsA);

			Log("Test [%d, %d] with Direction [%d, %d] came back with %zd total energy.", resultsA.Pos.x, resultsA.Pos.y, resultsA.Dir.x, resultsA.Dir.y, resultsA.TotalEnergy);

			allTests.push_back(resultsA);

			// Down heading up
			TestResults resultsB;
			resultsB.Pos = (m_MapWidth, i);
			resultsB.Dir = directions[2];

			GetEnergyForTest(resultsB);

			Log("Test [%d, %d] with Direction [%d, %d] came back with %zd total energy.", resultsB.Pos.x, resultsB.Pos.y, resultsB.Dir.x, resultsB.Dir.y, resultsB.TotalEnergy);

			allTests.push_back(resultsB);
		}

		std::sort(allTests.begin(), allTests.end(),[](const TestResults& LHS, const TestResults& RHS){ return LHS.TotalEnergy > RHS.TotalEnergy; });

		Log("Most Energy [%zd] Least Energy [%zd]", allTests.front().TotalEnergy, allTests.back().TotalEnergy);

		// Done.
		AdventGUIInstance::PartTwo(context);
	}

	uint32_t m_lastStableStep;
	uint32_t m_totalSimSteps;
	std::vector<char> m_Map;
	uint32_t m_MapWidth;
	uint32_t m_MapHeight;
	uint32_t m_ActiveIndex;
	std::unordered_set<IntVec4> m_ActiveLights[2];
	std::unordered_set<IntVec2> m_TouchedTiles;
	bool m_AutoStep;
	float m_SimHz;
};

int main()
{
	AdventGUIParams newParams;
	newParams.day = 16;
	newParams.year = 2023;
	newParams.puzzleTitle = "The Floor Will Be Lava";
	newParams.inputFilename = "input.txt";

	AdventGUIInstance::InstantiateAndExecute<AdventDay>(newParams);

	return 0;
}
