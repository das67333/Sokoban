#pragma once

#include <SFML/Graphics.hpp>
#include "config.h"
#include "gamelogic.h"
#include "tinyxml2.h"

class GameState;

class Graphics : public sf::RenderWindow {
public:
    Graphics(GameState& state);
    void update();

private:
    static tinyxml2::XMLElement* find_xml_element(tinyxml2::XMLElement* ptr,
                                                       const std::string& name);
    void parse_sprites_from_file();
    void place_into_cell(sf::Sprite& sprite, uint32_t i, uint32_t j);
    void draw_field(CharacterDirection dir);
    void draw_fps();
    void draw_moves_number();
    void handle_events();
    void handle_keyboard();
    void handle_mouse();

    const uint32_t WIDTH, HEIGHT, CELL_SIZE;

    GameState& state_;
    sf::Texture texture_;
    sf::Sprite character_left_, character_right_, character_down_, character_up_, crate_, endpoint_,
        ground_, wall_;
    sf::Font font_;
    sf::Text text_;
    sf::Event event_;
    Solver solver_;
    bool is_paused_, is_solving_;
};
