#include <fstream>
#include <iostream>
#include <unordered_map>

void usage(std::string app_name) {
    std::cout << "Usage: " << app_name << " <input_file>" << std::endl;
}

int problem(std::string line) {
    std::vector<std::unordered_map<int, int>> maps;

    if (line.find('seeds:') == 0) {
    } else if (line.find(''))
    return 0;
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
    while(std::getline(f, line)) {
        sum += problem(line);
    }

    f.close();

    std::cout << "sum: " << sum << std::endl;
}
