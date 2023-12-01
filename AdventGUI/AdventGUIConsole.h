#pragma once

#include "ACUtils/Memory.h"

#include "imgui.h"

#include <cstdint>
#include <algorithm>
#include <stdarg.h>
#include <vector>
#include <cstdio>
#include <unordered_map>

class AdventGUIConsole
{
public:
	static AdventGUIConsole& Get()
	{
		static AdventGUIConsole s_Instance;
		return s_Instance;
	}

	typedef void (*GUIConsoleCommandCB)(const char* args);

	void Log(const char* str, ...)
	{
		constexpr int32_t MAX_LOG_LINE_SIZE = 512;
		char printBuffer[MAX_LOG_LINE_SIZE];
		va_list args;
		va_start(args, str);
		int totalChars = std::min(vsprintf_s(printBuffer, 512, str, args), MAX_LOG_LINE_SIZE - 1);
		assert(totalChars > 0);
		va_end(args);

		// Just to be safe, the docs are weird about if the string is exactly the size of the buffer.
		printBuffer[MAX_LOG_LINE_SIZE - 1] = '\0';

		void* logCopy = m_Allocator.Alloc(totalChars + 1);
		uintptr_t lineOffset = (uintptr_t)logCopy - m_Allocator.GetBaseOffset();
		m_LogEntries.emplace_back(lineOffset);
		memcpy(logCopy, printBuffer, totalChars + 1);

		// Echo to stdout
		printf_s("%s\n", printBuffer);
	};

	void RegisterCommand(const char* command, GUIConsoleCommandCB callback);
	void ExecuteCommand(const char* command);
	void Enable() { m_Enable = true; }
	void Disable() { m_Enable = false; }
	void ToggleConsole() { m_Enable = !m_Enable; }
	void Draw();
	void ClearLog();
private:
	AdventGUIConsole();
	~AdventGUIConsole();
	
	typedef std::unordered_map<size_t, GUIConsoleCommandCB> HashToCommandCBMap;

	Memory::Ringbuffer       m_Allocator;
	std::vector<uintptr_t>   m_LogEntries;
	std::vector<const char*> m_Commands;
	HashToCommandCBMap	     m_RegisterCommands;
	bool			         m_Enable;
	bool                     m_AutoScroll;
	bool                     m_ScrollToBottom;
	int						 m_HistoryIndex;
	char					 m_CommandInput[256];
};