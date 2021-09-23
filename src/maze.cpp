#include "maze.h"

#include <algorithm>
#include <chrono>

static const float v_prob = 0.5;
static const float h_prob = 0.4;

Maze::Maze() {
    auto seed = std::chrono::system_clock::now().time_since_epoch().count();
    gen_.seed(seed);
    max_ = gen_.max();
}

void Maze::init(unsigned w, unsigned h) {
    width_ = w;
    height_ = h;
    v_walls_.resize((width_ + 1) * height_);
    h_walls_.resize(width_ * (height_ + 1));

    std::generate(v_walls_.begin(), v_walls_.end(), [this]{
        auto random = gen_();
        return random < max_ * v_prob;
    });
    std::generate(h_walls_.begin(), h_walls_.end(), [this]{
        auto random = gen_();
        return random < max_ * h_prob;
    });

    // ensure can't go outside of maze
    for (unsigned y = 0; y < height_; ++y) {
        v_walls_[(y * (width_+1))] = true;
        v_walls_[(y * (width_+1)) + width_] = true;
    }
    for (unsigned x = 0; x < width_; ++x) {
        h_walls_[x] = true;
        h_walls_[(height_ * width_) + x] = true;
    }
}

uint8_t Maze::getUDLR(unsigned x, unsigned y) const {
    if (x >= width_ || y >= height_) {
        return 0;
    }

    uint8_t ret = 0;
    if (!h_walls_[(y * width_) + x]) {
        ret |= UP;
    }
    if (!h_walls_[((y+1) * width_) + x]) {
        ret |= DOWN;
    }
    if (!v_walls_[(y * (width_ + 1)) + x]) {
        ret |= LEFT;
    }
    if (!v_walls_[(y * (width_ + 1)) + x + 1]) {
        ret |= RIGHT;
    }
       
    return ret;
}

void Maze::start_solve() {
    visited_.assign(width_ * height_, 0);
    queue_.clear();

    // start at middle of top row
    queue_.emplace_back(std::make_pair(width_ / 2, 0));
    visited_[width_ / 2] = 1;
}

std::tuple<bool, unsigned, unsigned> Maze::solve_step() {
 
    if (queue_.empty())
        return std::make_tuple(true, 0, 0);

    auto xy = queue_.front();
    queue_.pop_front();

    if (xy.second == height_ - 1) {
        queue_.clear();
        return std::make_tuple(true, xy.first, xy.second);
    }

    uint8_t moves = getUDLR(xy.first, xy.second);
    if (xy.second > 0 && (moves & UP)) {
        // can go up
        auto new_xy = std::make_pair(xy.first, xy.second-1);
        if (visited_[(width_ * new_xy.second) + new_xy.first] == 0) {
            visited_[(width_ * new_xy.second) + new_xy.first] = visited_[(width_ * xy.second) + xy.first] + 1;
            queue_.push_back(new_xy);
        }
    }
    if ((moves & DOWN)) {
        // can go down
        auto new_xy = std::make_pair(xy.first, xy.second+1);
        if (visited_[(width_ * new_xy.second) + new_xy.first] == 0) {
            visited_[(width_ * new_xy.second) + new_xy.first] = visited_[(width_ * xy.second) + xy.first] + 1;
            queue_.push_back(new_xy);
        }
    }
    if (xy.first > 0 && ((moves & LEFT))) {
        // can go left
        auto new_xy = std::make_pair(xy.first-1, xy.second);
        if (visited_[(width_ * new_xy.second) + new_xy.first] == 0) {
            visited_[(width_ * new_xy.second) + new_xy.first] = visited_[(width_ * xy.second) + xy.first] + 1;
            queue_.push_back(new_xy);
        }
    }
    if (xy.first < (width_ - 1) && ((moves & RIGHT))) {
        // can go right
        auto new_xy = std::make_pair(xy.first+1, xy.second);
        if (visited_[(width_ * new_xy.second) + new_xy.first] == 0) {
            visited_[(width_ * new_xy.second) + new_xy.first] = visited_[(width_ * xy.second) + xy.first] + 1;
            queue_.push_back(new_xy);
        }
    }
 
    return std::make_tuple(false, xy.first, xy.second);
}
