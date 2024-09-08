// Project Identifier: 950181F63D0A883F183EC0A5CC67B19928FE896A
#include<iostream>
#include<vector>
#include<deque>
#include <getopt.h>
#include <string>
#include "container.h"
#include "findPath.h"
using namespace std;

findPath::findPath(){
    tolLen = 0;
    tolLevel = 0;
    output_mode = 'M';
};

void findPath::readOptions(int argc, char *argv[], Options &options){
    opterr = false;
    int choice;
    int index = 0;

    option long_options[] = {
        {"stack", no_argument, nullptr, 's' },
        {"queue", no_argument, nullptr, 'q' },
        {"output", required_argument, nullptr, 'o' },
        {"help", no_argument, nullptr, 'h' },
        { nullptr, 0, nullptr, '\0' },
    };

    while ((choice = getopt_long(argc, argv, "sqo:h", long_options, &index)) != -1) {
        //cout << "choice: " << choice << endl;
        switch (choice) {
            case 's':
                if (options.use_queue || options.use_stack) {
                    cerr << "Multiple routing modes specified" << '\n';
                    exit(1);
                }
                options.use_stack = true;
                break;
            case 'q':   
                if (options.use_queue || options.use_stack) {
                    cerr << "Multiple routing modes specified" << '\n';
                    exit(1);
                }
                options.use_queue = true;
                break;
            case 'o': {
                string arg{optarg};
                if (arg != "M" && arg != "L") {
                    cerr << "Invalid output format" << '\n';
                    exit(1);
                }
                output_mode = arg[0];
                break;
            }
            case 'h':
                cout << "Usage: ship [--stack | -s] [--queue | -q] [--output | -o (M|L)] [--help | -h]" << '\n';
                exit(0);
            default:
                cerr << "No routing mode specified,test" << endl;
                exit(1);
        }
    }

    if (!options.use_queue && !options.use_stack) {
        cerr << "No routing mode specified" << '\n';
        exit(1);
    }
};

void findPath::readMaze(){
    ios::sync_with_stdio(false);
    cin >> input_mode >> tolLevel >> tolLen;
    string useless;
    getline(cin, useless);
    char input;
    char useless_L;

    //really needed?
    if(tolLevel > 10){
        cerr << "Invalid map level" << '\n';
        exit(1);
    }

    maze.resize(tolLevel, vector<vector<tile>>(tolLen, vector<tile>(tolLen)));
    if(input_mode == 'M'){
        /*for(uint32_t level = 0; level < tolLevel; level++){
            for(uint32_t row = 0; row < tolLen; row++){
                while(cin >> input){
                    if (input == '/'){
                        getline(cin, useless);
                        continue;
                    } else {
                        break;
                    }
                }
                for(uint32_t col = 0; col < tolLen - 1; col++){
                    
                    if (!(input == '.' || input == 'E' || input == 'S' || input == 'H' || input == '#')){
                        cerr << "Invalid map character" << '\n';
                        exit(1);
                    } else if (input != '.'){
                        maze[level][row][col].type = input;
                        if (input == 'S'){
                            start.col = col;
                            start.row = row;
                            start.level = level;
                            searchContainer.push_back(start);
                            maze[level][row][col].discovered = true;
                        }
                    }
                    cin >> input;
                }
                if (!(input == '.' || input == 'E' || input == 'S' || input == 'H' || input == '#')){
                        cerr << "Invalid map character" << '\n';
                        exit(1);
                } else if (input != '.'){
                    maze[level][row][tolLen - 1].type = input;
                    if (input == 'S'){
                        start.col = tolLen - 1;
                        start.row = row;
                        start.level = level;
                        searchContainer.push_back(start);//add discovered
                        maze[level][row][tolLen - 1].discovered = true;
                    }
                }
            }
        }  */
        for(uint32_t level = 0; level < tolLevel; level++){
            for(uint32_t row = 0; row < tolLen; row++){
                while(getline(cin, useless)){
                    if (useless[0] == '/'){
                        continue;
                    } else {
                        break;
                    }
                }
                for(uint32_t col = 0; col < tolLen; col++){
                    if (!(useless[col] == '.' || useless[col] == 'E' || useless[col] == 'S' || useless[col] == 'H' || useless[col] == '#')){
                        cerr << "Invalid map character" << '\n';
                        exit(1);
                    } else if (useless[col] != '.'){
                        maze[level][row][col].type = useless[col];
                        if (useless[col] == 'S'){
                            start.col = col;
                            start.row = row;
                            start.level = level;
                            searchContainer.push_back(start);
                            maze[level][row][col].discovered = true;
                        }
                    }
                }
            }
        } 
    } else if(input_mode == 'L'){
        uint32_t level;
        uint32_t row;
        uint32_t col;
        char tile;
        while (cin >> input){
            if (input == '/'){
                getline(cin, useless);
                continue;
            } else {
                cin >> level >> useless_L >> row >> useless_L >> col >> useless_L >> tile >> useless_L; 
                if (level >= tolLevel){
                    cerr << "Invalid map level" << '\n';
                    exit(1);
                }
                if (row >= tolLen){
                    cerr << "Invalid map row" << '\n';
                    exit(1);
                }
                if (col >= tolLen){
                    cerr << "Invalid map column" << '\n';
                    exit(1);
                }
                if (!(tile == '.' || tile == 'E' || tile == 'S' || tile == 'H' || tile == '#')){
                    cerr << "Invalid map character" << '\n';
                    exit(1);
                }
                if (tile != '.'){
                    maze[level][row][col].type = tile;
                    if (tile == 'S'){
                        start.col = col;
                        start.row = row;
                        start.level = level;
                        searchContainer.push_back(start);
                        maze[level][row][col].discovered = true;
                    }
                }
            }
        }
    }
};

void findPath::getSolution(Options &options){
    if (options.use_queue){
        while(!searchContainer.empty()){
            container current = searchContainer.front();
            searchContainer.pop_front();
            if (current.row >= 1 && !maze[current.level][current.row - 1][current.col].discovered && maze[current.level][current.row - 1][current.col].type != '#'){
                uint32_t nRow = current.row - 1;
                maze[current.level][nRow][current.col].discovered = true;
                container next(nRow, current.col, current.level);
                searchContainer.push_back(next);
                maze[current.level][nRow][current.col].direction = 'n';
                if (maze[current.level][nRow][current.col].type == 'H'){
                    end.row = nRow;
                    end.col = current.col;
                    end.level = current.level;
                    success = true;
                    return;
                }
            }
            //next search direction e
            if (current.col < tolLen - 1 && !maze[current.level][current.row][current.col + 1].discovered && maze[current.level][current.row][current.col + 1].type != '#'){
                uint32_t nCol = current.col + 1;
                maze[current.level][current.row][nCol].discovered = true;
                container next(current.row, nCol, current.level);
                searchContainer.push_back(next);
                maze[current.level][current.row][nCol].direction = 'e';
                if (maze[current.level][current.row][nCol].type == 'H'){
                    end.row = current.row;
                    end.col = nCol;
                    end.level = current.level;
                    success = true;
                    return;
                }
            }
            //next search direction s
            if (current.row < tolLen - 1 && !maze[current.level][current.row + 1][current.col].discovered && maze[current.level][current.row + 1][current.col].type != '#'){
                uint32_t nRow = current.row + 1;
                maze[current.level][nRow][current.col].discovered = true;
                container next(nRow, current.col, current.level);
                searchContainer.push_back(next);
                maze[current.level][nRow][current.col].direction = 's';
                if (maze[current.level][nRow][current.col].type == 'H'){
                    end.row = nRow;
                    end.col = current.col;
                    end.level = current.level;
                    success = true;
                    return;
                }
            }
            //next search direction w
            if (current.col >= 1 && !maze[current.level][current.row][current.col - 1].discovered && maze[current.level][current.row][current.col - 1].type != '#'){
                uint32_t nCol = current.col - 1;
                maze[current.level][current.row][nCol].discovered = true;
                container next(current.row, nCol, current.level);
                searchContainer.push_back(next);
                maze[current.level][current.row][nCol].direction = 'w';
                if (maze[current.level][current.row][nCol].type == 'H'){
                    end.row = current.row;
                    end.col = nCol;
                    end.level = current.level;
                    success = true;
                    return;
                }
            }
            //next search different level
            if (maze[current.level][current.row][current.col].type == 'E'){
                for (uint32_t l = 0; l < tolLevel; l++){
                    if (l != current.level && maze[l][current.row][current.col].type == 'E' && !maze[l][current.row][current.col].discovered){
                        maze[l][current.row][current.col].discovered = true;
                        container next(current.row, current.col, l);
                        searchContainer.push_back(next);
                        maze[l][current.row][current.col].direction = static_cast<char>(current.level);
                        
                    }
                }
            }
            
        }
        
    } else if (options.use_stack){
        while(!searchContainer.empty()){
            container current = searchContainer.back();
            searchContainer.pop_back();
            if (current.row >= 1 && !maze[current.level][current.row - 1][current.col].discovered && maze[current.level][current.row - 1][current.col].type != '#'){
                uint32_t nRow = current.row - 1;
                maze[current.level][nRow][current.col].discovered = true;
                container next(nRow, current.col, current.level);
                searchContainer.push_back(next);
                maze[current.level][nRow][current.col].direction = 'n';
                if (maze[current.level][nRow][current.col].type == 'H'){
                    end.row = nRow;
                    end.col = current.col;
                    end.level = current.level;
                    success = true;
                    return;
                }
            }
            //next search direction e
            if (current.col < tolLen - 1 && !maze[current.level][current.row][current.col + 1].discovered && maze[current.level][current.row][current.col + 1].type != '#'){
                uint32_t nCol = current.col + 1;
                maze[current.level][current.row][nCol].discovered = true;
                container next(current.row, nCol, current.level);
                searchContainer.push_back(next);
                maze[current.level][current.row][nCol].direction = 'e';
                if (maze[current.level][current.row][nCol].type == 'H'){
                    end.row = current.row;
                    end.col = nCol;
                    end.level = current.level;
                    success = true;
                    return;
                }
            }
            //next search direction s
            if (current.row < tolLen - 1 && !maze[current.level][current.row + 1][current.col].discovered && maze[current.level][current.row + 1][current.col].type != '#'){
                uint32_t nRow = current.row + 1;
                maze[current.level][nRow][current.col].discovered = true;
                container next(nRow, current.col, current.level);
                searchContainer.push_back(next);
                maze[current.level][nRow][current.col].direction = 's';
                if (maze[current.level][nRow][current.col].type == 'H'){
                    end.row = nRow;
                    end.col = current.col;
                    end.level = current.level;
                    success = true;
                    return;
                }
            }
            //next search direction w
            if (current.col >= 1 && !maze[current.level][current.row][current.col - 1].discovered && maze[current.level][current.row][current.col - 1].type != '#'){
                uint32_t nCol = current.col - 1;
                maze[current.level][current.row][nCol].discovered = true;
                container next(current.row, nCol, current.level);
                searchContainer.push_back(next);
                maze[current.level][current.row][nCol].direction = 'w';
                if (maze[current.level][current.row][nCol].type == 'H'){
                    end.row = current.row;
                    end.col = nCol;
                    end.level = current.level;
                    success = true;
                    return;
                }
            }
            //next search different level
            if (maze[current.level][current.row][current.col].type == 'E'){
                for (uint32_t l = 0; l < tolLevel; l++){
                    if (l != current.level && maze[l][current.row][current.col].type == 'E' && !maze[l][current.row][current.col].discovered){
                        maze[l][current.row][current.col].discovered = true;
                        container next(current.row, current.col, l);
                        searchContainer.push_back(next);
                        maze[l][current.row][current.col].direction = static_cast<char>(current.level);
                        
                    }
                }
            }
            
        }
        
    }
};

void findPath::outputPath(){
if (success){
    if (output_mode == 'M'){
        cout << "Start in level " << start.level << ", row " << start.row << ", column " << start.col << "\n";
        while (!end.equTo(start)){
            if (maze[end.level][end.row][end.col].direction == 'n'){
                end.row++;
                maze[end.level][end.row][end.col].type = 'n';
            } else if (maze[end.level][end.row][end.col].direction == 'e'){
                end.col--;
                maze[end.level][end.row][end.col].type = 'e';
            } else if (maze[end.level][end.row][end.col].direction == 's'){
                end.row--;
                maze[end.level][end.row][end.col].type = 's';
            } else if (maze[end.level][end.row][end.col].direction == 'w'){
                end.col++;
                maze[end.level][end.row][end.col].type = 'w';
            } else {
                uint32_t temp = end.level;
                end.level = static_cast<uint32_t>(maze[end.level][end.row][end.col].direction);
                maze[end.level][end.row][end.col].type = static_cast<char>(temp + '0');
            }
        }
        for (uint32_t l = 0; l < tolLevel; l++){
            std::cout << "//level " << l << std::endl;
            for (uint32_t r = 0; r < tolLen; r++){
                for (uint32_t c = 0; c < tolLen; c++){
                    cout << maze[l][r][c].type;
                }
            std::cout << "\n";
            }
        }
    } else if (output_mode == 'L'){
        deque<container> path;
        cout << "//path taken" << '\n';
        while (!end.equTo(start)){
            if (maze[end.level][end.row][end.col].direction == 'n'){
                end.row++;
                maze[end.level][end.row][end.col].type = 'n';
            } else if (maze[end.level][end.row][end.col].direction == 'e'){
                end.col--;
                maze[end.level][end.row][end.col].type = 'e';
            } else if (maze[end.level][end.row][end.col].direction == 's'){
                end.row--;
                maze[end.level][end.row][end.col].type = 's';
            } else if (maze[end.level][end.row][end.col].direction == 'w'){
                end.col++;
                maze[end.level][end.row][end.col].type = 'w';
            } else {
                uint32_t temp = end.level;
                end.level = static_cast<uint32_t>(maze[end.level][end.row][end.col].direction);
                maze[end.level][end.row][end.col].type = static_cast<char>(temp + '0');
            }
            path.push_front(end);
        }
        while (!path.empty()){
            cout << "(" << path.front().level << "," << path.front().row << "," << path.front().col << "," << maze[path.front().level][path.front().row][path.front().col].type << ")" << '\n';
            path.pop_front();
        }
    }
} else {
    if (output_mode == 'M'){
        cout << "Start in level " << start.level << ", row " << start.row << ", column " << start.col << "\n";
        for (uint32_t l = 0; l < tolLevel; l++){
            std::cout << "//level " << l << std::endl;
            for (uint32_t r = 0; r < tolLen; r++){
                for (uint32_t c = 0; c < tolLen; c++){
                    cout << maze[l][r][c].type;
                }
            std::cout << "\n";
            }
        }
    } else if (output_mode == 'L'){ 
        cout << "//path taken" << '\n';
    }
}
};    
