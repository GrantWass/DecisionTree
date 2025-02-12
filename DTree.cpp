#include "DTree.h"
#include "DNode.h"
#include "Decision.h"
#include "MinHeap.h"
#include "DHelper.h"
#include <queue>
#include <string>
#include <vector>
#include <unordered_map>
#include <sstream>
#include <utility>
#include <algorithm>
#include <cmath>

using std::queue, std::string, std::vector, std::unordered_map, std::stringstream;

DTree::DTree(vector<string> attr) : root(nullptr), attributes(attr) {}

DTree::~DTree()
{
	clear(root);
}

string DTree::levelOrderTraversal()
{
	stringstream ss;
	queue<DNode *> q;
	q.push(root);
	while (!q.empty())
	{
		DNode *u = q.front();
		q.pop();
		if (u != nullptr)
		{
			ss << string(u->depth, ' ') << u->data;
			q.push(u->left);
			q.push(u->right);
		}
	}
	return ss.str();
}

void DTree::train(unordered_map<string, vector<double>> &data, vector<int> &outcomes)
{
	// TODO
}

DNode *DTree::trainSubtree(DNode *parent, unordered_map<string, vector<double>> data, vector<int> outcomes, int depth)
{
	if (data.empty() || outcomes.empty())
	{
		printf("data or outcomes is empty");
		return nullptr;
	}
	Decision best = getMinImpurity(data, outcomes);
	return nullptr;
}

Decision DTree::getMinImpurity(unordered_map<string, vector<double>> &data, vector<int> &outcomes)
{
	// TODO
	return Decision();
}

Decision DTree::getImpurity(string attr, unordered_map<string, vector<double>> &data, vector<int> &outcomes)
{
	// TODO
	return Decision();
}

int DTree::classify(vector<double> &data)
{
	// TODO
	return 0;
}

void DTree::clear(DNode *n)
{
	// TODO
}
