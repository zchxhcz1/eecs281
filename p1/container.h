// Project Identifier: 950181F63D0A883F183EC0A5CC67B19928FE896A
#ifndef _CONTAINER_H_
#define _CONTAINER_H_
#include<iostream>
#include<vector>
#include<deque>
class container{
public:    
    uint32_t row;
    uint32_t col;
    uint32_t level;
    container();
    container(uint32_t &r,uint32_t &c,uint32_t &l);
    bool equTo(const container &pos);
};
#endif