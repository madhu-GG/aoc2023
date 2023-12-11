#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <cstring>
#include <split>

void usage(std::string app_name) {
    std::cout << "Usage: " << app_name << " <input_file>" << std::endl;
}

class FSM {
	using lang = std::map<std::string, std::string>;
	std::string start_node;
	std::string cur_node;
	long steps;
	bool is_end;

	public:
		FSM(const std::string& start) {
			cur_node = start_node = start;
			steps = 0;
			is_end = cur_node[cur_node.length() - 1] == 'Z';
		}

		FSM(const FSM& other) {
			start_node = other.start_node;
			cur_node = start_node;
			steps = other.steps;
			is_end = cur_node[cur_node.length() - 1] == 'Z';
		}

		void step(const lang& left, const lang& right, char inst) {
			is_end = cur_node[2] == 'Z';
			if (is_end) return;

			if (inst == 'L') cur_node = left.at(cur_node);
			else if (inst == 'R') cur_node = right.at(cur_node);
			else steps--;

			steps++;
#ifdef CERR_DEBUG
			std::cerr << start_node << " : " << "{" << cur_node << "} -> " << ((is_end)? "at end" : "not at end") << std::endl;
#endif
		}

		void reset() {
			is_end = false;
			steps = 0;
			cur_node = start_node;
		}

		void walk(const lang& left, const lang& right, std::string inst) {
			reset();
			int cur_inst = 0;
			while (!is_end) {
				step(left, right, inst[cur_inst]);
				cur_inst = (cur_inst + 1) % inst.length();
			}
		}

		inline long get_steps() { return steps; }
		inline bool end() { return is_end; }
};


long gcd(long a, long b) {
	if (a == 0 || b == 0) return 0;

	long high, low;
	high = (a > b)? a : b;
	low = (a > b)? b : a;

	long q = high / low;
	long r = high % low;

	if (r == 0) return low;

	return gcd(r, low);
}

long lcm(long a, long b) {
	long high, low;

	high = (a > b)? a : b;
	low = (a > b)? b : a;

	return low * (high / gcd(low, high));
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
	std::vector<FSM> fsms;
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
		if (node[2] == 'A') fsms.push_back(FSM(node));
	}
    f.close();

	long steps = 0;
	for (auto fsm : fsms) {
		fsm.walk(left_map, right_map, instructions);
		if (steps == 0) steps = fsm.get_steps();
		else steps = lcm(steps, fsm.get_steps());
	}

	std::cout << "Steps: " << steps << std::endl;
}
