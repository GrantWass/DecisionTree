#include <iostream>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <cmath>

// MinHeap definition
template <typename T>
class MinHeap
{
public:
    void insert(const T &item)
    {
        heap.push_back(item);
        heapifyUp(heap.size() - 1);
    }

    T removeMin()
    {
        if (heap.empty())
        {
            throw std::out_of_range("Heap is empty");
        }
        T minItem = heap[0];
        heap[0] = heap.back();
        heap.pop_back();
        heapifyDown(0);
        return minItem;
    }

private:
    std::vector<T> heap;

    void heapifyUp(int index)
    {
        while (index > 0)
        {
            int parentIndex = (index - 1) / 2;
            if (heap[index] < heap[parentIndex])
            {
                std::swap(heap[index], heap[parentIndex]);
                index = parentIndex;
            }
            else
            {
                break;
            }
        }
    }

    void heapifyDown(int index)
    {
        int leftChild = 2 * index + 1;
        int rightChild = 2 * index + 2;
        int smallest = index;

        if (leftChild < heap.size() && heap[leftChild] < heap[smallest])
        {
            smallest = leftChild;
        }
        if (rightChild < heap.size() && heap[rightChild] < heap[smallest])
        {
            smallest = rightChild;
        }
        if (smallest != index)
        {
            std::swap(heap[index], heap[smallest]);
            heapifyDown(smallest);
        }
    }
};

// Decision class to hold impurity information
class Decision
{
public:
    std::string attribute;
    double threshold;
    double impurity;
    int majorityAbove;
    int majorityBelow;

    Decision(std::string attr, double thresh, double imp, int majAbove, int majBelow)
        : attribute(attr), threshold(thresh), impurity(imp), majorityAbove(majAbove), majorityBelow(majBelow) {}

    bool operator<(const Decision &other) const
    {
        return impurity < other.impurity;
    }
};

// DNode class for decision tree node
class DNode
{
public:
    Decision decision;
    int depth;
    DNode *parent;
    DNode *left;
    DNode *right;

    DNode(Decision d, int dpt, DNode *par)
        : decision(d), depth(dpt), parent(par), left(nullptr), right(nullptr) {}
};

// Helper function to get the sort order of a vector
namespace DHelper
{
    int getSplitPoint(double thresh, std::vector<double> &vec)
    {
        int i = 0;
        while ((i < vec.size()) && (thresh > vec[i]))
        {
            i++;
        }
        return i;
    }

    std::vector<int> getSortOrder(std::vector<double> &vec)
    {
        std::vector<int> indices(vec.size());
        for (int i = 0; i < indices.size(); ++i)
        {
            indices[i] = i; // init as indices
        }

        // sort index list based on attribute values with anonymous function
        std::sort(indices.begin(), indices.end(), [&vec](int i1, int i2) -> bool
                  { return vec[i1] < vec[i2]; });

        return indices;
    }

    template <typename T>
    std::vector<T> sortVector(std::vector<int> indices, std::vector<T> &data)
    {
        std::vector<T> sortedVector(data.size());
        for (int i = 0; i < indices.size(); ++i)
        {
            sortedVector[i] = data[indices[i]]; // reorder based on the sorted indices
        }
        return sortedVector;
    }

    std::pair<std::unordered_map<std::string, std::vector<double>>, std::unordered_map<std::string, std::vector<double>>>
    splitData(int splitIndex, const std::unordered_map<std::string, std::vector<double>> &data)
    {
        std::unordered_map<std::string, std::vector<double>> left, right;

        for (const auto &entry : data)
        {
            std::vector<double> leftValues(entry.second.begin(), entry.second.begin() + splitIndex);
            std::vector<double> rightValues(entry.second.begin() + splitIndex, entry.second.end());

            left[entry.first] = leftValues;
            right[entry.first] = rightValues;
        }

        return {left, right};
    }
};

// Decision Tree class to calculate impurity
class DTree
{
public:
    // Method to calculate impurity for a given attribute and dataset
    Decision getImpurity(const std::string &attr, std::unordered_map<std::string, std::vector<double>> &data, std::vector<int> &outcomes)
    {
        std::cout << "Calculating impurity for attribute: " << attr << std::endl;

        std::vector<double> attributeData = data[attr];
        std::vector<int> sortedIndices = DHelper::getSortOrder(attributeData);
        attributeData = DHelper::sortVector(sortedIndices, attributeData);
        outcomes = DHelper::sortVector(sortedIndices, outcomes);

        MinHeap<Decision> minHeap;
        int overallCount = attributeData.size();

        std::vector<double> uniqueValues;
        for (int i = 0; i < attributeData.size(); i++)
        {
            if (i == 0 || attributeData[i] != attributeData[i - 1])
            {
                uniqueValues.push_back(attributeData[i]);
            }
        }

        std::vector<double> thresholds;
        for (int i = 0; i < uniqueValues.size() - 1; i++)
        {
            thresholds.push_back((uniqueValues[i] + uniqueValues[i + 1]) / 2);
        }

        std::cout << "Unique values and thresholds: ";
        for (double val : uniqueValues)
            std::cout << val << " ";
        std::cout << std::endl;

        for (double threshold : thresholds)
        {
            std::cout << "Threshold: " << threshold << std::endl;

            double lowerImpurity = 0.0;
            double upperImpurity = 0.0;
            int yesCountLower = 0, noCountLower = 0;
            int yesCountUpper = 0, noCountUpper = 0;

            for (size_t i = 0; i < attributeData.size(); i++)
            {
                if (attributeData[i] < threshold)
                {
                    if (outcomes[i] == 1)
                        yesCountLower++;
                    else
                        noCountLower++;
                }
                else
                {
                    if (outcomes[i] == 1)
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

            double weightedImpurity = (static_cast<double>(lowerCount) / overallCount) * lowerImpurity +
                                      (static_cast<double>(upperCount) / overallCount) * upperImpurity;

            int majorityAbove = yesCountUpper > noCountUpper ? 1 : 0;
            int majorityBelow = yesCountLower > noCountLower ? 1 : 0;

            Decision d(attr, threshold, weightedImpurity, majorityAbove, majorityBelow);

            std::cout << "Impurity for threshold " << threshold << ": " << weightedImpurity << std::endl;
            minHeap.insert(d);
        }

        Decision lowestImpurity = minHeap.removeMin();
        if (thresholds.size() <= 2)
        {
            return lowestImpurity;
        }
        Decision secondLowestImpurity = minHeap.removeMin();

        // Unit tests want impurity from lowest threshold if impurities are even
        if (lowestImpurity.impurity == secondLowestImpurity.impurity && lowestImpurity.threshold > secondLowestImpurity.threshold)
        {
            return secondLowestImpurity;
        }
        std::cout << "Best decision based on impurity: " << lowestImpurity.impurity << " for threshold " << lowestImpurity.threshold << std::endl;

        return lowestImpurity;
    }

    // Method to recursively build the decision tree (train subtree)
    DNode *trainSubtree(DNode *parent, std::unordered_map<std::string, std::vector<double>> &data, std::vector<int> &outcomes, int depth)
    {
        std::cout << "Training subtree at depth " << depth << std::endl;

        // Base case: If all outcomes are the same, return nullptr
        if (std::all_of(outcomes.begin(), outcomes.end(), [&](int val)
                        { return val == outcomes[0]; }))
        {
            std::cout << "All outcomes are the same, returning nullptr" << std::endl;
            return nullptr;
        }

        // Base case: If there are no attributes left, return nullptr
        if (data.empty())
        {
            std::cout << "No attributes left, returning nullptr" << std::endl;
            return nullptr;
        }

        // Get the best feature/attribute threshold based on impurity
        Decision d = getMinImpurity(data, outcomes);

        std::cout << "Creating node with decision: " << d.attribute << " threshold: " << d.threshold << std::endl;

        // Create a new node with the decision
        DNode *n = new DNode(d, depth, parent);
        std::unordered_map<std::string, std::vector<double>> dataLeft, dataRight;
        std::vector<int> outcomesLeft, outcomesRight;

        // First, sort the data based on the attribute
        std::vector<int> sortedIndices = DHelper::getSortOrder(data[d.attribute]);

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

        // Recursively build left and right subtrees
        n->right = trainSubtree(n, dataLeft, outcomesLeft, depth + 1);
        n->left = trainSubtree(n, dataRight, outcomesRight, depth + 1);

        return n;
    }

    // Method to get the best impurity decision across all attributes
    Decision getMinImpurity(std::unordered_map<std::string, std::vector<double>> &data, std::vector<int> &outcomes)
    {
        std::cout << "Getting minimum impurity" << std::endl;

        MinHeap<Decision> minHeap;
        for (auto &[attr, values] : data)
        {
            Decision d = getImpurity(attr, data, outcomes);
            minHeap.insert(d);
        }
        return minHeap.removeMin();
    }
};

int main()
{
    // Input test data
    std::unordered_map<std::string, std::vector<double>> data;
    data["f1"] = {1, 2, 3, 4, 5, 1, 7};
    data["f2"] = {0.2, 0.1, 0.3, 0.5, 0.0, 0.6, 0.1};

    std::vector<int> outcomes = {0, 1, 0, 0, 1, 1, 0};

    // Create DTree object and run the training
    DTree tree;
    tree.trainSubtree(nullptr, data, outcomes, 0);

    return 0;
}
