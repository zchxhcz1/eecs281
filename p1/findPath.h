// Project Identifier: 950181F63D0A883F183EC0A5CC67B19928FE896A
#ifndef _FINDPATH_H_
#define _FINDPATH_H_
#include<iostream>
#include<vector>
#include<deque>
#include <getopt.h>
#include <string>
#include "container.h"
#include "tile.h"
using namespace std;

struct Options {
    bool use_stack = false;
    bool use_queue = false;
    //char output_mode = 'M'; // Default output mode
};

class findPath{
private:
    vector<vector<vector<tile>>> maze;
    container start;
    container end;
    container cur;
    uint32_t tolLen;
    uint32_t tolLevel;
    char output_mode = 'M';
    char input_mode;
public:
    deque<container> searchContainer;
    bool success = false;
    void readOptions(int argc, char *argv[], Options &options);
    void readMaze();
    findPath();
    void getSolution(Options &options);
    void outputPath();
};
#endif