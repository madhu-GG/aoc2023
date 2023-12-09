#include <fstream>
#include <iostream>
#include <map>
#include <unordered_map>
#include <set>
#include <split>
#include <trim>
#include <limits>
#include <utility>

void usage(std::string app_name) {
    std::cout << "Usage: " << app_name << " <input_file>" << std::endl;
}

class SeedProblem {
	using pair = std::pair<long, long>;
	using pair_map = std::map<pair, pair>;
    std::vector<pair_map> maps;
	std::set<pair> seeds;
    long min;

	std::set<pair> partitioned_query(std::set<pair> queries, const pair_map& map) {
		std::set<pair> values;
		for (auto q: queries) {
			bool no_mapping = true;
			for (auto item : map) {
				auto mk = item.first;
				auto mv = item.second;
redo:
				if (q.first > q.second) break;
				if (q.first > mk.second || q.second < mk.first) continue;
				no_mapping = false;
				auto front = mk.first - q.first;
				auto back = mk.second - q.second;
				if (front > 0) { // query start before key start
					auto new_key_start = q.first;
					auto new_key_end = new_key_start + front - 1;
					auto new_key = pair{new_key_start, new_key_end};
					auto new_value = new_key;

					/*new_map[new_key] = new_value;*/
					values.insert(new_value);
					q.first = new_key_end + 1;
					goto redo;
				} else /* if (front <= 0) */ { // key start before query start
					auto new_key_start = q.first;
					auto new_key_end = (back >= 0)? q.second : mk.second;
					auto new_key = pair{new_key_start, new_key_end};
					auto new_value_range = new_key_end - new_key_start;
					auto new_value_start = mv.first + (-front);
					auto new_value_end = new_value_start + new_value_range;
					auto new_value = pair{new_value_start, new_value_end};
					/*new_map[new_key] = new_value;*/
					values.insert(new_value);
					q.first = new_key_end + 1;
					goto redo;
				}
			}

			if (no_mapping) values.insert(q);
		}

		return values;
	}

	long min_value(std::set<pair> queries, const int level = 0) {
		std::set<pair> next_keys;

        if (0 <= level && level < maps.size()) {
			auto map = maps[level];
			next_keys = partitioned_query(queries, map);

			if (next_keys.empty()) {
#ifdef CERR_DEBUG
				std::cerr << "EMPTY!" << std::endl;
#endif
				next_keys = queries;
			}

#ifdef CERR_DEBUG
			std::cerr << "level: " << level;
			for (auto it : next_keys) {
				std::cerr << ", (" << it.first << ", " << it.second << ")";
			}
			std::cerr << std::endl;
#endif

			return min_value(next_keys, level + 1);
        } else {
			auto it = queries.begin();
			return (*it).first;
		}
	}

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
                                     auto seeds_str = split(nums_str, " ");
									 for (int j = 0; j < seeds_str.size(); j+=2) {
										 long start = stol(seeds_str[j]);
										 long end = start + stol(seeds_str[j + 1]) - 1;
										 seeds.insert(pair{start, end});
									 }
                                     break;
                                 }
                        case 0:
                        case 1:
                        case 2:
                        case 3:
                        case 4:
                        case 5:
                        case 6: {
								 std::map<pair, pair> m;
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
				long src_range_end = src_range_start + range_count - 1;
				long dest_range_end = dest_range_start + range_count - 1;
				std::pair<long, long> src(src_range_start, src_range_end);
				std::pair<long, long> dest(dest_range_start, dest_range_end);
				maps[k][src] = dest;
            }
        }
    }

    long solution() {
		return min_value(seeds);
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
