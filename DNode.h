#pragma once
#include <iostream>
#include <string>
#include "Decision.h"
using std::ostream, std::endl, std::string;
#include <vector>
#include <unordered_map>
#include <utility>

class DNode
{
public:
    DNode *right;
    DNode *left;
    DNode *parent;
    int depth;
    Decision data;
    int attributeIndex;

    DNode() {}
    DNode(Decision d, int dpth, DNode *p) : data(d), depth(dpth), parent(p), right(nullptr), left(nullptr) {}
};