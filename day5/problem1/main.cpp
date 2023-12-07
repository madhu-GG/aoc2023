#include <fstream>
#include <iostream>
#include <unordered_map>
#include <split>
#include <trim>
#include <limits>
#include <utility>

void usage(std::string app_name) {
    std::cout << "Usage: " << app_name << " <input_file>" << std::endl;
}

struct pair_hash {
    template <class T1, class T2>
    std::size_t operator () (const std::pair<T1,T2> &p) const {
        auto h1 = std::hash<T1>{}(p.first);
        auto h2 = std::hash<T2>{}(p.second);

        return h1 ^ h2;  
    }
};


class SeedProblem {
	using pair = std::pair<long, long>;
    std::vector<std::unordered_map<pair, pair, pair_hash>> maps;
    std::vector<std::string> seeds;
    long min;

    long value(const long key, const int level = 0) {
		long next_key;

        if (0 <= level && level < maps.size()) {
			bool found = false;
            auto map = maps[level];
			for (auto r : map) {
				auto src = r.first;
				auto dest = r.second;
				if (src.first <= key && key <= src.second) {
					found = true;
					next_key = dest.first + (key - src.first);
					break;
				}
			}

			if (!found) {
				next_key = key;
			}

            return value(next_key, level + 1);
        } else {
            return key;
        }
    }

    public:
    SeedProblem(std::ifstream& in) {
		min = -1;
		const std::unordered_map<std::string, int> sections = {
			{"seeds: ", -1},
			{"seed-to-soil map:" , 0},
			{"soil-to-fertilizer map:", 1},
			{"fertilizer-to-water map:", 2},
			{"water-to-light map:", 3},
			{"light-to-temperature map:", 4},
			{"temperature-to-humidity map:", 5},
			{"humidity-to-location map:", 6}
		};


        std::string line;
		long k;
        while (std::getline(in, line)) {
			if (line == "") continue;
            bool no_section = true;
            for (auto section : sections) {
                if (line.find(section.first) == 0) {
                    no_section = false;
                    k = section.second;

                    switch(k) {
                        case -1: {
                                     std::string nums_str = split(line, ": ")[1];
                                     seeds = split(nums_str, " ");
                                     break;
                                 }
                        case 0:
                        case 1:
                        case 2:
                        case 3:
                        case 4:
                        case 5:
                        case 6: {
								 std::unordered_map<pair, pair, pair_hash> m;
                                 maps.push_back(m);
                                 break;
						}
                        default:
                                 break;
                    }
                }
            }

            if (no_section) {
                auto mapping = split(line, " ");
                long dest_range_start = stol(trim(mapping[0]));
                long src_range_start = stol(trim(mapping[1]));
                long range_count = stol(trim(mapping[2]));
				long src_range_end = src_range_start + range_count;
				long dest_range_end = dest_range_start + range_count;
				std::pair<long, long> src(src_range_start, src_range_end);
				std::pair<long, long> dest(dest_range_start, dest_range_end);
				maps[k][src] = dest;
            }
        }
    }

    long solution() {
        if (min == -1) {
            min = stol(seeds[0]);
            for (auto j : seeds) {
                auto seed = stol(j);
				if (min == seed) continue;

                if (value(min) > value(seed)) min = seed;
            }
        }

        return value(min);
    }
};


int main(int argc, char **argv) {
    if (argc != 2) {
        usage(argv[0]);
        return 0;
    }

    std::string filename = argv[1];
    std::ifstream f(filename);

	SeedProblem s(f);
    f.close();

    std::cout << "result: " << s.solution() << std::endl;
}
