#pragma once
#include <iostream>
#include <string>
#include <Decision.h>
using std::ostream, std::endl, std::string;
using std::vector, std::unordered_map, std::pair;

class DNode
{
public:
    DNode *right;
    DNode *left;
    DNode *parent;
    int depth;
    Decision data;

    DNode() {}
    DNode(Decision d, int dpth, DNode *p) : data(d), depth(dpth), parent(p), right(nullptr), left(nullptr) {}
};