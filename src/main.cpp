#include "maze.h"

#include <curses.h>
#include <signal.h>
#include <chrono>
#include <thread>
#include <algorithm>
#include <iostream>

static void cleanUp(int sig) {
    endwin();
    exit(0);
}

unsigned MovesToChar(uint8_t moves) {
    const unsigned all[] = {
        ' ', ACS_VLINE, ACS_VLINE, ACS_VLINE,
        ACS_HLINE, ACS_LRCORNER, ACS_URCORNER, ACS_RTEE,
        ACS_HLINE, ACS_LLCORNER, ACS_ULCORNER, ACS_LTEE,
        ACS_HLINE, ACS_BTEE, ACS_TTEE, ACS_PLUS
    };

    return all[moves];
}

void draw_maze(const Maze& maze) {
    attrset(A_DIM);
    for (unsigned y = 0; y < maze.height(); ++y) {
        for (unsigned x = 0; x < maze.width(); ++x) {
            uint8_t moves = maze.getUDLR(x, y);
            mvaddch(y, x, MovesToChar(moves));
        }
    }
}

void draw_path(const std::vector<std::pair<unsigned, unsigned>>& path) {
    attrset(A_REVERSE);
    std::for_each(path.begin(), path.end(), [](auto a) {
        mvaddch(a.first, a.second, ' ');
    });
}

int main(int argc, char *argv[]) {
    signal(SIGINT, cleanUp);

    initscr();
    curs_set(0);

    Maze maze;
    int width = getmaxx(stdscr);
    int height = getmaxy(stdscr);
    maze.init(width, height);
    draw_maze(maze);

    maze.start_solve();
    bool solved = false;
    unsigned x, y;
    size_t n = 1;
    std::list<std::pair<unsigned, unsigned>> my_list;

    while (!solved) {
        auto [s, _x, _y] = maze.solve_step();
        solved = s;
        x = _x;
        y = _y;

        if (--n == 0) {
            n = maze.queueSize();
            if (n == 0)
                break;

            // highlight progress
            attrset(A_DIM);
            draw_maze(maze);
            attrset(A_REVERSE);
            std::for_each(my_list.begin(), my_list.end(), [](auto a) {
                mvaddch(a.first, a.second, ' ');
            });
            refresh();
            std::this_thread::sleep_for(std::chrono::milliseconds(100));    
            my_list.clear();
        }
        my_list.emplace_back(y, x);
    }
    if (solved) {
        // annimate lightening
        std::vector<std::pair<unsigned, unsigned>> path;
        auto a = std::back_inserter(path);
        maze.back_track(x, y, a);
        draw_maze(maze);
        draw_path(path);
        refresh();
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        draw_maze(maze);
        refresh();
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        draw_path(path);
        refresh();
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    curs_set(1);
    endwin();
    if (!solved)
        std::cout << "The randomly generated maze couldn't be solved.\n";
    return 0;
}

