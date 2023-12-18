#pragma once

#include "ACUtils/FileStream.h"
#include "ACUtils/Enum.h"
#include "ACUtils/Memory.h"
#include "ACUtils/Vec.h"

#include "AdventGUIConsole.h"

#include <cassert>
#include <type_traits>

enum class AdventGUIOptions
{
	AGO_None = 0,						   // Nothing special
	AGO_StartWithConsoleOpen = 1 << 0,     // Start with the console/logging showing
	AGO_EnableFixedWidthConsole = 1 << 1,  // Set console logs to be a fixed number of characters wide.
	AGO_ShowWindowTitle = 1 << 2,		   // If true, we'll set the title of the window to the year / day. Otherwise, it'll be a borderless window.
};

DECLARE_ENUM_BITFIELD_OPERATORS(AdventGUIOptions);

enum class AdventExecuteFlags
{
	AEF_None = 0,
	AEF_PartOne = 1 << 0,
	AEF_PartTwo = 1 << 1,
};

DECLARE_ENUM_BITFIELD_OPERATORS(AdventExecuteFlags);

struct AdventGUIParams
{
	uint32_t windowHeight = 720;
	uint32_t windowWidth = 1280;
	uint32_t fixedConsoleWidth = 300; // Ignored unless AGO_EnabledFixedWidthConsole is enabled.
	AdventGUIOptions options = AdventGUIOptions::AGO_None;
	AdventExecuteFlags exec = AdventExecuteFlags::AEF_PartOne;
	uint32_t day = 1;
	uint32_t year = 2023;
	const char* puzzleTitle = nullptr; 
	const char* inputFilename = nullptr;
	Vec4 clearColor = Vec4(0.45f, 0.55f, 0.60f, 1.00f); // Backbuffer Clear color
};

namespace AdventGUIColor
{
	extern const Vec4 Black;
	extern const Vec4 White;
	extern const Vec4 Red;
	extern const Vec4 Blue;
	extern const Vec4 Green;
	extern const Vec4 Yellow;
	extern const Vec4 Orange;
	extern const Vec4 Purple;
}

#define ACLOG(x, ...)  AdventGUIInstance::Get().Log(x, __VA_ARGS__);

class AdventGUIInstance
{
public:
	struct AdventGUIContext
	{
		double deltaTime = 0.0; // In seconds
	};

	template<class T, typename ...Args>
	static void InstantiateAndExecute(const AdventGUIParams& params, Args&&... args)
	{
		static_assert(std::is_base_of<AdventGUIInstance, T>::value, "Class must inherit from AdventGUIInstance");
		T* newInstance = new T(params, std::forward(args)...);
		
		s_Instance = newInstance->As<AdventGUIInstance>();

		if (s_Instance->GetInputFileName())
		{
			FileStreamReader inputFile(s_Instance->GetInputFileName());
			s_Instance->ParseInput(inputFile);
		}

		// Begin exec loop
		while (!s_Instance->ShouldExit())
		{
			s_Instance->PollEvents();

			s_Instance->BeginFrame();

			s_Instance->DoFrame();

			s_Instance->EndFrame();
		}
		s_Instance->InternalDestroy();

		delete s_Instance;
		s_Instance = nullptr;
	}

	static AdventGUIInstance* Get()
	{
		return s_Instance;
	}

	template<typename T>
	T* As() { return static_cast<T*>(this); }

	template<typename T>
	const T* As() const { return static_cast<const T*>(this); }

	virtual ~AdventGUIInstance() {};

	bool HasGUIOption(AdventGUIOptions options) const { return (m_params.options & options) != AdventGUIOptions::AGO_None; }
	void SetExecFlags(AdventExecuteFlags execFlags) { m_params.exec = execFlags; }
	bool HasExecFlags(AdventExecuteFlags execFlags) const { return (m_params.exec & execFlags) != AdventExecuteFlags::AEF_None; }

	void RequestExit(bool exit);
	void OnKeyAction(struct GLFWwindow* window, int key, int scancode, int action, int mods);

	void Log(const char* fmt, ...) const
	{
		char appendedLog[1024] = { 0 };
		va_list args;
		va_start(args, fmt);
		vsprintf_s(appendedLog, 1024, fmt, args);
		va_end(args);

		AdventGUIConsole::Get().Log("[%8f] %s", m_appLifetime, appendedLog);
	}

	void VLog(const char* fmt, va_list args) const
	{
		char appendedLog[1024] = { 0 };
		vsprintf_s(appendedLog, 1024, fmt, args);

		AdventGUIConsole::Get().Log("[%8f] %s", m_appLifetime, appendedLog);
	}

	const char* GetInputFileName() const { return m_params.inputFilename; }
	Memory::LinearAllocator& GetImGuiFrameAllocator() { return m_imguiFrameAllocator; } 

	void ToggleImGuiDemoWindow() { m_showImGuiDemo = !m_showImGuiDemo; }
protected:
	AdventGUIInstance(const AdventGUIParams& params);

	virtual void ParseInput(FileStreamReader& fileReader) = 0;
	virtual void PartOne(const AdventGUIContext& context);
	virtual void PartTwo(const AdventGUIContext& context);
private:
	
	void InternalCreate();
	void InternalDestroy();
	void PollEvents();
	void BeginFrame();
	void DoFrame();
	void EndFrame();
	bool ShouldExit();

	static AdventGUIInstance* s_Instance;

	AdventGUIParams m_params;
	Memory::LinearAllocator m_imguiFrameAllocator;
	struct GLFWwindow* m_appWindow;
	AdventGUIContext m_context;
	double m_appLifetime;
	double m_lastTimeStamp;
	bool m_showImGuiDemo;
};