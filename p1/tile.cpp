// Project Identifier: 950181F63D0A883F183EC0A5CC67B19928FE896A
#include "tile.h"
using namespace std;

tile::tile(){
    type = '.';
    direction = ' ';
    discovered = false;
    isPath = false;
}