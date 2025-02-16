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

struct DecisionComparator
{
	bool operator()(const Decision &a, const Decision &b) const
	{
		if (a.impurity == b.impurity)
		{
			return a.threshold < b.threshold;
		}
		return a.impurity < b.impurity;
	}
};

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

	std::unordered_map<std::string, std::vector<double>> dataLeft, dataRight;
	std::vector<int> outcomesLeft, outcomesRight;

	// First, sort the data based on the attribute
	vector<int> sortedIndices = DHelper::getSortOrder(data[d.attribute]);

	// Iterate over all attributes in data and sort the corresponding vectors
	for (auto &[key, values] : data)
	{
		values = DHelper::sortVector(sortedIndices, values);
	}

	// Sort outcomes as well
	outcomes = DHelper::sortVector(sortedIndices, outcomes);

	// Split the data based on the threshold
	int splitIndex = DHelper::getSplitPoint(d.threshold, data[d.attribute]);
	auto splitData = DHelper::splitData(splitIndex, data);

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

	Decision lowestImpurity = minHeap.removeMin();
	// std::cout << "Best decision based on impurity (whole): " << lowestImpurity.impurity << " for threshold " << lowestImpurity.threshold << std::endl;

	return lowestImpurity;
}

Decision DTree::getImpurity(string attr, unordered_map<string, vector<double>> &data, vector<int> &outcomes)
{

	// std::cout << "Calculating impurity for attribute: " << attr << ", " << data[attr].size() << std::endl;

	// get the impurity for a single attribute
	std::vector<double> attributeData = data[attr];
	// sort the attribute data
	std::vector<int> sortedIndices = DHelper::getSortOrder(attributeData);
	attributeData = DHelper::sortVector(sortedIndices, attributeData);

	// must also sort outcomes
	std::vector<int> sortedOutcomes;
	sortedOutcomes = DHelper::sortVector(sortedIndices, outcomes);

	// std::cout << "Attribute Data: ";
	// for (double val : attributeData)
	// {
	// 	std::cout << val << " ";
	// }
	// std::cout << std::endl;

	// // Print outcomes
	// std::cout << "Outcomes: ";
	// for (int outcome : outcomes)
	// {
	// 	std::cout << outcome << " ";
	// }
	// std::cout << std::endl;

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

	// for each threshold
	for (double threshold : thresholds)
	{
		double lowerImpurity = 0.0;
		double upperImpurity = 0.0;
		int yesCountLower = 0, noCountLower = 0;
		int yesCountUpper = 0, noCountUpper = 0;

		int overallCount = attributeData.size();

		// iterate over the attributes
		for (size_t i = 0; i < attributeData.size(); i++)
		{
			if (attributeData[i] < threshold)
			{
				if (sortedOutcomes[i] == 1)
					yesCountLower++;
				else
					noCountLower++;
			}
			else
			{
				if (sortedOutcomes[i] == 1)
					yesCountUpper++;
				else
					noCountUpper++;
			}
		}

		int lowerCount = yesCountLower + noCountLower;
		int upperCount = yesCountUpper + noCountUpper;

		if (lowerCount > 0)
		{
			double pYesLower = static_cast<double>(yesCountLower) / lowerCount;
			double pNoLower = static_cast<double>(noCountLower) / lowerCount;
			lowerImpurity = 1.0 - (pYesLower * pYesLower) - (pNoLower * pNoLower);
		}

		if (upperCount > 0)
		{
			double pYesUpper = static_cast<double>(yesCountUpper) / upperCount;
			double pNoUpper = static_cast<double>(noCountUpper) / upperCount;
			upperImpurity = 1.0 - (pYesUpper * pYesUpper) - (pNoUpper * pNoUpper);
		}

		// Calculate weighted impurity (based on partition sizes)
		double weightedImpurity = (static_cast<double>(lowerCount) / overallCount) * lowerImpurity +
								  (static_cast<double>(upperCount) / overallCount) * upperImpurity;

		// set belowMajority and aboveMajority above based on counts
		int majorityAbove = yesCountUpper > noCountUpper ? 1 : 0;
		int majorityBelow = yesCountLower > noCountLower ? 1 : 0;
		Decision d(attr, threshold, weightedImpurity, majorityAbove, majorityBelow);

		// std::cout << "Calculated impurity and threshold: " << weightedImpurity << ", " << threshold << std::endl;

		minHeap.insert(d);
	}
	// select the minimum threshold's impurity
	Decision lowestImpurity = minHeap.removeMin();

	// return decision that is best threshold and impurity
	// std::cout << "Chosen impurity and threshold: " << lowestImpurity.impurity << ", " << lowestImpurity.threshold << std::endl;

	return lowestImpurity;
}

int DTree::classify(vector<double> &data)
{
	DNode *currentNode = root;
	while (currentNode != nullptr)
	{
		std::cout << "Current node threshold: " << currentNode->data.threshold << std::endl;
		std::cout << "Current node attribute" << currentNode->data.attribute << std::endl;
		double attributeValue = data[currentNode->attributeIndex];

		std::cout << "Attribute value: " << attributeValue << std::endl;

		// If the current node is a leaf node (no left or right child), return the prediction
		if (currentNode->left == nullptr && currentNode->right == nullptr)
		{

			// Return majority class based on threshold comparison
			if (attributeValue < currentNode->data.threshold)
			{
				std::cout << "lower classify " << std::endl;

				return currentNode->data.majorityBelow;
			}
			else
			{
				std::cout << "higher classify" << std::endl;

				return currentNode->data.majorityAbove;
			}
		}

		if (currentNode->right == nullptr)
		{
			if (attributeValue < currentNode->data.threshold)
			{
				currentNode = currentNode->left;
			}
			else
			{
				return currentNode->data.majorityAbove;
			}
		}
		else if (currentNode->left == nullptr)
		{
			if (attributeValue < currentNode->data.threshold)
			{
				return currentNode->data.majorityBelow;
			}
			else
			{
				currentNode = currentNode->right;
			}
		}
		else
		{
			// Otherwise, move to the left or right child based on threshold
			if (attributeValue < currentNode->data.threshold)
			{
				std::cout << "lower " << std::endl;
				currentNode = currentNode->left;
			}
			else
			{
				std::cout << "higher " << std::endl;
				currentNode = currentNode->right;
			}
		}
	}

	return -1;
}

// DONE
void DTree::clear(DNode *n)
{
	if (n != nullptr)
	{
		clear(n->left);
		clear(n->right);
		delete n;
	}
}
