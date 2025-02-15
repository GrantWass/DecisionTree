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
	// Call recursive helper on data and outcomes to build the tree
	root = trainSubtree(nullptr, data, outcomes, 0);
}

// From ChatGPT
int getAttributeIndex(const string &attribute, vector<string> attributes)
{
	auto it = std::find(attributes.begin(), attributes.end(), attribute);
	if (it != attributes.end())
	{
		return std::distance(attributes.begin(), it); // Get index of the attribute
	}
	return -1;
}

DNode *DTree::trainSubtree(DNode *parent, unordered_map<string, vector<double>> data, vector<int> outcomes, int depth)
{
	// Base case: If all outcomes are the same, return nullptr
	if (std::all_of(outcomes.begin(), outcomes.end(), [&](int val)
					{ return val == outcomes[0]; }))
	{
		return nullptr;
	}

	// Base case: If there are no attributes left, return nullptr
	if (data.empty())
	{
		return nullptr;
	}

	// Get the best feature/attribute threshold based on impurity
	Decision d = getMinImpurity(data, outcomes);

	// Create a new node with the decision
	DNode *n = new DNode(d, depth, parent);

	n->attributeIndex = getAttributeIndex(d.attribute, attributes);

	// First, sort the data based on the attribute
	vector<int> sortedIndices = DHelper::getSortOrder(data[d.attribute]);

	// Iterate over all attributes in data and sort the corresponding vectors
	for (auto &[key, values] : data)
	{
		values = DHelper::sortVector(sortedIndices, values);
	}

	// Sort outcomes as well so data and outcomes are in sync
	outcomes = DHelper::sortVector(sortedIndices, outcomes);

	// Split the data based on the threshold
	int splitIndex = DHelper::getSplitPoint(d.threshold, data[d.attribute]);
	auto splitData = DHelper::splitData(splitIndex, data);

	// Create left and right data sets
	std::unordered_map<std::string, std::vector<double>> dataLeft, dataRight;
	std::vector<int> outcomesLeft, outcomesRight;
	dataLeft = splitData.first;
	dataRight = splitData.second;
	outcomesLeft = std::vector<int>(outcomes.begin(), outcomes.begin() + splitIndex);
	outcomesRight = std::vector<int>(outcomes.begin() + splitIndex, outcomes.end());

	// Recur for left and right subtrees
	n->left = trainSubtree(n, dataLeft, outcomesLeft, depth + 1);
	n->right = trainSubtree(n, dataRight, outcomesRight, depth + 1);

	return n;
}

Decision DTree::getMinImpurity(unordered_map<string, vector<double>> &data, vector<int> &outcomes)
{
	// for each attribute, get the impurity
	// pick feature with min impurity and return
	// reccomend using MinHeap<Decision> to store the decisions
	MinHeap<Decision> minHeap;
	for (auto &[attr, values] : data)
	{
		Decision d = getImpurity(attr, data, outcomes);
		minHeap.insert(d);
	}

	return minHeap.removeMin();
	;
}

Decision DTree::getImpurity(string attr, unordered_map<string, vector<double>> &data, vector<int> &outcomes)
{

	// get the impurity for a single attribute
	std::vector<double> attributeData = data[attr];
	// sort the attribute data
	std::vector<int> sortedIndices = DHelper::getSortOrder(attributeData);
	attributeData = DHelper::sortVector(sortedIndices, attributeData);

	// must also sort outcomes
	std::vector<int> sortedOutcomes = DHelper::sortVector(sortedIndices, outcomes);

	MinHeap<Decision> minHeap;
	int overallCount = attributeData.size();

	// get the unique values in the attribute
	std::vector<double> uniqueValues;
	for (int i = 0; i < attributeData.size(); i++)
	{
		if (i == 0 || attributeData[i] != attributeData[i - 1])
		{
			uniqueValues.push_back(attributeData[i]);
		}
	}
	// get a list of thresholds
	std::vector<double> thresholds;
	for (int i = 0; i < uniqueValues.size() - 1; i++)
	{
		thresholds.push_back((uniqueValues[i] + uniqueValues[i + 1]) / 2);
	}

	if (uniqueValues.size() == 1)
	{
		thresholds.push_back(uniqueValues[0]);
	}

	// for each threshold
	for (double threshold : thresholds)
	{
		double lowerImpurity = 0.0, upperImpurity = 0.0;
		int yesCountLower = 0, noCountLower = 0, yesCountUpper = 0, noCountUpper = 0;
		int overallCount = attributeData.size();

		// iterate over the attributes
		for (size_t i = 0; i < attributeData.size(); i++)
		{
			if (attributeData[i] < threshold)
			{
				sortedOutcomes[i] ? yesCountLower++ : noCountLower++;
			}
			else
			{
				sortedOutcomes[i] ? yesCountUpper++ : noCountUpper++;
			}
		}

		auto giniImpurity = [](int yes, int no)
		{
			if (yes + no == 0)
				return 0.0;
			double pYes = static_cast<double>(yes) / (yes + no);
			double pNo = 1.0 - pYes;
			return 1.0 - (pYes * pYes) - (pNo * pNo);
		};

		int lowerCount = yesCountLower + noCountLower, upperCount = yesCountUpper + noCountUpper;

		// Calculate weighted impurity (based on partition sizes)
		double weightedImpurity = (static_cast<double>(lowerCount) / overallCount) * giniImpurity(yesCountLower, noCountLower) +
								  (static_cast<double>(upperCount) / overallCount) * giniImpurity(yesCountUpper, noCountUpper);

		// set belowMajority and aboveMajority above based on counts
		int majorityAbove = yesCountUpper > noCountUpper ? 1 : 0;
		int majorityBelow = yesCountLower > noCountLower ? 1 : 0;
		Decision d(attr, threshold, weightedImpurity, majorityAbove, majorityBelow);

		minHeap.insert(d);
	}
	// select the minimum threshold's impurity
	Decision lowestImpurity = minHeap.removeMin();

	// return decision that is best threshold and impurity
	return lowestImpurity;
}

int DTree::classify(vector<double> &data)
{
	DNode *currentNode = root;
	while (currentNode != nullptr)
	{
		double attributeValue = data[currentNode->attributeIndex];

		// If the node is a leaf, return the majority class
		if (!currentNode->left && !currentNode->right)
			return attributeValue < currentNode->data.threshold ? currentNode->data.majorityBelow : currentNode->data.majorityAbove;

		if (attributeValue < currentNode->data.threshold)
		{
			if (currentNode->left)
				currentNode = currentNode->left;
			else
				return currentNode->data.majorityBelow;
		}
		else
		{
			if (currentNode->right)
				currentNode = currentNode->right;
			else
				return currentNode->data.majorityAbove;
		}
	}

	return -1; // Shouldn't reach here if the tree is properly formed
}

void DTree::clear(DNode *n)
{
	if (n != nullptr)
	{
		clear(n->left);
		clear(n->right);
		delete n;
	}
}
