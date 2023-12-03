#include <fstream>
#include <iostream>
#include <vector>
#include <cstring>

#include <split>

void usage(std::string app_name) {
    std::cout << "Usage: " << app_name << " <input_file>" << std::endl;
}

bool isdigits(const std::string& s) {
    for (auto c = s.begin(); c < s.end(); c++) {
        if (!isdigit(*c)) {
            return false;
        }
    }

    return true;
}

std::vector<long> problem(std::string line) {
    int reds, greens, blues;
    int min_reds, min_blues, min_greens;
    std::vector<int> possible;
    bool is_possible;

    // 1st split by : for game id and rest
    auto parts = split(line, ":");

    // 2nd split by space on "Game XYZ" for the game id
    auto game = split(parts[0], " ");

    int game_id = 0;
    if (game[0] == "Game") {
        errno = 0;
        game_id = strtol(game[1].c_str(), NULL, 10);
        if (game_id == 0 && errno != 0) {
            std::cerr << "Cannot convert from str to int: " << game[1] << ", errno: " << errno << std::endl;
        }
    } else {
        std::cerr << "Cannot find a Game on line: " << line << std::endl;
    }

    // 3rd split by semi-colon ; for the rounds per game
    auto rounds = split(parts[1], ";");

    min_reds = 0, min_blues = 0, min_greens=0;
    is_possible = true;
    // parse each round and check the cube count
    for (std::string round: rounds) {
        auto cubes = split(round, ",");
        int num = 0;
        reds = greens = blues = 0;
        for (std::string cube: cubes) {
            auto pair = split(cube, " ");

            for(int j = 0; j < pair.size(); j++) {
                if (pair[j] == "") {
                    pair.erase(pair.begin() + j);
                    j--;
                }
            }

            if (isdigits(pair[0])) {
                num = stoi(pair[0], NULL, 10);
            }

            if (pair[1] == "green") {
                greens = num;
                if (min_greens < greens) min_greens = greens;
            } else if (pair[1] == "red") {
                reds = num;
                if (min_reds < reds) min_reds = reds;
            } else if (pair[1] == "blue") {
                blues = num;
                if (min_blues < blues) min_blues = blues;
            }
        }

        std::cerr << "R: " << reds << " G: " << greens << " B: " << blues << std::endl;
        if (is_possible) {
            is_possible = (reds <= 12) && (greens <= 13) && (blues <= 14);
        }
    }
    std::cerr << "(min) R: " << min_reds << " G: " << min_greens << " B: " << min_blues << std::endl;

    if (!is_possible) game_id = 0;
    long power =  min_reds * min_greens * min_blues;
    std::vector<long> result = {game_id, power};
    return result;
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
    long power_sum = 0;
    while(std::getline(f, line)) {
        auto result = problem(line);
        sum += result[0];
        power_sum += result[1];
    }

    f.close();

    std::cout << "sum: " << sum << ", power sum: " << power_sum << std::endl;
}
