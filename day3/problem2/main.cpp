#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <deque>
#include <cstring>

typedef enum sym_type {
    EN_SYM_NONE=0,
    EN_SYM_NUM=1,
    EN_SYM_OTH=2,
    EN_SYM_GEAR=4
} en_sym_type;

void usage(std::string app_name) {
    std::cout << "Usage: " << app_name << " <input_file>" << std::endl;
}

std::vector<long> problem(std::vector<std::string>& lines, const int max_rows, const int max_cols) {
    long sum = 0, product, product_sum = 0;
    std::vector<char> digits;
    std::vector<std::vector<en_sym_type>> syms;
    syms.resize(max_rows);

    std::vector<std::vector<int>> start;
    start.resize(max_rows);
    std::vector<std::vector<int>> end;
    end.resize(max_rows);
    std::vector<std::vector<int>> number;
    number.resize(max_rows);

    std::vector<long> result;

    for (int i = 0; i < max_rows; i++) {
        syms[i].resize(max_cols);
        start[i].resize(max_cols);
        end[i].resize(max_cols);
        number[i].resize(max_cols);
        for (int j = 0; j < max_cols; ) {
            if (isdigit(lines[i][j])) {
                while(j < max_cols && isdigit(lines[i][j])) {
                    syms[i][j] = EN_SYM_NUM;
                    digits.push_back(lines[i][j]);
                    j++;
                }

                std::string value_str = std::string(digits.begin(), digits.end());
                int value = stoi(value_str, NULL, 10);
                for (int k = 0; k < digits.size(); k++) {
                    start[i][(j - 1) - k] = j - digits.size();
                    end[i][(j - 1) - k] = j - 1;
                    number[i][(j - 1) - k] = value;
                }
                
                digits.clear();
            } else if (lines[i][j] == '.') {
                start[i][j] = j;
                end[i][j] = j;
                syms[i][j] = EN_SYM_NONE;
                number[i][j] = 0;
                j++;
            } else if (lines[i][j] == '*') {
                start[i][j] = j;
                end[i][j] = j;
                syms[i][j] = EN_SYM_GEAR;
                number[i][j] = 0;
                j++;
            } else {
                start[i][j] = j;
                end[i][j] = j;
                syms[i][j] = EN_SYM_OTH;
                number[i][j] = 0;
                j++;
            }
        }
    }

    for (int i = 0; i < max_rows; i++) {
        for (int j = 0; j < max_cols; j++) {
            if(syms[i][j] & (EN_SYM_GEAR|EN_SYM_OTH)) {
                int adj_part_count = 0;
                bool *counted = new bool[max_cols];
                // check grid
                product = 1;
                for (int k = (i > 0)? i - 1: i; k <= i + 1 && k < max_rows; k++) {
                    memset(counted, 0, max_cols * sizeof(*counted));
                    for (int l = (j > 0)? j - 1: j; l <= j + 1 && l < max_cols; l++) {
                        if (syms[k][l] == EN_SYM_NUM && !counted[l]) {
                            adj_part_count++;
                            sum += number[k][l];
                            product *= number[k][l];
                            for (int p = start[k][l]; p <= end[k][l]; p++) {
                                counted[p] = true;
                            }
                        }
                    }
                }

                if (adj_part_count == 2 && (syms[i][j] & EN_SYM_GEAR)) {
                    product_sum += product;
                }

                delete[] counted;
            }
        }
    }

    // cleanup
    for (int i = 0; i < max_rows; i++) {
        syms[i].clear();
        start[i].clear();
        end[i].clear();
        number[i].clear();
    }

    syms.clear();
    start.clear();
    end.clear();
    number.clear();

    result.push_back(sum);
    result.push_back(product_sum);
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
    std::vector<std::string> lines;
    int max_rows = 0, max_cols = 0;
    while(std::getline(f, line)) {
        lines.push_back(line);
        max_cols = line.length();
        max_rows++;
    }
    f.close();

    std::vector<long> result = problem(lines, max_rows, max_cols);
    std::cout << "sum: " << result[0] << ", product sum: " << result[1] << std::endl;
}
