// Project Identifier: 950181F63D0A883F183EC0A5CC67B19928FE896A
#include <iostream>
#include "findPath.h"
using namespace std;

int main(int argc,char *argv[]){
    ios_base::sync_with_stdio(false);
    findPath path;
    Options options;
    path.readOptions(argc,argv,options);
    path.readMaze();
    path.getSolution(options);
    path.outputPath();
    return 0;
}