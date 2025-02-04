#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <utility>
using std::string;
using std::vector;
using std::unordered_map;
using std::pair;

namespace DHelper {
	/*
	readCSV takes in a string filename and vector placeholders that are populated as follows:
	all_attributes contains the string list of feature names
	all_data contains the feature values in parallel vectors matching the feature names
	outcomes contains the label matching each row in all_data feature values, another parallel vector
	*/
	void readCSV(string, vector<string>&, unordered_map< string,vector<double> >&, vector<int>&);

	/*
	getSplitPoint accepts a threshold value and sorted vector;
	it returns the index at which to split the data based on the threshold
	*/
	int getSplitPoint(double, vector<double>&);

	/*
	splitData is designed specifically for the dictionary data structure which maps attributes to parallel value arrays;
	it returns the two maps with parallel arrays split at the specified integer index.
	Note: this function is given to save effort splitting data, 
	but you'll still need to write logic to split outcomes
	*/
	pair<unordered_map< string,vector<double> >, unordered_map< string,vector<double> >> splitData(int, unordered_map< string,vector<double> >&);

	/*
	getSortOrder takes in a vector and returns a list of indices that will sort it in ascending order
	*/
	vector<int> getSortOrder(vector<double>&);

	/*
	sortVector takes a list of indices and a data vector of any type;
	it returns the data vector sorted according to the indices ordering.
	Note: this function is templated to support any type
	and for that reason must be defined here or have explicit type declarations somewhere
	*/
	template <typename T>
	vector<T> sortVector(vector<int> indices, vector<T>& data) {
		vector<T> sortedVector(data.size());
		for (int i = 0; i < indices.size(); ++i) {
			sortedVector[i] = data[indices[i]];  // reorder based on the sorted indices
		}
		return sortedVector;
	}
}