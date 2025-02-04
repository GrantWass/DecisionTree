#include "DHelper.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm> // for sort
#include <utility> // for pair
using std::string, std::vector, std::unordered_map;
using std::sort, std::pair;

// custom delimiter definition to support splitting on CSV's comma
struct comma_delimiter : std::ctype<char> {
	comma_delimiter() : std::ctype<char>(get_table()) {}
	static mask const* get_table()
	{
		static mask m[table_size];
		m[','] = std::ctype_base::space;
		return m;
	}
};

// main data reader definition
void DHelper::readCSV(string file, vector<string>& attributes, unordered_map< string,vector<double> >& data, vector<int>& outcomes) {
	// check that training data can be accessed
	std::ifstream trainin(file);
	if (!trainin.is_open()) {
		std::cerr << "Unable to open training file!" << std::endl;
		return;
	}

	// setup file reading
	string stringline;
	std::stringstream streamline;

	// get labels from first line
	getline(trainin, stringline);
	streamline << stringline;

	string label;
	while (!streamline.eof()) {
		getline(streamline, label, ',');
		attributes.push_back(label);
	}
	streamline.clear();

	// assume the last item is the outcomes
	attributes.pop_back();

	// setup hash table of attribute names to hold the parallel vectors of values
	for (int i = 0; i < attributes.size(); i++) {
		data.emplace(attributes[i], vector<double>() );
	}

	streamline.imbue(std::locale(trainin.getloc(), new comma_delimiter));
	double value;
	int instanceCounter = 0;
	while (!trainin.eof()) {
		getline(trainin, stringline);
		streamline << stringline;
		for (int i = 0; i < attributes.size(); i++) {
			streamline >> value;
			data[attributes[i]].push_back(value);
		}
		// read the outcome last
		int o;
		streamline >> o;
		outcomes.push_back(o);
		streamline.clear();
	}
}

// return split point of sorted vec at thresh
int DHelper::getSplitPoint(double thresh, vector<double>& vec) {
	int i = 0;
	while ((i < vec.size()) && (thresh > vec[i])) {
		i++;
	}
	return i;
}

// return two dictionaries of attributes with their parallel vectors split at integer index
pair<unordered_map< string,vector<double> >, unordered_map< string,vector<double> >> DHelper::splitData(int index, unordered_map< string,vector<double> >& data) {
	unordered_map< string,vector<double> > data1;
	unordered_map< string,vector<double> > data2;

	for (auto& p : data) {
		const string& key = p.first; // attribute
		const vector<double>& vec = p.second; // attibute values

		vector<double> vec1(vec.begin(), vec.begin() + index);
		vector<double> vec2(vec.begin() + index, vec.end());

		data1[key] = vec1;
		data2[key] = vec2;
	}

	return {data1, data2};
}

// return list of indices to put vec in ascending order
vector<int> DHelper::getSortOrder(vector<double>& vec) {
	vector<int> indices(vec.size());
	for (int i = 0; i < indices.size(); ++i) {
		indices[i] = i;  // init as indices
	}

	// sort index list based on attribute values with anonymous function
	sort(indices.begin(), indices.end(), [&vec](int i1, int i2) {
		return vec[i1] < vec[i2];
	});

	return indices;
}