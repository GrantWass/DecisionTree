#include "DTree.h"
#include "DHelper.h"
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
using std::string;
using std::vector;
using std::unordered_map;

vector<double> getInstance(vector<string>& a, unordered_map< string,vector<double> >& d, int i) {
	vector<double> inst;
	for (int j = 0; j < a.size(); j++) {
		inst.push_back(d[a[j]][i]); // get the ith element in the dictionary for attribute j
	}
	return inst;
}

int main(int argc, char* argv[]) {

	if (argc < 3) { // must have at least 3 arguments for ./sort -a <algorithm>
		std::cerr << "Usage: " << argv[0] << " <training data> <testing data>" << std::endl;
		return 1;
	}

	// get filenames
	string trainfile = std::string(argv[1]);
	string testfile = std::string(argv[2]);


	// read training data
	vector<string> attributes;
	unordered_map< string,vector<double> > data;
	vector<int> outcomes;
	DHelper::readCSV(trainfile, attributes, data, outcomes);

	// train the decision tree
	DTree dt(attributes);
	dt.train(data,outcomes);


	// read testing data
	attributes.clear();
	data.clear();
	outcomes.clear();
	DHelper::readCSV(testfile, attributes, data, outcomes);

	// test all instances by classifying them through the decision tree
	double totalCounted = 0;
	double totalCorrect = 0;
	for (int i = 0; i < outcomes.size(); i++) {
		vector<double> inst = getInstance(attributes,data,i);
		int outc = dt.classify(inst);
		if (outc == outcomes.at(i)) {
			totalCorrect++;
		}
		totalCounted++;
	}

	// print tree structure
	std::cout << dt.levelOrderTraversal() << std::endl;
	
	// output accuracy = correct / total
	std::cout << "Accuracy: " << totalCorrect / totalCounted << std::endl;

	return 0;
}