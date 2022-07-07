#include "gamelogic.h"
#include "graphics.h"

#include <iostream>

int request_level() {
    std::cout << "Choose level(1-24):\n";
    int level;
    std::cin >> level;
    return level;
}

int main() {
    int level = request_level();
    GameState state(level);
    Graphics gui(state);

    sf::Clock clock;

    while (gui.isOpen()) {
        gui.update();
    }
    std::cout << "resolution " << gui.getSize().x << 'x' << gui.getSize().y << '\n';
    std::cout << "duration   " << clock.getElapsedTime().asMilliseconds() << '\n';
}
