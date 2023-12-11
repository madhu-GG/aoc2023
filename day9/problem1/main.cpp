#include <fstream>
#include <iostream>
#include <vector>

#include <split>

void usage(std::string app_name) {
    std::cout << "Usage: " << app_name << " <input_file>" << std::endl;
}

long predict(const std::vector<long>& nums) {
	long next_value;
	std::vector<long> diffs;

	bool all_zeros = true;
	for (int j = 0; j < nums.size() - 1; j++) {
		auto diff = nums[j + 1] - nums[j];
		diffs.push_back(diff);
		if (diff != 0) all_zeros = false;
	}

	if (all_zeros) {
		next_value = nums[nums.size() - 1];
	} else {
		next_value = nums[nums.size() - 1] + predict(diffs);
	}

	return next_value;
}

long problem(std::string line) {
	std::vector<long> nums;
	auto nums_str = split(line, " ");
	for (auto n : nums_str) nums.push_back(stol(n));

    return predict(nums);
}

int main(int argc, char **argv) {
    if (argc != 2) {
        usage(argv[0]);
        return 0;
    }

    std::string filename = argv[1];
    std::ifstream f(filename);

    std::string line;
    long sum = 0;
    while(std::getline(f, line)) {
        sum += problem(line);
    }

    f.close();

    std::cout << "sum: " << sum << std::endl;
}
