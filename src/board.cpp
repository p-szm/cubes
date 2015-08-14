//
//  board.cpp
//  Cubes
//
//  Created by Patrick on 10/08/2015.
//  Copyright (c) 2015 Patrick. All rights reserved.
//

#include "board.h"
#include <iostream>
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include "mathematics.h"

Board::Board(double a): a(a)
{
    nomoves_win = true;
    nomoves_win_set = false;
    reset();
}

bool Board::bounded(int k)
{
    if (k >= 0 && k < SIZE)
        return true;
    return false;
}

void Board::add_move(int x, int y, int z, int lim)
{
    Move move(Vec3(x, y, z), lim);
    moves.push_back(move);
}

void Board::set_nomoves_win(bool val)
{
    nomoves_win = val;
    if (nomoves_win)
        data[0][0][0] = WIN;
    else
        data[0][0][0] = LOSE;
    nomoves_win_set = true;
}

void Board::solve()
{
    if (!nomoves_win_set)
        return;
    
    bool stop = false;
    for (int i = 0; i < SIZE && !stop; i++)
    {
        for (int j = 0; j < SIZE && !stop; j++)
        {
            for (int k = 0; k < SIZE && !stop; k++)
            {
                // Already filled in
                if (data[i][j][k] != EMPTY)
                    continue;
                
                // Make a list of all possible moves
                std::vector<Field> vals;
                for (int n = 0; n < moves.size(); n++)
                {
                    Move move = moves[n];
                    int lim = (move.limit == -1) ? SIZE : move.limit;
                    for (int m = 1; m < lim+1; m++)
                    {
                        if (!bounded(i+m*move.x) || !bounded(j+m*move.y) || !bounded(k+m*move.z))
                            break;
                        Field val = data[i+m*move.x][j+m*move.y][k+m*move.z];
                        vals.push_back(val);
                    }
                }
                
                bool no_moves = false;
                if (vals.size() == 0)
                {
                    if (nomoves_win)
                        data[i][j][k] = WIN;
                    else
                        data[i][j][k] = LOSE;
                    no_moves = true;
                }
                if (no_moves)
                    continue;
                
                // See if there is a winning cube reachable
                bool win_found = false;
                for (int n = 0; n < vals.size() && !win_found; n++)
                {
                    Field f = vals[n];
                    if (f == WIN || f == WILLWIN)
                    {
                        data[i][j][k] = WILLLOSE;
                        win_found = true;
                    }
                }
                if (win_found)
                    continue;
                

                // See if all reachable squares are loosing
                bool all_lose = true;
                for (int n = 0; n < vals.size() && all_lose; n++)
                {
                    Field f = vals[n];
                    if (f != WILLLOSE && f != LOSE)
                    {
                        all_lose = false;
                    }
                }
                if (all_lose)
                    data[i][j][k] = WILLWIN;
                else
                {
                    std::cout << "Cube (" << i << ' ' << j << ' ' << k
                    << ") could not be completed" << std::endl;
                    stop = true;
                }
            }
        }
    }
    solved = true;
}

void Board::update_separation(double &sep, double target_sep)
{
    double delta_sep = target_sep - sep;
    if (abs_d(delta_sep / target_sep) < 0.01)
        sep = target_sep;
    else
        sep += 0.05 * (target_sep - sep);
}

void Board::update()
{
    update_separation(separation_x, target_separation_x);
    update_separation(separation_y, target_separation_y);
    update_separation(separation_z, target_separation_z);
}

double Board::max_size()
{
    double max_sep = max(separation_x, max(separation_y, separation_z));
    return a * SIZE + (SIZE-1) * max_sep;
}

double Board::base_size()
{
    return a * SIZE;
}

void Board::explode_axis(double &target_sep)
{
    if (target_sep > 0.0)
        target_sep = 0.0;
    else
        target_sep = 2.0;
}

void Board::explode(Axis axis)
{
    if (axis == X)
        explode_axis(target_separation_x);
    else if (axis == Y)
        explode_axis(target_separation_y);
    else
        explode_axis(target_separation_z);
}

void Board::reset()
{
    for (int i = 0; i < SIZE; i++)
    {
        for (int j = 0; j < SIZE; j++)
        {
            for (int k = 0; k < SIZE; k++)
            {
                data[i][j][k] = EMPTY;
            }
        }
    }
    
    if (nomoves_win_set)
        set_nomoves_win(nomoves_win);
    
    solved = false;
    current_layer = 0;
    
    separation_x = 0.0;
    separation_y = 0.0;
    separation_z = 0.0;
    target_separation_x = 0.0;
    target_separation_y = 0.0;
    target_separation_z = 0.0;
}

void Board::draw(bool show_losing)
{
    for (int i = 0; i < SIZE; i++)
    {
        for (int j = 0; j < SIZE; j++)
        {
            for (int k = 0; k < SIZE; k++)
            {
                Field f = data[i][j][k];
                
                if ((i > current_layer || j > current_layer || k > current_layer))
                    continue;
                
                Vec3 color;
                if (f == EMPTY)
                    color = Vec3(0, 0, 0);
                else if (f == WIN)
                    color = Vec3(0, 0.5, 0);
                else if (f == LOSE)
                    color = Vec3(1, 0, 0);
                else if (f == WILLWIN)
                    color = Vec3(0.3, 0.8, 0.3);
                else // (if f == WiLLLOSE)
                    color = Vec3(1, 0.8, 0.8);
                
                bool fill;
                bool edges;
                if (f == WILLLOSE)
                {
                    fill = show_losing;
                    edges = show_losing;
                }
                else if (f == WIN || f == LOSE || f == WILLWIN)
                {
                    fill = true;
                    edges = true;
                }
                else
                {
                    fill = false;
                    edges = false;
                }
                
                // Draw a cube
                Vec3 centre((i-SIZE/2.0+0.5)*(a+separation_x),
                            (j-SIZE/2.0+0.5)*(a+separation_y),
                            (k-SIZE/2.0+0.5)*(a+separation_z));
                
                glPushMatrix();
                glTranslated(centre.x, centre.y, centre.z);
                
                if (fill)
                {
                    glColor4f(color.x, color.y, color.z, 1);
                    glutSolidCube(a*0.98);
                }
                if (edges)
                {
                    glColor3f(0.0, 0.0, 0.0);
                    glLineWidth(1.0);
                    glutWireCube(a);
                }
                glPopMatrix();
            }
        }
    }
    if (current_layer < SIZE and solved)
        current_layer ++;
    
    // Draw the boundary
    glColor3f(0.0, 0.0, 0.0);
    glLineWidth(0.5);
    glutWireCube(base_size());
}