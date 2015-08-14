//
//  main.cpp
//  Cubes
//
//  Created by Patrick on 10/08/2015.
//  Copyright (c) 2015 Patrick. All rights reserved.
//

#include <iostream>
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include "board.h"
#include "vec.h"
#include <cstdlib>
#include <vector>
#include "arrows.h"
#include <cmath>
#include <fstream>

// Constants
int width = 800;
int height = 600;
double theta = 0.0;
double phi = 0.0;
double r = 30.0;
double eyeX = 0.0;
double eyeY = 0.0;
double eyeZ = r;
double upX = 0.0;
double upY = 1.0;
double upZ = 0.0;

Board b(1.0);
bool show_losing = false;

Arrows arrows;

void update_view()
{
    // Convert spherical to Cartesian coordinates
    double c = M_PI / 180.0;
    eyeX = r * sin(theta*c) * cos(phi*c);
    eyeY = r * sin(phi*c);
    eyeZ = r * cos(theta*c) * cos(phi*c);
    
    // Calculate up vector
    GLfloat dt = 1.0;
    GLfloat eyeXtemp = r * sin(theta*c) * cos(phi*c - dt);
    GLfloat eyeYtemp = r * sin(phi*c - dt);
    GLfloat eyeZtemp = r * cos(theta*c) * cos(phi*c - dt);
    upX = eyeX - eyeXtemp;
    upY = eyeY - eyeYtemp;
    upZ = eyeZ - eyeZtemp;
    
    // Define the projection transformation
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, width/(float)height, 0.1, r + b.max_size() * 1.5);
}

void onMouseMove(int x, int y)
{
    // Change mouse coordinates to the coordinate system with
    // the centre in the middle of the screen
    x = (x - width / 2);
    y = - (y - height / 2);
    
    // Convert mouse position to angles
    theta = -0.5 * 360.0 * x / 400.0;
    phi = -0.5 * 360.0 * y / 400.0;
    
    // Trim angles
    if (theta > 360)
        theta = fmod((double)theta, 360.0);
    if (phi > 360)
        phi = fmod((double)theta, 360.0);
    
    update_view();
    
    // Refresh the display
    glutPostRedisplay();
}


void display()
{
    // Clear window
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // Switch to the modelview matrix
    glMatrixMode(GL_MODELVIEW);
    
    glLoadIdentity();
    gluLookAt(eyeX, eyeY, eyeZ, 0.0, 0.0, 0.0, upX, upY, upZ);
    
    b.draw(show_losing);
    
    glutSwapBuffers();
    glFlush();
}

void reshape(int w, int h)
{
    width = w;
    height = h;
    
    // Define the viewport transformation
    glViewport(0, 0, width, height);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, width/(float)height, 0.1, 100);
}

void key_down(unsigned char key, int x, int y)
{
    if (key == 27)
        std::exit(0);
    else if (key == 13)
        b.solve();
    else if (key == 'l')
        show_losing = !show_losing;
    else if (key == 'x')
        b.explode(X);
    else if (key == 'y')
        b.explode(Y);
    else if (key == 'z')
        b.explode(Z);
    else if (key == 'r')
        b.reset();
}

void special_key_down(int key, int x, int y)
{
    // Remember which arrows were pressed
    if (key == GLUT_KEY_UP)
        arrows.up = true;
    if (key == GLUT_KEY_DOWN)
        arrows.down = true;
    if (key == GLUT_KEY_LEFT)
        arrows.left = true;
    if (key == GLUT_KEY_RIGHT)
        arrows.right = true;
}

void special_key_up(int key, int x, int y)
{
    if (key == GLUT_KEY_UP)
        arrows.up = false;
    if (key == GLUT_KEY_DOWN)
        arrows.down = false;
    if (key == GLUT_KEY_LEFT)
        arrows.left = false;
    if (key == GLUT_KEY_RIGHT)
        arrows.right = false;
}

void idle()
{
    if (arrows.up)
    {
        r /= 1.01;
        update_view();
    }
    else if (arrows.down)
    {
        r *= 1.01;
        update_view();
    }
    b.update();
    glutPostRedisplay();
}

void print_instructions()
{
    std::cout << "Controls:" << std::endl <<
    "  Up/Down arrow - zoom in/out" << std::endl <<
    "  x/y/z - explode/collapse the board in x/y/z directions" << std::endl <<
    "  L - toggle losing cubes on/off" << std::endl <<
    "  R - reset" << std::endl <<
    "  ENTER - Solve the board" << std::endl;
}

int process_file(std::string filename)
{
    std::ifstream file(filename.c_str());
    if (!file.is_open())
    {
        std::cerr << "Could not open " << filename << std::endl;
        return 1;
    }
    
    std::string line;
    while (getline(file, line))
    {
        if (line.substr(0, 7) == "NOMOVES")
        {
            char str[32];
            int n = sscanf(line.c_str(), "NOMOVES = %s", str);
            if (n == 1)
            {
                if (std::string(str) == "LOSE")
                    b.set_nomoves_win(false);
                else if (std::string(str) == "WIN")
                    b.set_nomoves_win(true);
                else
                {
                    std::cerr << "Error: " << line << std::endl;
                    return 1;
                }
            }
            else
            {
                std::cerr << "Error: " << line << std::endl;
                return 1;
            }
        }
        
        else if (line.substr(0, 4) == "MOVE")
        {
            int x, y, z, lim;
            int n = sscanf(line.c_str(), "MOVE %d %d %d LIM %d", &x, &y, &z, &lim);
            if (n == 4)
                b.add_move(x, y, z, lim);
            else
            {
                std::cerr << "Error: " << line << std::endl;
                return 1;
            }
        }
    }
    return 0;
}

int main (int argc, char** argv)
{
    if (argc != 2)
    {
        std::cout << "Usage: ./cubes file" << std::endl;
        std::exit(1);
    }
    int res = process_file(argv[1]);
    if (res != 0)
    {
        std::exit(1);
    }
    
    // Initiallize GLUT
    glutInit(&argc, argv);
    
    // Setup for the new window
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(width, height);
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
    
    // Create a window
    glutCreateWindow("Cubes");
    
    // Register callback functions
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(key_down);
    glutSpecialFunc(special_key_down);
    glutSpecialUpFunc(special_key_up);
    glutIdleFunc(idle);
    glutPassiveMotionFunc(onMouseMove);
    
    // Set up depth-buffering
    glEnable(GL_DEPTH_TEST);
    
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glEnable(GL_POINT_SMOOTH);
    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(eyeX, eyeY, eyeZ, 0.0, 0.0, 0.0, upX, upY, upZ);
    
    // Define the projection transformation
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, width/(float)height, 0.1, 100);
    
    print_instructions();
    b.solve();
    
    // Enter the main loop
    glutMainLoop();
}

