#include "AdventGUIConsole.h"

#include "ACUtils/Hash.h"

#include "imgui.h"
#include <cctype>

void OutOfConsoleMemory()
{
	AdventGUIConsole::Get().ClearLog();
	AdventGUIConsole::Get().Log("Console OOM. Wrapped and cleared log.");
}

int TextEditCallbackStub(ImGuiInputTextCallbackData* data)
{
	AdventGUIConsole::Get().ExecuteCommand((const char*)data);

	return 0;
}

AdventGUIConsole::AdventGUIConsole()
: m_Allocator(4 * 1024 * 1024, &OutOfConsoleMemory),
m_Enable(false),
m_AutoScroll(true),
m_ScrollToBottom(false),
m_HistoryIndex(-1)
{
	m_LogEntries.reserve(1024);
	m_Commands.reserve(16);
	memset(m_CommandInput, 0, 256);
}

AdventGUIConsole::~AdventGUIConsole()
{

}

void AdventGUIConsole::RegisterCommand(const char* command, GUIConsoleCommandCB callback)
{
	size_t commandHash = Hash::HashString64(command);
	assert(m_RegisterCommands.find(commandHash) == m_RegisterCommands.end()); // Make sure the command isn't already registered.
	m_RegisterCommands.emplace(std::make_pair(commandHash, callback));
}

void AdventGUIConsole::ExecuteCommand(const char* command)
{
	char commandLine[256] = { 0 };
	strcpy_s(commandLine, 256, command);
	size_t commandLength = strlen(commandLine);
	if (commandLength <= 0)
	{
		return;
	}

	const char* commandStart = commandLine;
	const char* commandEnd = commandLine + commandLength;
	// Quick trim of the ends.
	size_t paramsStart = 0;
	size_t paramsEnd = commandLength;
	while (commandStart != commandEnd && *commandStart == ' ')
	{
		++commandStart;
	}
	
	while (commandStart != commandEnd && *commandEnd == ' ')
	{
		--commandEnd;
	}

	// Now find our parameter start.
	const char* firstSpace = strchr(commandStart, ' ');
	if (firstSpace == nullptr)
	{
		// it's fine, there's no parameters.
	}
	else
	{
		++firstSpace;	
	}
	
	// Hash it
	size_t commandSize = commandEnd - commandStart;
	char commandStr[64] = { 0 };
	assert(commandSize < 64);
	memcpy(commandStr, commandStart, commandSize);
	commandStr[commandSize + 1] = '\0';

	// Call it if it's registered.
	size_t commandHash = Hash::HashString64(commandStr);
	HashToCommandCBMap::const_iterator itFind = m_RegisterCommands.find(commandHash);
	if (itFind != m_RegisterCommands.end())
	{
		(*itFind->second)(firstSpace);
	}

	Log("Command [%s] not found.", commandStr);
}

void AdventGUIConsole::Draw()
{
	if (!m_Enable)
	{
		return;
	}

	ImGui::SetNextWindowSize(ImGui::GetMainViewport()->Size);
	ImGui::SetNextWindowPos(ImVec2(0,0));
	if (!ImGui::Begin("Console"))
	{
		ImGui::End();
		return;
	}

	bool copy_to_clipboard = ImGui::Button("Copy");
	ImGui::SameLine();
	if (ImGui::Button("Clear"))
	{ 
		ClearLog(); 
	}
	ImGui::SameLine();
	ImGui::Separator();

	// Options menu
	if (ImGui::BeginPopup("Options"))
	{
		ImGui::Checkbox("Auto-scroll", &m_AutoScroll);
		ImGui::EndPopup();
	}
	ImGui::SameLine();
	// Options
	if (ImGui::Button("Options"))
	{ 
		ImGui::OpenPopup("Options");
	}
	ImGui::Separator();

	// Draw Log
	const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
	if (ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footer_height_to_reserve), false, ImGuiWindowFlags_HorizontalScrollbar))
	{
		if (ImGui::BeginPopupContextWindow())
		{
			if (ImGui::Selectable("Clear"))
			{
				ClearLog();
			}
			ImGui::EndPopup();
		}
	
		ImGuiListClipper clipper;
		clipper.Begin((int)m_LogEntries.size());
		while (clipper.Step())
		{
			for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; ++i)
			{
				const char* logLine = reinterpret_cast<char*>(m_Allocator.GetBaseOffset()) + m_LogEntries[i];
				
				// Future parsing code to pull style tokens...

				ImGui::TextUnformatted(logLine);
			}
		}

		if (copy_to_clipboard)
		{
			ImGui::LogFinish();
		}
		// Keep up at the bottom of the scroll region if we were already at the bottom at the beginning of the frame.
		// Using a scrollbar or mouse-wheel will take away from the bottom edge.
		if (m_ScrollToBottom || (m_AutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()))
			ImGui::SetScrollHereY(1.0f);
		m_ScrollToBottom = false;
	}

	ImGui::EndChild();
	ImGui::Separator();

	// Command-line
	bool reclaim_focus = false;
	ImGuiInputTextFlags input_text_flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_EscapeClearsAll | ImGuiInputTextFlags_CallbackCompletion | ImGuiInputTextFlags_CallbackHistory;
	if (ImGui::InputText("Input", m_CommandInput, 256, input_text_flags, &TextEditCallbackStub, m_CommandInput))
	{
		ExecuteCommand(m_CommandInput);
		reclaim_focus = true;
	}

	// Auto-focus on window apparition
	ImGui::SetItemDefaultFocus();
	if (reclaim_focus)
	{
		ImGui::SetKeyboardFocusHere(-1); // Auto focus previous widget
	}

	ImGui::End();

}

void AdventGUIConsole::ClearLog()
{
	m_LogEntries.clear();
}
