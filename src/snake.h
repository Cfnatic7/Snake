//
// Created by micha on 09.01.2022.
//

#ifndef SNAKE_SNAKE_H
#define SNAKE_SNAKE_H

#include "winsys.h"
#include <ctime>
#include <vector>
#include <list>
#include <thread>
#include <unistd.h>
#include <mutex>

class actual_snake {
private:
    CRect view;
    friend class CSnake;
	useconds_t time_interval_between_moves;
    vector<CPoint> busy_points;
    vector<unsigned char> wavelike_propagation;
    CPoint cookie;
    bool game_lost;
    unsigned int length;
    enum direction_t {
        MOVE_UP, MOVE_DOWN, MOVE_RIGHT, MOVE_LEFT
    };
    enum direction_t general_direction;
    struct segment_t {
        CPoint point;
        char c;
        enum direction_t direction;
        struct segment_t *next_segment;
        struct segment_t *prev_segment;
    };
    struct segment_t *head;
public:
    actual_snake(CRect geom);

    ~actual_snake();

    void paint();

    bool handleEvent(int key);

    bool move(enum direction_t direction);
    
    void update_direction(enum direction_t new_general_direction);

    void push_back();

    void respawn_cookie();
    
    void tidal_shift();
};

class CSnake : public CFramedWindow {
private:
	bool game_start;
//	enum direction {MOVE_UP, MOVE_DOWN, MOVE_LEFT, MOVE_RIGHT};
    enum mode_t {
        PAUSE_MODE, PLAY_MODE, HELP_MODE, RESTART_MODE, QUIT
    };
    enum mode_t mode;
    thread *game;
    actual_snake *snake;
    const char* manual1 = "h - toggle help information";
    const char* manual2 = "p - toggle pause/play mode";
    const char* manual3 = "r - restart game";
    const char* manual4 = "arrows - move snake/move window";
public:
    CSnake(CRect r, char _c = ' ');

    ~CSnake();

    void paint();

    bool handleEvent(int key);

    void play();

    void restart();
};



#endif //SNAKE_SNAKE_H
