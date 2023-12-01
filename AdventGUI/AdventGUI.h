#pragma once

#include "ACUtils/FileStream.h"
#include "ACUtils/Enum.h"
#include "ACUtils/Vec.h"

#include "AdventGUIConsole.h"

#include <cassert>
#include <type_traits>

#define ACLOG(x, ...)  AdventGUIConsole::Get().Log(x, __VA_ARGS__);

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
	struct GLFWwindow* m_appWindow;
	AdventGUIContext m_context;
};