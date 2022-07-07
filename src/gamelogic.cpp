#include "gamelogic.h"

#include <cassert>
#include <fstream>
#include <vector>

GameState::GameState(int level)
    : width_(0), height_(0), dir_(CharacterDirection::down), start_field_(new std::string) {
    std::ifstream fs(config::levels_path + std::to_string(level) + ".txt");
    assert(fs);
    std::string str;
    std::vector<std::string> field_vec;
    while (std::getline(fs, str) && !str.empty()) {
        field_vec.push_back(str);
        ++height_;
        width_ = std::max(width_, static_cast<int>(str.size()));
    }
    width_ += 2, height_ += 2;
    start_field_->reserve(width_ * height_);
    start_field_->append(width_, ' ');
    for (std::string& s : field_vec) {
        start_field_->push_back(' ');
        s.resize(width_ - 2, ' ');
        start_field_->append(s);
        start_field_->push_back(' ');
    }
    start_field_->append(width_, ' ');
    restore_field();
    assert(px_ != 0 && px_ + 1 != width_ && py_ != 0 && py_ + 1 != height_);
}

void GameState::restore_field() {
    field_ = *start_field_;
    moves_number_ = 0;
    fix_crates_left_number();
    fix_character_position();
    dir_ = CharacterDirection::down;
}

void GameState::move(CharacterDirection dir) {
    dir_ = dir;
    int next1_x = px_, next1_y = py_, next2_x = px_, next2_y = py_;
    switch (dir) {
        case CharacterDirection::left:
            next1_x -= 1;
            next2_x -= 2;
            break;
        case CharacterDirection::right:
            next1_x += 1;
            next2_x += 2;
            break;
        case CharacterDirection::down:
            next1_y += 1;
            next2_y += 2;
            break;
        case CharacterDirection::up:
            next1_y -= 1;
            next2_y -= 2;
            break;
        default:
            assert(false);
            break;
    }
    if (!fits_into_borders(next1_x, next1_y)) {
        return;
    }
    char next1 = at(next1_x, next1_y), next2 = '\0';
    if (fits_into_borders(next2_x, next2_y)) {
        next2 = at(next2_x, next2_y);
    }
    if (next1 != ' ' && next1 != '.' &&
        !((next1 == '$' || next1 == '*') && fits_into_borders(next2_x, next2_y) &&
          (next2 == ' ' || next2 == '.'))) {
        return;
    }
    ++moves_number_;
    extract_character();
    switch (next1) {
        case ' ':
            at(next1_x, next1_y) = '@';
            break;
        case '.':
            at(next1_x, next1_y) = '+';
            break;
        case '$':
            at(next1_x, next1_y) = '@';
            switch (next2) {
                case ' ':
                    at(next2_x, next2_y) = '$';
                    break;
                case '.':
                    at(next2_x, next2_y) = '*';
                    --crates_left_;
                    break;
                default:
                    assert(false);
                    break;
            }
            break;
        case '*':
            at(next1_x, next1_y) = '+';
            switch (next2) {
                case ' ':
                    at(next2_x, next2_y) = '$';
                    ++crates_left_;
                    break;
                case '.':
                    at(next2_x, next2_y) = '*';
                    break;
                default:
                    assert(false);
                    break;
            }
            break;
        default:
            assert(false);
            break;
    }
    switch (dir) {
        case CharacterDirection::left:
            --px_;
            break;
        case CharacterDirection::right:
            ++px_;
            break;
        case CharacterDirection::down:
            ++py_;
            break;
        case CharacterDirection::up:
            --py_;
            break;
        default:
            assert(false);
            break;
    }
}

void GameState::fix_character_position() {
    int x = 0, y = 0;
    bool found = false;
    for (; y != height_; ++y) {
        for (x = 0; x != width_; ++x) {
            if (at(x, y) == '@' || at(x, y) == '+') {
                px_ = x, py_ = y;
                assert(!found);
                found = true;
            }
        }
    }
    assert(found);
}

bool GameState::fits_into_borders(int x, int y) const {
    return x >= 0 && x < width_ && y >= 0 && y < height_;
}

void GameState::extract_character() {
    switch (at(px_, py_)) {
        case '@':
            at(px_, py_) = ' ';
            break;
        case '+':
            at(px_, py_) = '.';
            break;
        default:
            assert(false);
            break;
    }
}

void GameState::fix_crates_left_number() {
    crates_left_ = 0;
    for (int y = 0; y != height_; ++y) {
        for (int x = 0; x != width_; ++x) {
            if (at(x, y) == '$') {
                ++crates_left_;
            }
        }
    }
}

void Solver::find_shortest_path(const GameState& from) {
    Map prev;
    prev.insert({from, {nullptr, CharacterDirection::undefined}});
    Queue queue;
    queue.push_back(&prev.begin()->first);
    while (true) {
        if (check_state(prev, queue, CharacterDirection::left)) {
            break;
        }
        if (check_state(prev, queue, CharacterDirection::right)) {
            break;
        }
        if (check_state(prev, queue, CharacterDirection::down)) {
            break;
        }
        if (check_state(prev, queue, CharacterDirection::up)) {
            break;
        }
        queue.pop_front();
    }
    std::pair<const GameState*, CharacterDirection> t = prev.at(*queue.back());
    while (t.first) {
        moves_.push_back(t.second);
        t = prev.at(*t.first);
    }
}

bool Solver::has_finished() const {
    return moves_.empty();
}

CharacterDirection Solver::pop_move() {
    CharacterDirection dir = moves_.back();
    moves_.pop_back();
    return dir;
}

bool Solver::check_state(Map& prev, Queue& queue, CharacterDirection dir) {
    GameState moved = *queue.front();
    moved.move(dir);
    auto it = prev.find(moved);
    if (it == prev.end()) {
        const GameState* ptr = &(prev.insert({moved, {queue.front(), dir}}).first->first);
        queue.push_back(ptr);
    }
    return queue.back()->is_solved();
}
