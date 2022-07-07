#pragma once

#include "config.h"

#include <deque>
#include <memory>
#include <vector>
#include <unordered_map>
class Solver;
class GameState {
public:
    GameState(int level);
    uint32_t get_width() const {
        return width_;
    }
    uint32_t get_height() const {
        return height_;
    }
    CharacterDirection get_character_direction() const {
        return dir_;
    }
    int get_moves_number() const {
        return moves_number_;
    }
    char at(int x, int y) const {
        return field_[x + y * width_];
    }
    char& at(int x, int y) {
        return field_[x + y * width_];
    }
    void restore_field();
    void move(CharacterDirection dir);
    bool operator==(const GameState& another) const {
        return field_ == another.field_ && width_ == another.width_;
    }
    bool operator!=(const GameState& another) const {
        return field_ != another.field_ || width_ != another.width_;
    }
    bool is_solved() const {
        return !crates_left_;
    }

private:
    void fix_character_position();
    bool fits_into_borders(int x, int y) const;
    void extract_character();
    void fix_crates_left_number();

    friend std::hash<GameState>;
    friend Solver;

    int width_, height_, moves_number_, crates_left_, px_, py_;
    CharacterDirection dir_;
    std::shared_ptr<std::string> start_field_;
    std::string field_;
};

template <>
struct std::hash<GameState> {
    size_t operator()(const GameState& state) const {
        return std::hash<std::string>()(state.field_) ^ static_cast<size_t>(state.width_);
    }
};

class Solver {
public:
    void find_shortest_path(const GameState& from);
    bool has_finished() const;
    CharacterDirection pop_move();

private:
    using Map = std::unordered_map<GameState, std::pair<const GameState*, CharacterDirection>>;
    using Queue = std::deque<const GameState*>;

    static bool check_state(Map& prev, Queue& queue, CharacterDirection dir);

    std::vector<CharacterDirection> moves_;
};
