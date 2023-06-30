#include "Zenit/Core/Application.h"
#include "Zenit/Core/Window.h"

#include "ImGuiLayer.h"

#include "ImGuiBuilder.cpp"
#include "imgui.h"


namespace Zenit {

	ImGuiLayer::ImGuiLayer() : EngineLayer("ImGui Layer")
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
		
		io.IniFilename = "Settings/imgui.ini";
		io.FontDefault = io.Fonts->AddFontFromFileTTF("Assets/Fonts/Lato/Lato-Regular.ttf", 16.5f);

		SetStyle();

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
		colors[ImGuiCol_WindowBg] = { 0.1f,0.1f,0.1f,1 };

		colors[ImGuiCol_ScrollbarBg] = { 0.3f,0.3f,0.3f,1 };
		colors[ImGuiCol_ScrollbarGrab] = { 0.2f,0.2f,0.2f,1 };
		colors[ImGuiCol_ScrollbarGrabActive] = { 0.6f,0.6f,0.6f,1 };
		colors[ImGuiCol_ScrollbarGrabHovered] = { 0.5,0.5,0.5,1 };

		colors[ImGuiCol_FrameBg] = { 0.2f,0.2f,0.2f,1 };
		colors[ImGuiCol_FrameBgActive] = { 0.28f,0.28f,0.28f,1 };
		colors[ImGuiCol_FrameBgHovered] = { 0.3f,0.3f,0.3f,1 };

		colors[ImGuiCol_Header] = { 0.2f,0.2f,0.2f,1 };
		colors[ImGuiCol_HeaderActive] = { 0.33f,0.33f,0.33f,1 };
		colors[ImGuiCol_HeaderHovered] = { 0.27f,0.27f,0.27f,1 };
		
		colors[ImGuiCol_ResizeGrip] = { 0.4f,0.4f,0.4f,1 };
		colors[ImGuiCol_ResizeGripActive] = { 0.9f,0.9f,0.9f,1 };
		colors[ImGuiCol_ResizeGripHovered] = { 0.75,0.75,0.75,1 };

		colors[ImGuiCol_TitleBg] = { 0.15f,0.15f,0.15f,1 };
		colors[ImGuiCol_TitleBgActive] = { 0.15f,0.15f,0.15f,1 };
		colors[ImGuiCol_TitleBgCollapsed] = { 0.1f,0.1f,0.1f,1 };

		colors[ImGuiCol_Button] = { 0.2f,0.2f,0.2f,1 };
		colors[ImGuiCol_ButtonActive] = { 0.15f,0.15f,0.15f,1 };
		colors[ImGuiCol_ButtonHovered] = { 0.3f,0.3f,0.3f,1 };

		colors[ImGuiCol_Tab] = { 0.15f,0.15f,0.15f,1 };
		colors[ImGuiCol_TabActive] = { 0.25,0.25,0.25,1 };
		colors[ImGuiCol_TabHovered] = { 0.3f,0.3f,0.3f,1 };
		colors[ImGuiCol_TabUnfocused] = { 0.15f,0.15f,0.15f,1 };
		colors[ImGuiCol_TabUnfocusedActive] = { 0.2f,0.2f,0.2f,1 };

		colors[ImGuiCol_SliderGrab] = { 0.45f, 0.45f, 0.45f, 1 };
		colors[ImGuiCol_SliderGrabActive] = { 0.42f, 0.42f, 0.42f, 1 };
	}
}