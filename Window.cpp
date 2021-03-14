#include "Window.h"
#include "GLFW/glfw3.h"
#include <iostream>
#include "Rendering/Renderer.h"
#include "Game.h"
#include <imgui/imgui.h>
#include "Artwork/Camera.h"
#include <imgui/imgui_impl_opengl3.h>
#include <imgui/imgui_impl_glfw.h>
#include "Rendering/Texture.h"
#include <imgui/TextEditor.h>
#include "Input/Input.h"
#include "Core/AnimationPlayer.h"
GLFWwindow* Window::m_Window;
Camera* camera;


static const char* fileToEdit = "Res/Shaders/Screen.shader";
//	static const char* fileToEdit = "test.cpp";

TextEditor editor;
auto lang = TextEditor::LanguageDefinition::CPlusPlus();

Window::Window(int width, int height, const std::string title)
{
	// Init glfw
	if (!glfwInit())
		std::cout << "GLFW init failed." << std::endl;
    
	m_Width = width;
	m_Height = height;
	m_Window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
    
	// Error check
	if (!m_Window)
		std::cout << "Window creation failed." << std::endl;
    
	glfwMakeContextCurrent(m_Window);
    
	std::cout << glGetString(GL_VERSION) << std::endl;

	Renderer::Init();

	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL(m_Window, true);
	ImGui_ImplOpenGL3_Init("#version 330");


	camera = new Camera();
	m_Game = new Game();

	{
		std::ifstream t(fileToEdit);
		if (t.good())
		{
			std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
			editor.SetText(str);
		}
	}
}

float deltaTime = 0.0f;
void Window::Draw(Timestep timestep)
{
	int width, height;
	glfwGetWindowSize(m_Window, &width, &height);
	glViewport(0, 0, width, height);
	glfwPollEvents();

	camera->Update(timestep);
	AnimationPlayer::Get()->Update(timestep);

	deltaTime += timestep;

	m_Game->Draw(camera);
    
	Renderer::QuadShader->SetUniform1f("deltaTime", AnimationPlayer::Get()->GetCurrentTime());
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
	static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;
	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->GetWorkPos());
	ImGui::SetNextWindowSize(viewport->GetWorkSize());
	ImGui::SetNextWindowViewport(viewport->ID);

	ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
	ImGui::DockSpaceOverViewport(viewport, dockspace_flags);

	{
		ImGui::Begin("Render");
		{

			ImVec2 regionAvail = ImGui::GetContentRegionAvail();
			glm::vec2 viewportPanelSize = glm::vec2(regionAvail.x, regionAvail.y);
			Texture* texture = m_Game->GetFrameBuffer()->GetTexture(GL_COLOR_ATTACHMENT0);
			texture->Bind();
			ImGui::Image((void*)(texture->GetId()), regionAvail, ImVec2(0, 1), ImVec2(1, 0));
			ImGui::End();

		}
	}
	editor.SetReadOnly(camera->controlled);
	auto cpos = editor.GetCursorPosition();
	ImGui::Begin("Shader settings", nullptr, ImGuiWindowFlags_MenuBar);
	{
		if (ImGui::Button("Reload"))
		{
			Renderer::ReloadShader();
			
		}

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Save", "Ctrl-S"))
				{
					auto textToSave = editor.GetText();
					std::ofstream myfile(fileToEdit);
					if (myfile.is_open())
					{
						myfile << textToSave;
						myfile.close();
					}
					else std::cout << "Unable to open file";
					/// save text....
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Edit"))
			{
				bool ro = editor.IsReadOnly();
				if (ImGui::MenuItem("Read-only mode", nullptr, &ro))
					editor.SetReadOnly(ro);
				ImGui::Separator();

				if (ImGui::MenuItem("Undo", "ALT-Backspace", nullptr, !ro && editor.CanUndo()))
					editor.Undo();
				if (ImGui::MenuItem("Redo", "Ctrl-Y", nullptr, !ro && editor.CanRedo()))
					editor.Redo();

				ImGui::Separator();

				if (ImGui::MenuItem("Copy", "Ctrl-C", nullptr, editor.HasSelection()))
					editor.Copy();
				if (ImGui::MenuItem("Cut", "Ctrl-X", nullptr, !ro && editor.HasSelection()))
					editor.Cut();
				if (ImGui::MenuItem("Delete", "Del", nullptr, !ro && editor.HasSelection()))
					editor.Delete();
				if (ImGui::MenuItem("Paste", "Ctrl-V", nullptr, !ro && ImGui::GetClipboardText() != nullptr))
					editor.Paste();

				ImGui::Separator();

				if (ImGui::MenuItem("Select all", nullptr, nullptr))
					editor.SetSelection(TextEditor::Coordinates(), TextEditor::Coordinates(editor.GetTotalLines(), 0));

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("View"))
			{
				if (ImGui::MenuItem("Dark palette"))
					editor.SetPalette(TextEditor::GetDarkPalette());
				if (ImGui::MenuItem("Light palette"))
					editor.SetPalette(TextEditor::GetLightPalette());
				if (ImGui::MenuItem("Retro blue palette"))
					editor.SetPalette(TextEditor::GetRetroBluePalette());
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		ImGui::Text("%6d/%-6d %6d lines  | %s | %s | %s | %s", cpos.mLine + 1, cpos.mColumn + 1, editor.GetTotalLines(),
			editor.IsOverwrite() ? "Ovr" : "Ins",
			editor.CanUndo() ? "*" : " ",
			editor.GetLanguageDefinition().mName.c_str(), fileToEdit);

		editor.Render("TextEditor");
	}
	ImGui::End();
	static ImGuiTableFlags flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable;

	int time = 0;
	ImGui::Begin("Animator");
	{
		ImVec2 p = ImGui::GetMousePos();
		ImGui::BeginChild("##Child", ImGui::GetContentRegionAvail(), true);
		{
			ImVec2 size = ImGui::GetContentRegionAvail();
			size.x *= 0.2f;

			if(ImGui::Button("Play"))
				AnimationPlayer::Get()->Play();
			ImGui::SameLine();
			if(ImGui::Button("Stop"))
				AnimationPlayer::Get()->Stop();

			ImGui::SameLine();
			std::string currentTime = "Current time: " + std::to_string(AnimationPlayer::Get()->GetCurrentTime());
			ImGui::Text(currentTime.c_str());
			ImGui::SameLine();
			ImGui::InputFloat("Length", &AnimationPlayer::Get()->m_MaxTime, 0.5f);
			ImGui::BeginChild("##uniforms", size, true);
			{
				std::vector < std::string> uniforms = AnimationPlayer::Get()->GetAnimation()->GetUniforms();
				for (int i = 0; i < uniforms.size(); i++)
				{
					ImGui::Text(uniforms[i].c_str());
				}
				ImGui::EndChild();
			}
			ImGui::SameLine();
			size = ImGui::GetContentRegionAvail();

			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
			ImGui::BeginChild("##time", size, true);
			{
				
				Input::IsMouseButtonPressed(1);


				ImVec2 pos = ImGui::GetWindowPos();
				ImVec2 cursor = ImGui::GetMousePos();

				
				if (cursor.x > pos.x && cursor.x < pos.x + ImGui::GetWindowWidth() && ImGui::IsMouseDragging(0))
				{
					pos.x = cursor.x;
					AnimationPlayer::Get()->SetCurrentTime(((pos.x - ImGui::GetWindowPos().x )/ ImGui::GetWindowWidth())* AnimationPlayer::Get()->m_MaxTime);
				}
				else
				{
					float percentage = (AnimationPlayer::Get()->GetCurrentTime() / AnimationPlayer::Get()->GetMaxTime());
					float final = percentage * ImGui::GetWindowWidth() + ImGui::GetWindowPos().x;
					pos.x = final;
				}
					

				ImVec2 target = ImVec2(pos.x, pos.y + ImGui::GetWindowHeight());
				ImGui::GetForegroundDrawList()->AddLine(pos, target, IM_COL32(255, 0, 0, 255), 3.0f);

				// Markers
				float space = ImGui::GetWindowWidth() / AnimationPlayer::Get()->m_MaxTime / 10;
				for (int i = 0; i < (int)AnimationPlayer::Get()->m_MaxTime * 10; i++)
				{
					pos.x = ImGui::GetWindowPos().x + i * space + space;
					target.x = ImGui::GetWindowPos().x + i * space + space;
					float size = 1.0f;
					auto color = IM_COL32(128, 128, 128, 255);
					if ((i + 1) % 10 == 0) {
						color = IM_COL32(255, 255, 255, 255);
						size = 3.0f;
					}
					ImGui::GetBackgroundDrawList()->AddLine(pos, target, color, size);
				}

				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
				std::vector < std::string> uniforms = AnimationPlayer::Get()->GetAnimation()->GetUniforms();
				for (int row = 0; row < uniforms.size(); row++)
				{
					auto kf = AnimationPlayer::Get()->GetAnimation()->GetUniformKeyframes(uniforms[row]);
					float last = 0.0f;
					for (int u = 0; u < kf.size(); u++)
					{
						float percentage = (kf[u]->time / AnimationPlayer::Get()->GetMaxTime() - last);
						float final = percentage * (ImGui::GetWindowWidth());
						
						ImGui::Dummy(ImVec2(final - (25 / 2), 25));
						ImGui::SameLine();
						ImGui::Button("", ImVec2(25, 25));
						
						if(u + 1 < kf.size())
							ImGui::SameLine();
						last = percentage;

					}
				}
				
				ImGui::PopStyleVar();


				ImGui::EndChild();
				
			}
			ImGui::PopStyleVar();
			ImGui::EndChild();
		}
		

		ImGui::End();

	}



	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	glfwSwapBuffers(m_Window);
}