#pragma once

#include <vector>
using std::vector;

template<typename T>
class MinHeap {
private:
	vector<T> pq;

public:
	MinHeap() {}
	MinHeap(vector<T> v) {
		pq = v;
		heapify();
	}

	int getParent(int i) const {
		return (i - 1) / 2;
	}

	int getLeft(int i) const {
		return (2*i + 1);
	}

	int getRight(int i) const {
		return (2*i + 2);
	}

	bool empty() const {
		return pq.size() == 0;
	}

	void swap(int i, int j) {
		T temp = pq.at(i);
		pq.at(i) = pq.at(j);
		pq.at(j) = temp;
	}

	void upheap(int i) {
		if (i <= 0) return;

		int p = getParent(i);
		if (pq.at(i) < pq.at(p)) {
			swap(i,p);
			upheap(p);
		}
	}

	void downheap(int i) {
		if (getLeft(i) >= pq.size()) return;

		int l = getLeft(i);
		int r = getRight(i);

		int comp = l;

		if (r < pq.size() && pq.at(r) < pq.at(l)) {
			comp = r;
		}

		if (pq.at(i) > pq.at(comp)) {
			swap(i,comp);
			downheap(comp);
		}
	}

	void heapify() {
		for (int i = pq.size()/2 - 1; i >= 0; i--) {
			downheap(i);
		}
	}

	void insert(T elem) {
		pq.push_back(elem);
		upheap(pq.size()-1);
	}

	T removeMin() {
		T temp = pq.at(0);
		pq.at(0) = pq.at(pq.size() - 1);
		pq.pop_back();
		downheap(0);
		return temp;
	}

};

template <typename T>
vector<T> heapsort(vector<T> v) {
	MinHeap h(v);
	vector<T> sorted;
	while (!h.empty()) {
		sorted.push_back(h.removeMin());
	}
	return sorted;
}