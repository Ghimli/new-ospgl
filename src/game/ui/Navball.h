#pragma once
/*
#include "../../assets/Image.h"
#include "../../assets/Shader.h"
#include "../../assets/AssetManager.h"
#include "../../assets/BitmapFont.h"

#include "../../util/render/Framebuffer.h"
#include "../../util/render/TextureDrawer.h"
#include "../../util/render/TextDrawer.h"
#include "../../util/geometry/SphereGeometry.h"

#include "../../universe/vessel/Vessel.h"
#include "../../universe/ReferenceFrame.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

class Navball
{
public:

	GLuint ball_vao, ball_vbo, ball_ebo;
	size_t ball_index_count;

	Framebuffer fbuffer;

	Shader* navball_shader;
	Image* texture;
	Image* frame_tex;
	Image* prograde_tex;
	Image* retrograde_tex;
	Image* normal_tex;
	Image* antinormal_tex;
	Image* radialin_tex;
	Image* radialout_tex;

	BitmapFont* speed_font;

	float view_distance = 4.0f;
	// Used to fit the navball inside the frame texture
	float scale = 0.9f;
	float voffset = 1.0f - scale + scale * 0.1785f;

	// Position of the navball's center in the screen
	float xoffset = 0.5f;

	// Actual scale of the UI element
	float final_scale = 0.75f;

	// Scale of the markers, scaled when the markers get near the edges
	float icon_scale = 0.3f;

	// The coordinate it's given is what the navball
	// will show at its center (the direction of the vessel)
	void draw_to_texture(const Vessel& vessel, const ReferenceFrame& frame);

	// Draws the navball to the screen centered bottom
	void draw_to_screen(glm::ivec2 screen_size);

	Navball();
	~Navball();
};


template<>
class GenericSerializer<Navball>
{
public:

	static void serialize(const Navball& what, cpptoml::table& target)
	{
	}

	// Deserialize is only called for bodies and barycenters
	// Star is special
	static void deserialize(Navball& to, const cpptoml::table& from)
	{
		std::string speed_font_path;
		SAFE_TOML_GET_OR(speed_font_path, "speed_font", std::string, "core:fonts/fira_code_medium.fnt");
		to.speed_font = assets->get_from_path<BitmapFont>(speed_font_path);
	}
};
*/


