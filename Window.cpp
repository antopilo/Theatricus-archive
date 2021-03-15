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
#include "imgui/FontAwesome5.h"

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
	io.Fonts->AddFontDefault();
	// merge in icons from Font Awesome
	static const ImWchar icons_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
	ImFontConfig icons_config; icons_config.MergeMode = true; icons_config.PixelSnapH = true;
	io.Fonts->AddFontFromFileTTF(FONT_ICON_FILE_NAME_FAS, 16.0f, &icons_config, icons_ranges);

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

char buffText[32];
float deltaTime = 0.0f;
KeyFrame* selectedKeyFrame = nullptr;
std::string selectedTrack = "";
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

			if(ImGui::Button(ICON_FA_PLAY""))
				AnimationPlayer::Get()->Play();
			ImGui::SameLine();
			if (ImGui::Button(ICON_FA_STOP""))
			{
				if (!AnimationPlayer::Get()->IsPlaying())
					AnimationPlayer::Get()->SetCurrentTime(0);

				AnimationPlayer::Get()->Stop();
			}
			

			ImGui::SameLine();
			std::string currentTime = "Current time: " + std::to_string(AnimationPlayer::Get()->GetCurrentTime());
			ImGui::Text(currentTime.c_str());
			ImGui::SameLine();
			ImGui::InputFloat("Length", &AnimationPlayer::Get()->m_MaxTime, 0.5f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
			ImGui::BeginChild("##uniforms", size, true);
			{
				std::vector < std::string> uniforms = AnimationPlayer::Get()->GetAnimation()->GetUniforms();
				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
				for (int i = 0; i < uniforms.size(); i++)
				{
					
					ImGui::Button(uniforms[i].c_str(), ImVec2(size.x - 25, 25));
					ImGui::SameLine();
					if (ImGui::Button((ICON_FA_KEY  "##" + std::to_string(i)).c_str(), ImVec2(25, 25)))
					{
						AnimationPlayer::Get()->GetAnimation()->AddKeyframe(uniforms[i].c_str(), AnimationPlayer::Get()->GetCurrentTime(), 1.0f);
					}
				
			
				}
				bool openpopuptemp = true;
	
				if (ImGui::Button(ICON_FA_PLUS, ImVec2(size.x, 30)))
				{
					ImGui::OpenPopup("Delete?");
					
				}

				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
				ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8, 8));
				if (ImGui::BeginPopupModal("Delete?", &openpopuptemp, ImGuiWindowFlags_NoResize))
				{
					ImGui::Text("Add a new animated uniform:");
					ImGui::Dummy(ImVec2(0, 32));
					ImGui::Text("Note:");
					ImGui::TextWrapped("Currently only floats are supported, but its possible to edit a single float of a vector by \
						Doing u_MyVectorUniform.x to edit its value.");

					ImGui::Separator();
					ImGui::InputText("Uniform name", buffText, 32);


					if (ImGui::Button("Close", ImVec2(100, 30)))
						ImGui::CloseCurrentPopup();
					if (ImGui::Button("Add", ImVec2(100, 30)))
					{
						AnimationPlayer::Get()->GetAnimation()->AddKeyframe(buffText, 0.0f, 1.0f); // Todo add.
						ImGui::CloseCurrentPopup();
					}
						
					ImGui::EndPopup();
				}
				ImGui::PopStyleVar();
				ImGui::PopStyleVar();
				ImGui::PopStyleVar();
				ImGui::EndChild();
			}
			ImGui::PopStyleVar();
			ImGui::SameLine();
			size = ImGui::GetContentRegionAvail();

			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
			ImGui::BeginChild("##time", size, true);
			{
				//if (ImGui::IsWindowFocused() && !Input::IsKeyReleased(GLFW_KEY_SPACE))
				//{
				//	if (AnimationPlayer::Get()->IsPlaying())
				//		AnimationPlayer::Get()->Stop();
				//	else
				//		AnimationPlayer::Get()->Play();
				//}

				if (ImGui::BeginPopupContextItem("sssss"))
				{
					if (ImGui::Selectable("Add key"))

						if (ImGui::Selectable("Copy"));
					if (ImGui::Selectable("Copy Value"));
					if (ImGui::Selectable("Copy Time"));
					ImGui::EndPopup();
				}

				ImVec2 pos = ImGui::GetWindowPos();
				ImVec2 cursor = ImGui::GetMousePos();

				
				if (cursor.x > pos.x && cursor.x < pos.x + ImGui::GetWindowWidth() && ImGui::IsMouseDragging(0) && ImGui::IsWindowFocused())
				{
					pos.x = cursor.x;
					float time = ((pos.x - ImGui::GetWindowPos().x) / ImGui::GetWindowWidth()) * AnimationPlayer::Get()->m_MaxTime;

					if(time >= 0 && time <= AnimationPlayer::Get()->m_MaxTime)
						AnimationPlayer::Get()->SetCurrentTime(time);
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

				if (ImGui::IsMouseReleased(0) && ImGui::IsWindowFocused())
					selectedKeyFrame = nullptr;
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

						ImGui::PushID(row* u);
						std::string id = "##" + std::to_string(row * 100 + u);
						
						bool hasStyle = selectedKeyFrame == kf[u];

						if(hasStyle)
							ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2, 0.4, 1.00f, 1.0f));
				
						

						if (ImGui::IsMouseDragging(0) && hasStyle)
						{
							ImVec2 mousePos = ImGui::GetMousePos();
							mousePos.x = mousePos.x; // TODO: Add snapping here.
							float time = ((mousePos.x - ImGui::GetWindowPos().x) / ImGui::GetWindowWidth()) * AnimationPlayer::Get()->m_MaxTime;

							if (time >= 0 && time <= AnimationPlayer::Get()->m_MaxTime)
								kf[u]->time = time;
						}

					
						if (ImGui::Button((ICON_FA_CIRCLE + id).c_str() , ImVec2(25, 25)))
						{
							selectedTrack = uniforms[row];
							selectedKeyFrame = kf[u];
						}
					
					
						std::string contextId = "##" + std::to_string(100 * row + u) + "context";
						if (ImGui::BeginPopupContextItem(contextId.c_str()))
						{
							if (ImGui::Selectable("Delete"))
								AnimationPlayer::Get()->GetAnimation()->RemoveKeyframe(selectedTrack, u);
							if (ImGui::Selectable("Copy"));
							if (ImGui::Selectable("Copy Value"));
							if (ImGui::Selectable("Copy Time"));
							ImGui::EndPopup();
						}
						
						
						
						if (hasStyle)
							ImGui::PopStyleColor();

						ImGui::PopID();
						
						
						
						if(u < kf.size() - 1)
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

		if (ImGui::Begin("Inspector"))
		{
			if (selectedKeyFrame != nullptr){
				ImGui::Text(selectedTrack.c_str());
				ImGui::SameLine();
				ImGui::Text(" - float");

				ImGui::InputFloat("Time", &(selectedKeyFrame->time));
				ImGui::InputFloat("Value", &(selectedKeyFrame->value));
			}
			
			
		}
		ImGui::End();

		ImGui::End();

	}



	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	glfwSwapBuffers(m_Window);
}