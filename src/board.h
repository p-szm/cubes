//
//  board.h
//  Cubes
//
//  Created by Patrick on 10/08/2015.
//  Copyright (c) 2015 Patrick. All rights reserved.
//

#ifndef __Cubes__board__
#define __Cubes__board__

#include <stdio.h>
#include <vector>
#include "move.h"

#define SIZE 16

enum Field {EMPTY, WIN, LOSE, WILLWIN, WILLLOSE};
enum Axis {X, Y, Z};

class Board
{
public:
    Board(double a);
    double a;
    void set_nomoves_win(bool val);
    
    void explode(Axis axis);
    
    void add_move(int x, int y, int z, int lim);
    
    double max_size();
    double base_size();
    
    void solve();
    void reset();
    
    void update();
    void draw(bool show_losing);
    
private:
    Field data[SIZE][SIZE][SIZE];
    std::vector<Move> moves;
    bool nomoves_win;
    bool nomoves_win_set;
    bool solved;
    
    double separation_x;
    double separation_y;
    double separation_z;
    
    double target_separation_x;
    double target_separation_y;
    double target_separation_z;
    
    void update_separation(double &sep, double target_sep);
    void explode_axis(double &target_sep);
    
    int current_layer;
    
    bool bounded(int k);
};

#endif /* defined(__Cubes__board__) */
