#include "graphics.h"

#include "config.h"
#include "gamelogic.h"

#include <cassert>
#include <iostream>
#include <vector>

Graphics::Graphics(GameState& state)
    : sf::RenderWindow(sf::VideoMode(sf::VideoMode(state.get_width() * config::cell_size,
                                                   state.get_height() * config::cell_size)),
                       "Sokoban", sf::Style::Close),
      WIDTH(state.get_width()),
      HEIGHT(state.get_height()),
      CELL_SIZE(config::cell_size),
      state_(state),
      is_paused_(false),
      is_solving_(false) {
    assert(font_.loadFromFile(config::font_path));
    text_.setFont(font_);
    text_.setCharacterSize(config::font_size);
    text_.setFillColor(sf::Color::Green);
    setFramerateLimit(config::fps_max);
    parse_sprites_from_file();
}

void Graphics::update() {
    handle_events();
    if (is_paused_) {
        return;
    }
    if (is_solving_ && solver_.has_finished()) {
        is_solving_ = false;
    }
    CharacterDirection dir;
    if (is_solving_) {
        dir = solver_.pop_move();
        state_.move(dir);
    } else {
        dir = state_.get_character_direction();
    }
    clear();
    draw_field(dir);
    draw_fps();
    draw_moves_number();
    display();
}

tinyxml2::XMLElement* Graphics::find_xml_element(tinyxml2::XMLElement* ptr, const std::string& name) {
    while (ptr && name != ptr->FindAttribute("name")->Value()) {
        ptr = ptr->NextSiblingElement();
    }
    assert(ptr);
    return ptr;
}

void Graphics::parse_sprites_from_file() {
    texture_.loadFromFile(config::sprites_path);

    tinyxml2::XMLDocument doc;
    doc.LoadFile(config::xml_path.c_str());
    tinyxml2::XMLElement* root = doc.RootElement()->FirstChildElement();
    tinyxml2::XMLElement* ptr = nullptr;

    ptr = find_xml_element(root, config::character_left);
    character_left_.setTexture(texture_);
    character_left_.setTextureRect(
        sf::IntRect({ptr->IntAttribute("x"), ptr->IntAttribute("y")},
                    {ptr->IntAttribute("width"), ptr->IntAttribute("height")}));

    ptr = find_xml_element(root, config::character_right);
    character_right_.setTexture(texture_);
    character_right_.setTextureRect(
        sf::IntRect({ptr->IntAttribute("x"), ptr->IntAttribute("y")},
                    {ptr->IntAttribute("width"), ptr->IntAttribute("height")}));

    ptr = find_xml_element(root, config::character_down);
    character_down_.setTexture(texture_);
    character_down_.setTextureRect(
        sf::IntRect({ptr->IntAttribute("x"), ptr->IntAttribute("y")},
                    {ptr->IntAttribute("width"), ptr->IntAttribute("height")}));

    ptr = find_xml_element(root, config::character_up);
    character_up_.setTexture(texture_);
    character_up_.setTextureRect(
        sf::IntRect({ptr->IntAttribute("x"), ptr->IntAttribute("y")},
                    {ptr->IntAttribute("width"), ptr->IntAttribute("height")}));

    ptr = find_xml_element(root, config::crate);
    crate_.setTexture(texture_);
    crate_.setTextureRect(sf::IntRect({ptr->IntAttribute("x"), ptr->IntAttribute("y")},
                                      {ptr->IntAttribute("width"), ptr->IntAttribute("height")}));

    ptr = find_xml_element(root, config::endpoing);
    endpoint_.setTexture(texture_);
    endpoint_.setTextureRect(
        sf::IntRect({ptr->IntAttribute("x"), ptr->IntAttribute("y")},
                    {ptr->IntAttribute("width"), ptr->IntAttribute("height")}));

    ptr = find_xml_element(root, config::ground);
    ground_.setTexture(texture_);
    ground_.setTextureRect(
        sf::IntRect({ptr->IntAttribute("x"), ptr->IntAttribute("y")},
                    {ptr->IntAttribute("width"), ptr->IntAttribute("height")}));

    ptr = find_xml_element(root, config::wall);
    wall_.setTexture(texture_);
    wall_.setTextureRect(sf::IntRect({ptr->IntAttribute("x"), ptr->IntAttribute("y")},
                                     {ptr->IntAttribute("width"), ptr->IntAttribute("height")}));
}

void Graphics::place_into_cell(sf::Sprite& sprite, uint32_t i, uint32_t j) {
    sf::FloatRect rect = sprite.getGlobalBounds();
    sf::Vector2u size = {static_cast<uint32_t>(rect.width), static_cast<uint32_t>(rect.height)};
    sprite.setPosition(i * CELL_SIZE + (CELL_SIZE - size.x) / 2,
                       j * CELL_SIZE + (CELL_SIZE - size.y) / 2);
    draw(sprite);
}

void Graphics::draw_field(CharacterDirection dir) {
    sf::Sprite* character_ptr;
    switch (dir) {
        case CharacterDirection::left:
            character_ptr = &character_left_;
            break;
        case CharacterDirection::right:
            character_ptr = &character_right_;
            break;
        case CharacterDirection::down:
            character_ptr = &character_down_;
            break;
        case CharacterDirection::up:
            character_ptr = &character_up_;
            break;
        default:
            character_ptr = nullptr;
            break;
    }
    for (uint32_t j = 0; j != HEIGHT; ++j) {
        for (uint32_t i = 0; i != WIDTH; ++i) {
            place_into_cell(ground_, i, j);
            switch (state_.at(i, j)) {
                case '#':
                    place_into_cell(wall_, i, j);
                    break;
                case '@':
                    place_into_cell(*character_ptr, i, j);
                    break;
                case '+':
                    place_into_cell(endpoint_, i, j);
                    place_into_cell(*character_ptr, i, j);
                    break;
                case '$':
                    place_into_cell(crate_, i, j);
                    break;
                case '*':
                    place_into_cell(endpoint_, i, j);
                    place_into_cell(crate_, i, j);
                    break;
                case '.':
                    place_into_cell(endpoint_, i, j);
                    draw(endpoint_);
                    break;
                case ' ':
                    break;
                default:
                    assert(false);
                    break;
            }
        }
    }
}

void Graphics::draw_fps() {
    static sf::Clock clock;
    std::string str("FPS:\t\t");
    str.append(std::to_string(1. / clock.getElapsedTime().asSeconds()));
    clock.restart();
    str.erase(str.find('.') + 3);
    text_.setString(str);
    text_.setPosition({config::fps_x, config::fps_y});
    draw(text_);
}

void Graphics::draw_moves_number() {
    std::string str("Moves:\t");
    str.append(std::to_string(state_.get_moves_number()));
    text_.setString(str);
    text_.setPosition({config::moves_number_x, config::moves_number_y});
    draw(text_);
}

void Graphics::handle_events() {
    while (pollEvent(event_)) {
        switch (event_.type) {
            case sf::Event::Closed:
                close();
                break;
            case sf::Event::KeyPressed:
                if (!is_solving_) {
                    handle_keyboard();
                }
                break;
            case sf::Event::MouseButtonPressed:
                handle_mouse();
                break;
            default:
                break;
        }
    }
}

void Graphics::handle_keyboard() {
    switch (event_.key.code) {
        case sf::Keyboard::Escape:
            close();
            break;
        case sf::Keyboard::P:
            is_paused_ = !is_paused_;
            setMouseCursorVisible(is_paused_);
            break;
        case sf::Keyboard::R:
            state_.restore_field();
            break;
        case sf::Keyboard::S: {
            sf::Clock clock;
            solver_.find_shortest_path(state_);
            std::cout << "solving time: " << clock.getElapsedTime().asSeconds() << '\n';
            is_solving_ = true;
            break;
        }
        case sf::Keyboard::Left:
            state_.move(CharacterDirection::left);
            break;
        case sf::Keyboard::Right:
            state_.move(CharacterDirection::right);
            break;
        case sf::Keyboard::Down:
            state_.move(CharacterDirection::down);
            break;
        case sf::Keyboard::Up:
            state_.move(CharacterDirection::up);
            break;
        default:
            break;
    }
}

void Graphics::handle_mouse() {
    // sf::Vector2i p = sf::Mouse::getPosition() - gui.getPosition();
    // uint32_t x = static_cast<uint32_t>(p.x) / gui.cell_size;
    // uint32_t y = static_cast<uint32_t>(p.y) / gui.cell_size;
}
