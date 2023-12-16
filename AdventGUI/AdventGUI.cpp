#include "AdventGUI.h"

#include <iostream>
#include <GLFW/glfw3.h>
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include "ACUtils/Debug.h"

#include "AdventGUIConsole.h"

AdventGUIInstance* AdventGUIInstance::s_Instance = nullptr;

const Vec4 AdventGUIColor::Black = Vec4(0.0f, 0.0f, 0.0f, 1.0f);
const Vec4 AdventGUIColor::White = Vec4(1.0f, 1.0f, 1.0f, 1.0f);
const Vec4 AdventGUIColor::Red = Vec4(1.0f, 0.0f, 0.0f, 1.0f);
const Vec4 AdventGUIColor::Green = Vec4(0.0f, 1.0f, 0.0f, 1.0f);
const Vec4 AdventGUIColor::Blue = Vec4(0.0f, 0.0f, 1.0f, 1.0f);
const Vec4 AdventGUIColor::Yellow = Vec4(0.0f, 1.0f, 1.0f, 1.0f);
const Vec4 AdventGUIColor::Orange = Vec4(1.0f, 0.0f, 1.0f, 1.0f);
const Vec4 AdventGUIColor::Purple = Vec4(1.0f, 1.0f, 0.0f, 1.0f);

static void glfw_error_callback(int error, const char* errorDescription)
{
	fprintf(stderr, "GLFW Error (%d): %s\n", error, errorDescription);
}

static void* ImGuiFrameAlloc(size_t size, void* /*userData*/)
{
	return AdventGUIInstance::Get()->GetImGuiFrameAllocator().Alloc(size);
}

static void ImGuiFrameFree(void* /*ptr*/, void* /*userData*/)
{
	// No need.
}

void AdventGUIInstance::RequestExit(bool exit)
{
	if (m_appWindow)
	{
		glfwSetWindowShouldClose(m_appWindow, exit ? GLFW_TRUE : GLFW_FALSE);
	}
}

void AdventGUIInstance::OnKeyAction(struct GLFWwindow* window, int key, int scancode, int action, int mods)
{
	//ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);

	if (window != m_appWindow)
	{
		return;
	}

	// Check for escape and exit.
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		RequestExit(true);
	}
	else if (key == GLFW_KEY_GRAVE_ACCENT && action == GLFW_RELEASE)
	{
		AdventGUIConsole::Get().ToggleConsole();
	}
	else if (key == GLFW_KEY_F1 && action == GLFW_RELEASE)
	{
		ToggleImGuiDemoWindow();
	}
}

AdventGUIInstance::AdventGUIInstance(const AdventGUIParams& params)
: m_params(params),
m_imguiFrameAllocator(1 * 1024 * 1024),
m_appLifetime(0.0),
m_lastTimeStamp(0.0),
m_showImGuiDemo(false)
{
	InternalCreate();
}

void input_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	AdventGUIInstance::Get()->OnKeyAction(window, key, scancode, action, mods);
}

void AdventGUIInstance::InternalCreate()
{
	glfwSetErrorCallback(glfw_error_callback);
	if (!glfwInit())
	{
		printf("Failed to Init GLFW. Check any error messages above.");
		assert(false);
	}

	const char* glsl_version = "#version 130";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	if ((m_params.options & AdventGUIOptions::AGO_ShowWindowTitle) == AdventGUIOptions::AGO_None)
	{
		glfwWindowHint(GLFW_DECORATED, 0);
	}

	char titleBuffer[256] = { 0 };

	if (m_params.puzzleTitle)
	{
		sprintf_s(titleBuffer, 256, "Advent of Code %u, Day %u : %s", m_params.year, m_params.day, m_params.puzzleTitle);
	}
	else
	{
		sprintf_s(titleBuffer, 256, "Advent of Code %u, Day %u", m_params.year, m_params.day);
	}

	m_appWindow = glfwCreateWindow(m_params.windowWidth, m_params.windowHeight, titleBuffer, nullptr, nullptr);
	if (!m_appWindow)
	{
		printf("Failed to create app window. Check for any error messages above.");
		assert(false);
	}

	glfwMakeContextCurrent(m_appWindow);
	glfwSwapInterval(1);
	glfwSetKeyCallback(m_appWindow, &input_callback);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& ioSys = ImGui::GetIO();
	ioSys.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	ioSys.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

	ImGui::SetFrameAllocatorFunctions(ImGuiFrameAlloc, ImGuiFrameFree);

	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL(m_appWindow, true);
	ImGui_ImplOpenGL3_Init(glsl_version);

	AdventGUIConsole& console = AdventGUIConsole::Get();
	console.Disable();

	if ((m_params.options & AdventGUIOptions::AGO_StartWithConsoleOpen) != AdventGUIOptions::AGO_None)
	{
		console.Enable();
	}

	console.Log("AdventGUI Initialized.");
}

void AdventGUIInstance::InternalDestroy()
{
	if (!m_appWindow)
	{
		return;
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();

	ImGui::DestroyContext();

	glfwDestroyWindow(m_appWindow);
	m_appWindow = nullptr;
	glfwTerminate();
}

void AdventGUIInstance::PollEvents()
{
	glfwPollEvents();
}

void AdventGUIInstance::BeginFrame()
{
	m_imguiFrameAllocator.Reset();

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();

	ImGui::NewFrame();

	int32_t windowHeight = 0;
	int32_t windowWidth = 0;

	glfwGetFramebufferSize(m_appWindow, &windowWidth, &windowHeight);
	glViewport(0, 0, windowWidth, windowHeight);
	glClearColor(m_params.clearColor.x, m_params.clearColor.y, m_params.clearColor.z, m_params.clearColor.w);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
}

void AdventGUIInstance::DoFrame()
{
	m_appLifetime = glfwGetTime();
	AdventGUIContext context;
	context.deltaTime = m_appLifetime - m_lastTimeStamp;
	
	if (HasExecFlags(AdventExecuteFlags::AEF_PartOne))
	{
		PartOne(context);
	}

	if (HasExecFlags(AdventExecuteFlags::AEF_PartTwo))
	{
		PartTwo(context);
	}

	if (m_showImGuiDemo)
	{
		ImGui::ShowDemoWindow();
	}

	m_lastTimeStamp = m_appLifetime;
}

void AdventGUIInstance::PartOne(const AdventGUIContext& context)
{
	// Transition to Part Two
	SetExecFlags(AdventExecuteFlags::AEF_PartTwo);
}

void AdventGUIInstance::PartTwo(const AdventGUIContext& context)
{
	// Request exit
	SetExecFlags(AdventExecuteFlags::AEF_None);
}

void AdventGUIInstance::EndFrame()
{
	/*
		Render any geo...
	*/

	// Draw Console
	AdventGUIConsole::Get().Draw();

	// Prepare IMGui data stream.
	ImGui::Render();

	// Render IMGui
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	glfwSwapBuffers(m_appWindow);
}

bool AdventGUIInstance::ShouldExit()
{
	if (m_appWindow)
	{
		return glfwWindowShouldClose(m_appWindow) != 0;
	}

	return true;
}


