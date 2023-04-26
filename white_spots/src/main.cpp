#include <iostream>
#include <vector>

bool mark_spot(std::vector<std::vector<int>>& image,
			   std::vector<std::vector<int>>& marked,
			   int i, int j, int n, int m) {
	if(i < 0 || j < 0 || i >= n || j >= m) {
		return false;
	}
	
	int node = image[i][j];
	int mark = marked[i][j];

	// Unmarked white node
	if(node == 1 && mark != 1) {
		marked[i][j] = 1;

		mark_spot(image, marked, i+1, j, n, m);
		mark_spot(image, marked, i, j+1, n, m);
		mark_spot(image, marked, i-1, j, n, m);
		mark_spot(image, marked, i, j-1, n, m);

		return true;
	}

	return false;
}

int spots_count(std::vector<std::vector<int>>& image, int n, int m) {
	std::vector<std::vector<int>> marked_nodes;
	marked_nodes.reserve(n);
	for(int i = 0; i < n; ++i) {
		marked_nodes[i].reserve(m);
		
		for(int j = 0; j < m; ++j) {
			marked_nodes[i][j] = 0;
		}
	}

	int result = 0;
	for(int i = 0; i < n; ++i) {
		for(int j = 0; j < m; ++j) {
			if(mark_spot(image, marked_nodes, i, j, n, m))
				++result;
		}
	}

	return result;
}

int main(int argc, char** argv) {
	int n = 5-1;
	int m = 5;

	std::vector<std::vector<int>> image;
	image.reserve(n);
	for(int i = 0; i < n; ++i) {
		image[i].reserve(m);

		std::cout << i+1 << " line: " << std::endl;
		
		for(int j = 0; j < m; ++j) {
			std::cin >> image[i][j];
		}
	}

	std::cout << "Image: " << std::endl;
	for(int i = 0; i < n; ++i) {
		for(int j = 0; j < m; ++j) {
			std::cout << image[i][j] << " ";
		}

		std::cout << std::endl;
	}

	std::cout << spots_count(image, n, m) << std::endl;
	
	return 0;
}
