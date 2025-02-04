#pragma once

#include <iostream>
#include <string>
using std::ostream, std::endl, std::string;

class Decision {
public:
	string attribute;
	double threshold;
	double impurity;

	int majorityAbove;
	int majorityBelow;

	Decision() {}
	Decision(string a, double t, double i, int ma, int mb) : attribute(a), threshold(t), impurity(i), majorityAbove(ma), majorityBelow(mb) {}
	// decisions are ranked based on information gain
	bool operator<(const Decision& d) const {
		return (impurity < d.impurity);
	}
	// define other comparisons using the existing one for convenience
	bool operator>(const Decision& rhs) { return rhs < *this; }
	bool operator>=(const Decision& rhs) { return !(*this < rhs); }
	bool operator<=(const Decision& rhs) { return !(*this > rhs); }

	// output operator for printing
	friend ostream& operator<<(ostream&, const Decision&);
};
inline ostream& operator<<(ostream& os, const Decision& d) {
	os << d.attribute << "," << d.threshold << std::endl;
	return os;
}