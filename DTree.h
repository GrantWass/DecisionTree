#pragma once

#include "DNode.h"
#include <string>
#include <vector>
#include <unordered_map>
using std::string, std::vector, std::unordered_map;

class DTree
{
private:
	vector<string> attributes;
	DNode *root;

public:
	DTree(vector<string> attr);
	~DTree();
	string levelOrderTraversal();

	void train(unordered_map<string, vector<double>> &data, vector<int> &outcomes);
	DNode *trainSubtree(DNode *parent, unordered_map<string, vector<double>> data, vector<int> outcomes, int depth);

	Decision getMinImpurity(unordered_map<string, vector<double>> &data, vector<int> &outcomes);
	Decision getImpurity(string attr, unordered_map<string, vector<double>> &data, vector<int> &outcomes);

	int classify(vector<double> &data);

	void clear(DNode *n);
};