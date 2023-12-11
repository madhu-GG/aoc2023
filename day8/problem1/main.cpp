#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <cstring>
#include <split>

void usage(std::string app_name) {
    std::cout << "Usage: " << app_name << " <input_file>" << std::endl;
}


int main(int argc, char **argv) {
    if (argc != 2) {
        usage(argv[0]);
        return 0;
    }

    std::string filename = argv[1];
    std::ifstream f(filename);

    std::string instructions;
	std::getline(f, instructions);

	std::map<std::string, std::string> left_map, right_map;
	std::string statement, node, left, right;

	while (std::getline(f, statement)) {
		if (statement == "") continue;
		auto parts = split(statement, " = ");
		node = parts[0];
		auto next = split(parts[1], ", ");
		left = next[0].erase(0, 1);
		right = next[1].erase(next[1].size() - 1);

		left_map[node] = left;
		right_map[node] = right;
	}
    f.close();

	std::string cur_node = "AAA", end_node = "ZZZ";
	long steps = 0;
	int cur_inst = 0;
	while(cur_node != "ZZZ" && (0 <= cur_inst && cur_inst < instructions.length())) {
		if (instructions[cur_inst] == 'L') cur_node = left_map[cur_node];
		else cur_node = right_map[cur_node];

		cur_inst = ( cur_inst + 1 ) % instructions.length();
		++steps;
	}

	std::cout << "Steps: " << steps << std::endl;
}
