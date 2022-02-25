//
// Created by micha on 09.01.2022.
//

#include "snake.h"
#include "screen.h"

using namespace std;

actual_snake::actual_snake(CRect geom) {
    srand(time(NULL));
    this->general_direction = MOVE_RIGHT;
    this->game_lost = false;
    time_interval_between_moves = 1200000;
    this->view = geom;
    this->cookie = CPoint(geom.topleft.x + geom.size.x / 2, geom.topleft.y + geom.size.y / 2);
    this->length = 3;
    this->head = new segment_t();
    this->head->direction = MOVE_RIGHT;
    this->head->c = '*';
    this->head->point = CPoint(view.topleft.x + this->length + 1, view.topleft.y + 1);
    this->head->prev_segment = nullptr;
    this->head->next_segment = nullptr;
    busy_points.push_back(this->head->point);
    this->wavelike_propagation.push_back(0);
    struct segment_t *temp = this->head;
    for (unsigned int i = 0; i < this->length - 1; i++) {
        temp->next_segment = new segment_t();
        temp->next_segment->c = '+';
        temp->next_segment->direction = MOVE_RIGHT;
        temp->next_segment->prev_segment = temp;
        temp->next_segment->next_segment = nullptr;
        temp->next_segment->point = CPoint(head->point.x - i - 1, head->point.y);
        busy_points.push_back(temp->next_segment->point);
        temp = temp->next_segment;
        this->wavelike_propagation.push_back(0);
    }
}

actual_snake::~actual_snake() {
    struct segment_t *temp = head, *temp_v2;
    while (temp) {
        temp_v2 = temp->next_segment;
        delete temp;
        temp = temp_v2;
    }
}

bool actual_snake::handleEvent(int key) {
	if (game_lost) return false;
    switch (key) {
        case KEY_UP:
            if (this->general_direction == MOVE_DOWN || this->general_direction == MOVE_UP) return false;
            update_direction(MOVE_UP);
            return true;
        case KEY_DOWN:
            if (this->general_direction == MOVE_DOWN || this->general_direction == MOVE_UP) return false;
            update_direction(MOVE_DOWN);
            return true;
        case KEY_RIGHT:
            if (this->general_direction == MOVE_LEFT || this->general_direction == MOVE_RIGHT) return false;
            update_direction(MOVE_RIGHT);
            return true;
        case KEY_LEFT:
            if (this->general_direction == MOVE_LEFT || this->general_direction == MOVE_RIGHT) return false;
            update_direction(MOVE_LEFT);
            return true;
    };
    return false;
}

bool actual_snake::move(enum direction_t direction) {
    struct segment_t *temp = head;
    tidal_shift();
    if (head->direction != direction) {
		wavelike_propagation[0] = 1;
	}
	else wavelike_propagation[0] = 0;
	enum direction_t last_head_direction = head->direction;
	head->direction = direction;
	while(temp->next_segment) {
		temp = temp->next_segment;
	}
    int i = this->length - 1;
    while (i >= 0) {
		if (!temp->next_segment) {
			gotoyx(temp->point.y, temp->point.x);
			printl("%c", ' ');
		}
		if (wavelike_propagation[i] != 0 && i != 0) {
			if (i == 1) {
				temp->direction = last_head_direction;
			}
			else temp->direction = temp->prev_segment->direction;
		}
        if (temp->direction == MOVE_UP) {
            temp->point += CPoint(0, -1);
        } else if (temp->direction == MOVE_DOWN) {
            temp->point += CPoint(0, 1);
        } else if (temp->direction == MOVE_RIGHT) {
            temp->point += CPoint(1, 0);
        } else if (temp->direction == MOVE_LEFT) {
            temp->point += CPoint(-1, 0);
        }
        if (temp->point.x == this->view.topleft.x) {
            temp->point.x = this->view.topleft.x + this->view.size.x - 2;
        } else if (temp->point.x == this->view.topleft.x + this->view.size.x - 1) {
            temp->point.x = this->view.topleft.x + 1;
        } else if (temp->point.y == this->view.topleft.y) {
            temp->point.y = this->view.topleft.y + this->view.size.y - 2;
        } else if (temp->point.y == this->view.topleft.y + this->view.size.y - 1) {
            temp->point.y = this->view.topleft.y + 1;
        }
        busy_points[i] = temp->point;
        i--;
        temp = temp->prev_segment;
    }
    if (this->head->point.x == this->cookie.x && this->head->point.y == this->cookie.y) {
        push_back();
        respawn_cookie();
    }
    for (unsigned int k = 1; k < busy_points.size(); k++) {
		if (head->point.x == busy_points[k].x && head->point.y == busy_points[k].y) return true;
	}
	return false;
}

void actual_snake::paint() {
    struct segment_t* temp = head;
    while(temp) {
        gotoyx(temp->point.y, temp->point.x);
        printl("%c", temp->c);
        temp = temp->next_segment;
    }
	gotoyx(cookie.y, cookie.x);
	printl("%c", '0');
    gotoyx(this->view.topleft.y - 1, this->view.topleft.x);
    printl("level %d", this->length - 2);
}

void actual_snake::push_back() {
    struct segment_t *temp = head;
    while(temp->next_segment) {
        temp = temp->next_segment;
    }
    temp->next_segment = new segment_t();
    temp->next_segment->next_segment = nullptr;
    temp->next_segment->prev_segment = temp;
    temp->next_segment->c = '+';
    temp->next_segment->direction = temp->direction;
    if (temp->direction == MOVE_UP) {
        temp->next_segment->point = CPoint(temp->point.x, temp->point.y + 1);
    }
    else if (temp->direction == MOVE_DOWN) {
        temp->next_segment->point = CPoint(temp->point.x, temp->point.y - 1);
    }
    else if (temp->direction == MOVE_RIGHT) {
        temp->next_segment->point = CPoint(temp->point.x - 1, temp->point.y);
    }
    else {
        temp->next_segment->point = CPoint(temp->point.x + 1, temp->point.y);
    }
    if (temp->next_segment->point.x == this->view.topleft.x) {
        temp->next_segment->point.x = this->view.topleft.x + this->view.size.x - 2;
    } else if (temp->next_segment->point.x == this->view.topleft.x + this->view.size.x - 1){
        temp->next_segment->point.x = this->view.topleft.x + 1;
    } else if (temp->next_segment->point.y == this->view.topleft.y) {
        temp->next_segment->point.y = this->view.topleft.y + this->view.size.y - 2;
    } else if (temp->next_segment->point.y == this->view.topleft.y + this->view.size.y - 1) {
        temp->next_segment->point.y = this->view.topleft.y + 1;
    }
    busy_points.push_back(temp->next_segment->point);
    wavelike_propagation.push_back(0);
    if (time_interval_between_moves > 100000) {
		time_interval_between_moves -= 100000;
	}
    this->length++;
}

void actual_snake::respawn_cookie() {
    srand(time(NULL));
    this->cookie.x = (rand() % (view.size.x - 1)) + view.topleft.x  + 1;
    this->cookie.y = (rand() % (view.size.y - 1)) + view.topleft.y  + 1;
}

void actual_snake:: tidal_shift() {
	for (int i = wavelike_propagation.size() - 1; i >= 1; i--) {
		wavelike_propagation[i] = wavelike_propagation[i - 1];
	}
}

void actual_snake::update_direction(enum direction_t new_general_direction) {
	this->general_direction = new_general_direction;
}




CSnake::CSnake(CRect r, char _c /*=' '*/) :
        CFramedWindow(r, _c), game(nullptr) {
    mode = HELP_MODE;
    game_start = false;
    this->snake = new actual_snake(this->geom);
}

CSnake::~CSnake() {
	this->mode = QUIT;
	game->join();
	if (game) delete game;
    delete this->snake;
}

void CSnake::paint() {
    CFramedWindow::paint();
    if (this->mode == HELP_MODE) {
        gotoyx(this->geom.topleft.y + 4, this->geom.topleft.x + 4);
        printl("%s", this->manual1);
        gotoyx(this->geom.topleft.y + 5, this->geom.topleft.x + 4);
        printl("%s", this->manual2);
        gotoyx(this->geom.topleft.y + 6, this->geom.topleft.x + 4);
        printl("%s", this->manual3);
        gotoyx(this->geom.topleft.y + 7, this->geom.topleft.x + 4);
        printl("%s", this->manual4);
    } else if (this->mode == PAUSE_MODE) {
		gotoyx(this->geom.topleft.y + 7, this->geom.topleft.x + 17);
		printl("game paused");
    }
    else {
        this->snake->paint();
    }
}

bool CSnake::handleEvent(int key) {
    if (key == 'h' || key == 'H') {
		if (snake->game_lost) return false;
        if (mode == HELP_MODE) {
            this->mode = PLAY_MODE;
            if (!game) {
				game = new thread(&CSnake::play, this);
			}
        } else if (mode == PAUSE_MODE) {
			if (game) {
				game->join();
				delete game;
				game = nullptr;
			}
            this->mode = HELP_MODE;
        } else {
            this->mode = HELP_MODE;
            if (game) {
				game->join();
				delete game;
				game = nullptr;
			}
        }
        return true;
    } else if (key == 'p' || key == 'P') {
		if (snake->game_lost) return false;
        if (mode == PAUSE_MODE) {
			if (!game) {
				game = new thread(&CSnake::play, this);
			}
            this->mode = PLAY_MODE;
        } else if (mode == HELP_MODE) {
            this->mode = PAUSE_MODE;
            if (game) {
				game->join();
				delete game;
				game = nullptr;
			}
        } else {
            this->mode = PAUSE_MODE;
            if (game) {
				game->join();
				delete game;
				game = nullptr;
			}
        }
        return true;
    } else if (key == 'r' || key == 'R') {
        restart();
        return true;
    } else if (key == KEY_UP || key == KEY_DOWN || key == KEY_RIGHT || key == KEY_LEFT) {
		if (snake->game_lost) return false;
        if (this->mode == PAUSE_MODE || this->mode == HELP_MODE) {
            CWindow::handleEvent(key);
            int delta_x = this->geom.topleft.x - this->snake->view.topleft.x;
            int delta_y = this->geom.topleft.y - this->snake->view.topleft.y;
            this->snake->cookie.x = this->snake->cookie.x + delta_x;
            this->snake->cookie.y= this->snake->cookie.y + delta_y;
            struct actual_snake::segment_t *temp = this->snake->head;
            unsigned int i = 0;
            while(temp) {
				snake->busy_points[i].x += delta_x;
				snake->busy_points[i].y += delta_y;
                temp->point.x += delta_x;
                temp->point.y += delta_y;
                temp = temp->next_segment;
                i++;
            }
            this->snake->view.topleft.x += delta_x;
            this->snake->view.topleft.y += delta_y;
        }
        else {
			snake->handleEvent(key);
		}
        return true;
    }
    return false;
}

void CSnake::play(){
	while(1) {
		if (this->mode == PLAY_MODE && this->snake->game_lost == false) {
			this->snake->game_lost = snake->move(this->snake->general_direction);
			if (this->snake->game_lost) {
				gotoyx(this->geom.topleft.y + 7, this->geom.topleft.x + 4);
				printl("score: %d, press r to restart", snake->length - 2);
				return ;
			}
			snake->paint();
			usleep(snake->time_interval_between_moves);
		}
		else return;
	}
}

void CSnake::restart(){
	this->mode = RESTART_MODE;
	if (game) {
		game->join();
	delete game;
	}
	delete this->snake;
    snake = new actual_snake(this->geom);
    snake->game_lost = false;
    this->mode = PLAY_MODE;
    game = new thread(&CSnake::play, this);
}
