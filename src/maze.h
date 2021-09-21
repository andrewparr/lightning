#ifndef MAZE_H
#define MAZE_H

#include <vector>
#include <random>
#include <list>
#include <utility>
#include <tuple>

class Maze {
public:
    Maze();
    void init(unsigned w, unsigned h);

    // get possible moves from (x,y) returns bit map: up, down, left, right moves
    uint8_t getUDLR(unsigned x, unsigned y) const;

    void start_solve();
    std::tuple<bool, unsigned, unsigned> solve_step();

    template<typename iterator>
    void back_track(unsigned x, unsigned y, iterator iter) const;

    size_t queueSize() const {return queue_.size();}
    unsigned width() const {return width_;}
    unsigned height() const {return height_;}

private:

    std::default_random_engine gen_;
    std::default_random_engine::result_type max_;

    unsigned width_;
    unsigned height_;
    std::vector<bool> v_walls_;
    std::vector<bool> h_walls_;

    std::vector<int> visited_;
    std::list<std::pair<unsigned, unsigned>> queue_;
};

template<typename iterator>
void Maze::back_track(unsigned xx, unsigned yy, iterator iter) const {
    unsigned cur = visited_[(yy * width_) + xx];
    *iter = std::make_pair(yy, xx);
    while (--cur > 0) {
        if (yy > 0 && visited_[((yy-1) * width_) + xx] == cur)
            --yy;
        else if (xx > 0 && visited_[(yy * width_) + xx - 1] == cur)
            --xx;
        else if (xx < width_ && visited_[(yy * width_) + xx + 1] == cur)
            ++xx;
        else if (yy < height_ && visited_[((yy+1) * width_) + xx] == cur)
            ++yy;
        *iter = std::make_pair(yy, xx);
    }
}


#endif