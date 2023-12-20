// main.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "AdventGUI/AdventGUI.h"

#include "ACUtils/IntVec.h"
#include "ACUtils/AABB.h"
#include "ACUtils/StringUtil.h"
#include <vector>
#include <cinttypes>

class AdventDay : public AdventGUIInstance
{
public:
	AdventDay(const AdventGUIParams& params) 
	: AdventGUIInstance(params), m_MapWidth(0)
	{};
private:
	virtual void ParseInput(FileStreamReader& fileReader) override
	{
		// Parse Input. Input never changes between parts of a problem.
		IntVec2 currentPoint(0, 0);
		Int64Vec2 partTwoCurrentPoint(0, 0);

		m_Bounds = IntAABB2D(currentPoint, 1);

		IntVec2 endPos = currentPoint;
		std::string line;
		std::vector<std::string> tokens;
		
		const IntVec2 directions[] = {  {0, 1},  // Up 
									    {0, -1}, // Down
										{-1, 0}, // Left
										{1, 0} };// Right

		const Int64Vec2 partTwoDirections[] = { {1, 0},  // Right 
										{0, -1}, // Down
										{-1, 0}, // Left
										{0, 1} };// Up
		
		uint32_t directionIndex = 0;
		while (!fileReader.IsEOF())
		{
			line = fileReader.ReadLine();
			StringUtil::SplitBy(line, " ", tokens);
			assert(tokens.size() == 3);
			assert(tokens[0].size() == 1);
			switch (*tokens[0].c_str())
			{
				case 'U' : directionIndex = 0; break;
				case 'D' : directionIndex = 1; break;
				case 'L' : directionIndex = 2; break;
				case 'R' : directionIndex = 3; break;
				default: assert(false); break;
			}

			int32_t length = atoi(tokens[1].c_str());
			m_totalWalk += length;
			assert(tokens[2].size() == 9);
			uint32_t color = 0;
			std::string hexStr = tokens[2].substr(2, 6).c_str();
			sscanf_s(hexStr.c_str(), "%x", &color);

			IntVec2 offset = directions[directionIndex] * length;
			m_Edges.emplace_back(currentPoint, currentPoint + offset, (uint32_t)color);
			
			m_partTwoTotalWalk += (color >> 4);
			Int64Vec2 partTwoOffset = partTwoDirections[(color & 3)] * (color >> 4);
			Int64Vec2 partTwoPoint = partTwoCurrentPoint + partTwoOffset;
			m_PointsP2.emplace_back(partTwoPoint);
			partTwoCurrentPoint = m_PointsP2.back();

			currentPoint = m_Edges.back().End;
			m_Bounds = m_Bounds.ExpandToContain(currentPoint);
		}
		assert(m_Edges.back().End == m_Edges.front().Start); // verify we loop back and aren't a degen.
	}

	void DrawEdges()
	{
		IntVec2 boundsSize = m_Bounds.GetSize();
		ImVec2 imCanvasMax(768.0f, 768.0f);
		ImVec2 canvasScalar(imCanvasMax.x / (float)boundsSize.x, imCanvasMax.y / (float)boundsSize.y);
		ImVec2 imCanvasSize((float)boundsSize.x * canvasScalar.x, (float)boundsSize.y * canvasScalar.y);
		ImGui::SetNextWindowSizeConstraints(imCanvasSize, imCanvasMax);

		if (!ImGui::Begin("Dig", nullptr, ImGuiWindowFlags_NoSavedSettings))
		{
			ImGui::End();
			return;
		}

		if (ImGui::BeginChild("drawcanvas", imCanvasSize, false, ImGuiWindowFlags_NoSavedSettings))
		{
			ImVec2 canvas_xy = ImGui::GetCursorScreenPos();
			ImVec2 canvas_sz = ImGui::GetContentRegionAvail();

			canvas_sz.x = std::max(canvas_sz.x, imCanvasSize.x);
			canvas_sz.y = std::max(canvas_sz.y, imCanvasSize.y);

			ImVec2 canvas_br(canvas_xy.x + canvas_sz.x, canvas_xy.y + canvas_sz.y);

			ImVec2 drawOrigin(canvas_xy.x, canvas_xy.y + (float)m_Bounds.GetSize().y + 200);

			ImGuiIO& io = ImGui::GetIO();
			ImDrawList* draw_list = ImGui::GetWindowDrawList();

			// Draw background
			ImColor bgColor = IM_COL32(50, 50, 50, 255);
			draw_list->AddRectFilled(canvas_xy, canvas_br, bgColor);
			draw_list->PushClipRect(canvas_xy, canvas_br, true);
			for(const Edge& edge : m_Edges)
			{
				ImVec2 p1 = ImVec2(drawOrigin.x + (float)edge.Start.x * canvasScalar.x, drawOrigin.y - (float)edge.Start.y * canvasScalar.y);
				ImVec2 p2 = ImVec2(drawOrigin.x + (float)edge.End.x * canvasScalar.x, drawOrigin.y - (float)edge.End.y * canvasScalar.y);
				draw_list->AddLine(p1, p2, edge.Color | IM_COL32_A_MASK);
			}
			draw_list->PopClipRect();
		}

		ImGui::EndChild();

		ImGui::End();
	}

	uint64_t GetArea32(const std::vector<IntVec2>& allPoints, uint64_t surfaceArea) const
	{
		// Shoelace
		int64_t areaSansPerimeter = 0LL;

		for (size_t i = 0; i < allPoints.size(); ++i) 
		{
			int j = (i + 1) % allPoints.size();
			areaSansPerimeter += (int64_t)(allPoints[i].x * allPoints[j].y) - (int64_t)(allPoints[j].x * allPoints[i].y);
		}

		// Pick's
		uint64_t interior =  (uint64_t)(abs(areaSansPerimeter) / 2);
		uint64_t perimeter = (surfaceArea / 2) + 1;
		return interior + perimeter;
	}

	uint64_t GetArea64(const std::vector<Int64Vec2>& allPoints, uint64_t surfaceArea) const
	{
		// Shoelace
		int64_t areaSansPerimeter = 0LL;

		for (size_t i = 0; i < allPoints.size(); ++i)
		{
			int j = (i + 1) % allPoints.size();
			areaSansPerimeter += (int64_t)(allPoints[i].x * allPoints[j].y) - (int64_t)(allPoints[j].x * allPoints[i].y);
		}

		// Pick's
		uint64_t interior = (uint64_t)(abs(areaSansPerimeter) / 2);
		uint64_t perimeter = (surfaceArea / 2) + 1;
		return interior + perimeter;
	}

	virtual void PartOne(const AdventGUIContext& context) override
	{
		// Part One
		//DrawEdges();

		std::vector<IntVec2> allPoints;

		allPoints.reserve(m_Edges.size());
		for (const Edge& edge : m_Edges)
		{
			allPoints.push_back(edge.End);
		}

		uint64_t cubicArea = GetArea32(allPoints, m_totalWalk);

		Log("Total Area = %" PRIu64, cubicArea);
		// Done.
		AdventGUIInstance::PartOne(context);
	}

	virtual void PartTwo(const AdventGUIContext& context) override
	{	
		// Part Two
		uint64_t cubicArea = GetArea64(m_PointsP2, m_partTwoTotalWalk);

		Log("Total Area = %" PRIu64, cubicArea);

		// Done.
		AdventGUIInstance::PartTwo(context);
	}

	struct Edge
	{
		Edge():Start(IntVec2::Zero), End(IntVec2::Zero), Color(0){}
		Edge(const IntVec2& _start, const IntVec2& _end, uint32_t _color):Start(_start), End(_end), Color(_color) {};
		IntVec2 Start;
		IntVec2 End;
		uint32_t Color;
	};
	uint32_t m_totalWalk;
	uint64_t m_partTwoTotalWalk;
	IntAABB2D m_Bounds;
	std::vector<Edge> m_Edges;
	std::vector<Int64Vec2> m_PointsP2;
	uint32_t m_MapWidth;
};

int main()
{
	AdventGUIParams newParams;
	newParams.day = 18;
	newParams.year = 2023;
	newParams.puzzleTitle = "Lavaduct Lagoon";
	newParams.inputFilename = "input.txt";

	AdventGUIInstance::InstantiateAndExecute<AdventDay>(newParams);

	return 0;
}
