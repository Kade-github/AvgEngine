/*
	Developer: Kade
	Copyright 2023 - Kade-Github

	All source code (besides libraries) are licensed to the Crystallium Team.
*/

#pragma comment(lib,"x64_static\\opengl.lib")
#pragma comment(lib,"x64_static\\glfw3_mt.lib")
#pragma comment(lib,"x64_static\\OpenAL32.lib")

#ifdef _DEBUG
#pragma comment(lib,"x64_static\\Debug\\AvgEngine.lib")
#else
#pragma comment(lib,"x64_static\\Release\\AvgEngine.lib")
#endif

#include <filesystem>
#include <thread>
#include <windows.h>

#include <AvgEngine/External/ImGui/ImGUIHelper.h>
#include <Glad/glad.h>
#include <AvgEngine/Utils/Logging.h>
#include <AvgEngine/EventManager.h>
#include <AvgEngine/External/OpenAL/AL.h>
#include <AvgEngine/Game.h>

#include "Demo/DemoMenu.h"

size_t write_data(void* ptr, size_t size, size_t nmemb, FILE* stream) {
	size_t written = fwrite(ptr, size, nmemb, stream);
	return written;
}

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	PSTR lpCmdLine, INT nCmdShow)
{
#ifdef  _DEBUG
	AllocConsole();
	freopen("conout$", "w", stdout);
#else
	AvgEngine::Logging::openLog();
#endif
	AvgEngine::Logging::writeLog("[Main] Logging system initialized.");
	if (!glfwInit())
	{
		AvgEngine::Logging::writeLog("[GLFW] Failed to initialize GLFW!");
		exit(EXIT_FAILURE);
	}

	AvgEngine::Logging::writeLog("[GLFW] GLFW initialized.");

	glfwSetErrorCallback([](int error, const char* description)
		{
			AvgEngine::Logging::writeLog("[GLFW] [Error] " + std::string(description) + " - Code: " + std::to_string(error));
		});

	// Wine check
	const char* (CDECL * pwine_get_version)(void);
	HMODULE hntdll = GetModuleHandle(L"ntdll.dll");
	if (!hntdll)
	{
		AvgEngine::Logging::writeLog("[Main] Error retrieving NTDLL handle");
	}

	pwine_get_version = (const char* (*)())GetProcAddress(hntdll, "wine_get_version");

	if (pwine_get_version)
	{
		AvgEngine::Logging::writeLog("[Main] Running on wine version " + std::string(pwine_get_version()));
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
	glfwWindowHint(GLFW_DOUBLEBUFFER, GL_TRUE);
	glfwWindowHint(GLFW_SAMPLES, 4);

	AvgEngine::Game* game = new AvgEngine::Game("Demo", "v1.0");

	glfwMakeContextCurrent(game->Window);

	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	glfwMakeContextCurrent(NULL);

	AvgEngine::Logging::writeLog("[GLAD] GLAD initialized.");

	glfwSetKeyCallback(game->Window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
		{
			AvgEngine::Events::Event e;
			switch (action)
			{
			case GLFW_PRESS:
				e.type = AvgEngine::Events::EventType::Event_KeyPress;
				e.data = key;
				AvgEngine::Game::Instance->QueueEvent(e);
				break;
			case GLFW_RELEASE:
				e.type = AvgEngine::Events::EventType::Event_KeyRelease;
				e.data = key;
				AvgEngine::Game::Instance->QueueEvent(e);
				break;
			default:
				break;
			}
		});

	glfwSetMouseButtonCallback(game->Window, [](GLFWwindow* window, int button, int action, int mods)
		{
			AvgEngine::Events::Event e;
			double xpos, ypos;
			glfwGetCursorPos(window, &xpos, &ypos);
			AvgEngine::Events::Vec v;
			v.x = xpos;
			v.y = ypos;
			switch (action)
			{
			case GLFW_PRESS:
				e.type = AvgEngine::Events::EventType::Event_MouseDown;
				e.data = button;
				e.vector = v;
				AvgEngine::Game::Instance->QueueEvent(e);
				break;
			case GLFW_RELEASE:
				e.type = AvgEngine::Events::EventType::Event_MouseRelease;
				e.data = button;
				e.vector = v;
				AvgEngine::Game::Instance->QueueEvent(e);
				break;
			default:
				break;
			}
		});

	glfwSetWindowSizeCallback(game->Window, [](GLFWwindow* window, int width, int height)
		{
			AvgEngine::Events::Event e;
			e.type = AvgEngine::Events::EventType::Event_WindowResize;
			e.data = width;
			e.vector.x = width;
			e.vector.y = height;
			AvgEngine::Game::Instance->QueueEvent(e);
		});

	glfwSetScrollCallback(game->Window, [](GLFWwindow* window, double xoffset, double yoffset)
		{
			AvgEngine::Events::Event e;
			e.type = AvgEngine::Events::EventType::Event_MouseScroll;
			e.data = yoffset;
			AvgEngine::Game::Instance->QueueEvent(e);
		});

	AvgEngine::Logging::writeLog("[Main] Starting game...");

	// Render thread

	bool shouldQuit = false;

	std::thread t = std::thread([&] {
		glfwMakeContextCurrent(game->Window);
		glfwSwapInterval(0);

		AvgEngine::External::OpenAL::AL::init();
		AvgEngine::Logging::writeLog("[Main] Initialized OpenAL!");

		AvgEngine::External::OpenAL::AL::listDevices();
		AvgEngine::External::OpenAL::AL::createContext();

		AvgEngine::Render::Display::Init();
		AvgEngine::Logging::writeLog("[Main] Initialized Display!");

		std::shared_ptr<DemoMenu> cm = std::make_shared<DemoMenu>();

		game->NextMenu = cm;
		game->Switch();

		while (!shouldQuit)
		{
			glClearColor(0.05f, 0.05f, 0.05f, 1);
			glClear(GL_COLOR_BUFFER_BIT);

			game->update();

			if (game->CurrentMenu != NULL)
			{
				game->CurrentMenu->cameraDraw();
			}

			glfwSwapBuffers(game->Window);

		}
	});

	while (!glfwWindowShouldClose(game->Window))
	{
		glfwPollEvents();
	}

	shouldQuit = true;

	t.join();

	glfwDestroyWindow(game->Window);
	glfwTerminate();

	exit(EXIT_SUCCESS);
}
