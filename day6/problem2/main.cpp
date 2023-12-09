#include <fstream>
#include <iostream>
#include <vector>
#include <split>
#include <trim>

void usage(std::string app_name) {
    std::cout << "Usage: " << app_name << " <input_file>" << std::endl;
}

long num_wins_possible(long total_time, long min_distance) {
	long count = 0;
	for (long charge_time = 1; charge_time < total_time; charge_time++) {
		long travel_time = total_time - charge_time;
		long travel_distance = travel_time * charge_time;
		if (travel_distance > min_distance) count++;
	}

	return count;
}

long problem(std::vector<long> times, std::vector<long> distances) {
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
	std::vector<long> times;
	for (auto val : split(times_str, " ")) {
		if (val == "") continue;
		times.push_back(stol(val));
	}

	std::getline(f, line);
    f.close();
	auto distances_str = split(line, "Distance:")[1];
	distances_str = trim(distances_str);
	std::vector<long> distances;
	for (auto val : split(distances_str, " ")) {
		if (val == "") continue;
		distances.push_back(stol(val));
	}

	long margin = problem(times, distances);

    std::cout << "margin: " << margin << std::endl;
}
