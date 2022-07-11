#pragma once

#include <string>

enum class CharacterDirection { left, right, down, up, undefined };

namespace config {
const uint32_t fps_max = 15;
const uint32_t cell_size = 64;
const std::string sprites_path = "../resources/sprites.png";
const std::string xml_path = "../resources/sprites.xml";
const std::string levels_path = "../levels/";
const std::string font_path = "../resources/arial.ttf";
const uint32_t font_size = 15;
const float fps_x = 15., fps_y = 10.;
const float moves_number_x = 15., moves_number_y = 30.;

const std::string character_left = "Character1.png";
const std::string character_right = "Character2.png";
const std::string character_down = "Character4.png";
const std::string character_up = "Character7.png";
const std::string crate = "Crate_Blue.png";
const std::string endpoing = "EndPoint_Red.png";
const std::string ground = "GroundGravel_Dirt.png";
const std::string wall = "Wall_Brown.png";
}  // namespace config
