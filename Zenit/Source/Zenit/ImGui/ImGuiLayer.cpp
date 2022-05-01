#include "Zenit/Core/Application.h"
#include "Zenit/Core/Window.h"

#include "ImGuiLayer.h"

#include "ImGuiBuilder.cpp"
#include "imgui.h"


namespace Zenit {

	ImGuiLayer::ImGuiLayer() : Layer("ImGui Layer")
	{
	}

	ImGuiLayer::~ImGuiLayer()
	{
	}

	void ImGuiLayer::Begin()
	{
		ImGuiIO& io = ImGui::GetIO();
		Application& app = Application::GetInstance();
		io.DisplaySize = ImVec2((float)app.GetWindow().GetWidth(), (float)app.GetWindow().GetHeight());

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGui::DockSpaceOverViewport();
	}

	void ImGuiLayer::End()
	{
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		ImGuiIO& io = ImGui::GetIO();

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* backup_current_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_current_context);
		}

	}

	void ImGuiLayer::OnAttach()
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
		//io.ConfigViewportsNoAutoMerge = true;
		//io.ConfigViewportsNoTaskBarIcon = true;
		io.IniFilename = "Settings/imgui.ini";
		io.FontDefault = io.Fonts->AddFontFromFileTTF("Assets/Fonts/Lato/Lato-Regular.ttf", 16.5f);


		SetStyle();
		// Setup Dear ImGui style

		// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		ImGui_ImplGlfw_InitForOpenGL(Application::GetInstance().GetWindow().GetNativeWindow(), true);
		ImGui_ImplOpenGL3_Init("#version 430");
	}

	void ImGuiLayer::OnDetach()
	{
		ImGui_ImplGlfw_Shutdown();
		ImGui_ImplOpenGL3_Shutdown();
		ImGui::DestroyContext();
	}

	void ImGuiLayer::OnImGuiRender()
	{
	}

	void ImGuiLayer::SetStyle()
	{
		ImGuiStyle& style = ImGui::GetStyle();
		style.WindowRounding = 0.0f;
		style.ChildRounding = 0.0f;
		style.FrameRounding = 0.0f;
		style.ScrollbarRounding = 0.0f;
		style.TabRounding = 0.0f;

		ImVec4* colors = style.Colors;
		colors[ImGuiCol_WindowBg] = { 0.1,0.1,0.1,1 };

		colors[ImGuiCol_ScrollbarBg] = { 0.3,0.3,0.3,1 };
		colors[ImGuiCol_ScrollbarGrab] = { 0.2,0.2,0.2,1 };
		colors[ImGuiCol_ScrollbarGrabActive] = { 0.6,0.6,0.6,1 };
		colors[ImGuiCol_ScrollbarGrabHovered] = { 0.5,0.5,0.5,1 };

		colors[ImGuiCol_FrameBg] = { 0.1,0.1,0.1,1 };
		colors[ImGuiCol_FrameBgActive] = { 0.3,0.3,0.3,1 };
		colors[ImGuiCol_FrameBgHovered] = { 0.4,0.4,0.4,1 };

		colors[ImGuiCol_Header] = { 0.2,0.2,0.2,1 };
		colors[ImGuiCol_HeaderActive] = { 0.33,0.33,0.33,1 };
		colors[ImGuiCol_HeaderHovered] = { 0.27,0.27,0.27,1 };
		
		colors[ImGuiCol_ResizeGrip] = { 0.4,0.4,0.4,1 };
		colors[ImGuiCol_ResizeGripActive] = { 0.9,0.9,0.9,1 };
		colors[ImGuiCol_ResizeGripHovered] = { 0.75,0.75,0.75,1 };

		colors[ImGuiCol_TitleBg] = { 0.15,0.15,0.15,1 };
		colors[ImGuiCol_TitleBgActive] = { 0.15,0.15,0.15,1 };
		colors[ImGuiCol_TitleBgCollapsed] = { 0.1,0.1,0.1,1 };

		colors[ImGuiCol_Button] = { 0.2,0.2,0.2,1 };
		colors[ImGuiCol_ButtonActive] = { 0.15,0.15,0.15,1 };
		colors[ImGuiCol_ButtonHovered] = { 0.3,0.3,0.3,1 };

		colors[ImGuiCol_Tab] = { 1,0,0,1 };
		colors[ImGuiCol_TabActive] = { 0.25,0.25,0.25,1 };
		colors[ImGuiCol_TabHovered] = { 0.3,0.3,0.3,1 };
		colors[ImGuiCol_TabUnfocused] = { 0.15,0.15,0.15,1 };
		colors[ImGuiCol_TabUnfocusedActive] = { 0.2,0.2,0.2,1 };

	}
}