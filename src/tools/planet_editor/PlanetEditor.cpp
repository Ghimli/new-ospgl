#include "PlanetEditor.h"
#include <fstream>

void PlanetEditor::update(float dt, ImFont* code_font)
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("New"))
			{

			}
			
			if (ImGui::MenuItem("Save"))
			{

			}

			if (ImGui::MenuItem("Save As"))
			{

			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("View"))
		{
			if (ImGui::MenuItem("QuadTree Planet"))
			{
				show_planet_info = !show_planet_info;
			}

			if (ImGui::MenuItem("Reset Camera"))
			{
				camera.reset(config.radius);
			}

			if (ImGui::MenuItem("Wireframe"))
			{
				wireframe = !wireframe;
			}

			ImGui::EndMenu();
		}

		std::string str = "Speed: " + std::to_string(camera.speed) + " m/s";
		ImGui::MenuItem(str.c_str());
		std::string alt = "Altitude: " + std::to_string(altitude) + " m";
		ImGui::MenuItem(alt.c_str());

		std::string salt = "Sea Altitude: " + std::to_string(glm::length(camera.pos) - config.radius) + " m";
		ImGui::MenuItem(salt.c_str());

		ImGui::EndMainMenuBar();
	}

	
	// The sleep avoids crashes, atleast on Windows
	if (script_watch.has_changed())
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(300));
		on_script_file_change();
	}

	if (config_watch.has_changed())
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(300));
		on_config_file_change();
	}

	if (renderer.rocky != nullptr)
	{
		renderer.rocky->server->update(renderer.rocky->qtree);
		renderer.rocky->qtree.dirty = false;
		renderer.rocky->qtree.update(*renderer.rocky->server);
	}
	



	if (show_planet_info)
	{
		do_planet_window();
	}




	if (!ImGui::IsAnyItemFocused() && !ImGui::IsAnyItemActive() && !ImGui::IsAnyWindowFocused())
	{
		bool moved = false;

		// Motion
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		{
			camera.forwards(dt);
			moved = true;
		}

		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		{
			camera.backwards(dt);
			moved = true;
		}

		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		{
			camera.leftwards(dt);
			moved = true;
		}

		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		{
			camera.rightwards(dt);
			moved = true;
		}

		if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
		{
			camera.upwards(dt);
			moved = true;
		}

		if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
		{
			camera.downwards(dt);
			moved = true;
		}

		if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		{
			camera.tilt(dt, -1.0f);
		}

		if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		{
			camera.tilt(dt, 1.0f);
		}

		if (moved)
		{
			on_move();
		}

		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_2))
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			if (input->mouse_delta != glm::dvec2(0.0, 0.0))
			{
				camera.mouse(input->mouse_delta, dt);
			}

			if (input->mouse_scroll_delta != 0)
			{
				camera.speed += camera.speed * input->mouse_scroll_delta * 0.05;
			}
		}
		else
		{
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}

	}
	else
	{
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}
	
}

void PlanetEditor::render(int width, int height)
{
	if (wireframe)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	}
	else
	{
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	// ~1 light year
	float far_plane = 10e16f;

	glm::mat4 proj = glm::perspective(glm::radians(60.0f), (float)width / (float)height, 0.1f, far_plane);
	glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), (glm::vec3)camera.forward, (glm::vec3)camera.up);

	// We apply the view position to the world so floating point errors happen
	// far away, and not near the camera
	glm::dmat4 model = glm::translate(glm::dmat4(1.0f), -camera.pos);
	model = glm::scale(model, glm::dvec3(config.radius, config.radius, config.radius));

	glm::dmat4 proj_view = (glm::dmat4)proj * (glm::dmat4)view;

	renderer.render(proj_view, model, far_plane, camera.pos, config, glfwGetTime());


}

void PlanetEditor::on_script_file_change()
{
	delete renderer.rocky->server;
	bool exists = false;

	{
		std::ifstream checker = std::ifstream(path);
		exists = checker.good();
	}

	if (!exists)
	{
		logger->warn("Script file did not exist, copying default");

		std::filesystem::path script_path = path;
		script_path.remove_filename();

		std::filesystem::create_directories(script_path);

		std::ifstream src("./res/default_planet.lua", std::ios::binary);
		std::ofstream dst(path, std::ios::binary);

		dst << src.rdbuf();

	}

	std::string scripts = assets->loadString(path);
	renderer.rocky->server = new PlanetTileServer(scripts, &config, 0, 0, config.surface.has_water);

	renderer.rocky->qtree.dirty = true;
}

void PlanetEditor::on_config_file_change()
{

	SerializeUtil::read_file_to(config_path, config);

	if (config.surface.script_path != path)
	{
		path = config.surface.script_path;
	}

	on_script_file_change();
}

PlanetEditor::PlanetEditor(GLFWwindow* window, const std::string& planet_name)
{
	renderer.rocky = new RockyPlanetRenderer();
	renderer.atmo = new AtmosphereRenderer();

	this->window = window;
	last_frame_save = false;


	config_path = "./res/planets/" + planet_name + "/config.toml";

	// Copy default config if file does not exists
	std::ifstream f2(config_path);
	if (!f2.good())
	{
		std::filesystem::path cpath = config_path;
		cpath.remove_filename();

		std::filesystem::create_directories(cpath);

		std::ifstream src("./res/default_planet.toml", std::ios::binary);
		std::ofstream dst(config_path, std::ios::binary);

		dst << src.rdbuf();
	}


	SerializeUtil::read_file_to(config_path, config);

	if (config.surface.script_path == "$$NEW_SCRIPT$$")
	{
		path = "./res/planets/" + planet_name + "/surface.lua";
		config.surface.script_path = path;
		SerializeUtil::write_to(config_path, config);
	}
	else
	{
		path = config.surface.script_path;
	}

	on_script_file_change();





	std::string script = assets->loadString(path);
	renderer.rocky->server = new PlanetTileServer(script, &config, 0, 0, config.surface.has_water);

	

	// Load custom theme
	/*const static TextEditor::Palette pallete = 
	{ {
			0xffcbcbcb,	// Default
			0xff756cd4,	// Keyword	
			0xffdd78c6,	// Number
			0xff56b4e5,	// String
			0xff56b4e5, // Char literal
			0xffcbcbcb, // Punctuation
			0xff408080,	// Preprocessor
			0xff79c398, // Identifier
			0xff79c398, // Known identifier
			0xff79c398, // Preproc identifier
			0xff79685f, // Comment (single line)
			0xff79685f, // Comment (multi line)
			0xff342c28, // Background
			0xffe0e0e0, // Cursor
			0x80a06020, // Selection
			0x800020ff, // ErrorMarker
			0x40f08000, // Breakpoint
			0xff79685f, // Line number
			0x10ffffff, // Current line fill
			0x10808080, // Current line fill (inactive)
			0x10a0a0a0, // Current line edge
	} };

	// Setup editors
	auto script_lang = TextEditor::LanguageDefinition::Lua();
	script_editor.SetLanguageDefinition(script_lang);

	script_editor.SetText(script_loaded);
	script_editor.SetShowWhitespaces(false);
	script_editor.SetPalette(pallete); 

	auto config_lang = TextEditor::LanguageDefinition::TOML();
	toml_editor.SetLanguageDefinition(config_lang);

	toml_editor.SetText(config_loaded);
	toml_editor.SetShowWhitespaces(false);
	toml_editor.SetPalette(pallete);
	*/

	script_watch = FileWatcher(path);
	config_watch = FileWatcher(config_path);
	
	camera.reset(config.radius);

	on_move();

}


PlanetEditor::~PlanetEditor()
{
	delete renderer.rocky->server;
}

void PlanetEditor::do_editor_window(ImFont* code_font, TextEditor* editor, const std::string& name, bool* open)
{
	ImGui::Begin(name.c_str(), open);

	ImGui::PushFont(code_font);

	editor->Render((name + "EDIT").c_str());
	
	ImGui::PopFont();

	ImGui::End();
}


void PlanetEditor::do_planet_window()
{
	if (ImGui::Begin("Planet", &show_planet_info))
	{



		if (ImGui::CollapsingHeader("Quadtree", ImGuiTreeNodeFlags_DefaultOpen))
		{
			renderer.rocky->qtree.do_imgui(*renderer.rocky->server);
		}

		if (ImGui::CollapsingHeader("Tile Server"))
		{
			renderer.rocky->server->do_imgui();
		}

	}
	ImGui::End();
}

void PlanetEditor::on_move()
{
	glm::vec3 pos_nrm = (glm::vec3)glm::normalize(camera.pos);
	PlanetSide side = renderer.rocky->qtree.get_planet_side(pos_nrm);
	glm::dvec2 offset = renderer.rocky->qtree.get_planet_side_offset(pos_nrm, side);

	double height = std::max(glm::length(camera.pos) - config.radius - renderer.rocky->server->get_height(pos_nrm, 1), 1.0);
	altitude = height;
	height /= config.radius;
	double depthf = (config.surface.coef_a - (config.surface.coef_a * glm::log(height) 
		/ ((glm::pow(height, 0.15) * config.surface.coef_b))) - 0.3 * height) * 0.4;

	depth = (size_t)round(std::max(std::min(depthf, (double)config.surface.max_depth - 1.0), -1.0) + 1.0);

	renderer.rocky->qtree.set_wanted_subdivide(offset, side, depth);

}
