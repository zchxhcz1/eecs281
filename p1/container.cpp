// Project Identifier: 950181F63D0A883F183EC0A5CC67B19928FE896A
#include "container.h"

container::container(){
    row = 0;
    col = 0;
    level = 0;
};

container::container(uint32_t &r,uint32_t &c,uint32_t &l){
    row = r;
    col = c;
    level = l;
};

bool container::equTo(const container &pos){
    if(this->col == pos.col && this->row == pos.row && this->level == pos.level){
        return true;
    }
    return false;
};