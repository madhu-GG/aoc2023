#include <fstream>
#include <iostream>
#include <vector>
#include <split>
#include <trim>

void usage(std::string app_name) {
    std::cout << "Usage: " << app_name << " <input_file>" << std::endl;
}

long num_wins_possible(int total_time, int min_distance) {
	long count = 0;
	for (int charge_time = 1; charge_time < total_time; charge_time++) {
		int travel_time = total_time - charge_time;
		int travel_distance = travel_time * charge_time;
		if (travel_distance > min_distance) count++;
	}

	return count;
}

long problem(std::vector<int> times, std::vector<int> distances) {
	long product = 1;
	for (int race = 0; race < times.size(); race++) {
		product *= num_wins_possible(times[race], distances[race]);
	}

	return product;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        usage(argv[0]);
        return 0;
    }

    std::string filename = argv[1];
    std::ifstream f(filename);

    std::string line;
    int sum = 0;

	std::getline(f, line);
	auto times_str = split(line, "Time:")[1];
	times_str = trim(times_str);
	std::vector<int> times;
	for (auto val : split(times_str, " ")) {
		if (val == "") continue;
		times.push_back(stoi(val));
	}

	std::getline(f, line);
    f.close();
	auto distances_str = split(line, "Distance:")[1];
	distances_str = trim(distances_str);
	std::vector<int> distances;
	for (auto val : split(distances_str, " ")) {
		if (val == "") continue;
		distances.push_back(stoi(val));
	}

	long margin = problem(times, distances);

    std::cout << "margin: " << margin << std::endl;
}
