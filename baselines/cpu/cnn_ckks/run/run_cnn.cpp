#include <iostream>
#include "cnn_seal.h"
#include "infer_seal.h"
#include <algorithm>

using namespace std;

int main(int argc, char **argv) {
	int layer = atoi(argv[1]);
	int dataset = atoi(argv[2]);
	int start = atoi(argv[3]);
	int end = atoi(argv[4]);

	if (start < 0 || start >= 10000) throw std::invalid_argument("start number is not correct");
	if (end < 0 || end >= 10000) throw std::invalid_argument("end number is not correct");
	if (start > end) throw std::invalid_argument("start number is larger than end number");

	cout << "model: ResNet-" << layer << endl;
	cout << "dataset: CIFAR-" << dataset << endl;
	cout << "start image: " << start << endl;
	cout << "end image: " << end << endl;

	if (dataset == 10) ResNet_cifar10_seal_sparse(layer, start, end);
	else if (dataset == 100) ResNet_cifar100_seal_sparse(layer, start, end);
	else throw std::invalid_argument("dataset number is not correct");

	return 0;
}
