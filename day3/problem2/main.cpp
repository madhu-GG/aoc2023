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

// typedef struct sym {
//     int value;
//     en_sym_type type;
//     bool ispartnum, isgear;
//     int row, col_start, col_end, gear_parts[2], gear_count;
// } symbol;



// en_sym_type * get_row_map(std::vector<symbol> symbols, const int max_cols) {
//     en_sym_type * row = new en_sym_type[max_cols];
//     memset(row, 0, sizeof(en_sym_type));

//     for (symbol s : symbols) {
//         if (s.type == EN_SYM_NUM) {
//             for (int j = s.col_start; j <= s.col_end; j++) row[j] = EN_SYM_NUM;
//         } else {
//             row[s.col_start] = s.type;
//         }
//     }

//     return row;
// }

// bool update_has_part(symbol& sym, en_sym_type * row_map, const int max_cols) {
//     for (int j = sym.col_start - 1; j <= sym.col_end + 1; j++) {
//         if (j > 0 && j < max_cols && (row_map[j] & (EN_SYM_OTH|EN_SYM_GEAR))) {
//             sym.ispartnum = true;
//         }
//     }

//     return sym.ispartnum;
// }

// void update_symbols(std::vector<symbol> &syms, en_sym_type * row_map, const int max_cols) {
//     for (symbol& sym : syms) {
//         if (sym.type == EN_SYM_NUM && !sym.ispartnum)
//             update_has_part(sym, row_map, max_cols);
//     }
// }

// std::vector<symbol> get_adj_parts(const std::vector<symbol> &syms, int col) {
//     std::vector<symbol> parts;
//     for (symbol s: syms) {
//         if (s.ispartnum && ((s.col_start - 1) <= col) && (col <= s.col_end + 1)) {
//             parts.push_back(s);
//         }
//     }

//     return parts;
// }

// std::vector<symbol> get_adj_gears(const std::vector<symbol> &syms, int col) {
//     std::vector<symbol> gears;
//     for (symbol s: syms) {
//         if (s.type == EN_SYM_GEAR && ((s.col_start - 1) <= col) && (col <= s.col_end + 1)) {
//             gears.push_back(s);
//         }
//     }

//     return gears;
// }

// void problem(std::deque<std::vector<symbol>> &kernel, int row, int max_cols) {
//     en_sym_type * prev_row_map = nullptr, * cur_row_map = nullptr;
//     std::vector<symbol> cur_row_parts, pre

//     cur_row_map = get_row_map(kernel[row], max_cols);
//     // update kernel[row - 1] based on current row
//     if (row > 0) {
//         prev_row_map = get_row_map(kernel[row - 1], max_cols);
//         update_symbols(kernel[row - 1], cur_row_map, max_cols);

//         // update kernel[row] based on previous row
//         update_symbols(kernel[row], prev_row_map, max_cols);
//     }

//     // update kernel[row] based on current row
//     update_symbols(kernel[row], cur_row_map, max_cols);
// }

// std::vector<symbol> get_row_symbols(std::string line, int row) {
//     const char * line_str = line.c_str();
//     std::vector<symbol> syms;
//     std::vector<char> digits;
//     symbol * n = nullptr;
//     for (int col = 0; col < line.length(); col++) {
//         if (isdigit(line_str[col])) {
//             digits.push_back(line_str[col]);
//             if (!n) {
//                 n = new symbol();
//                 n->col_start = col;
//                 n->value = 0;
//                 n->row = row;
//                 n->type = EN_SYM_NUM;
//                 n->ispartnum = false;
//                 n->gear_count = 0;
//                 n->isgear = false;
//             }
//         } else if (n) {
//             std::string number_str = std::string(digits.begin(), digits.end());
//             n->value = std::stoi(number_str, NULL, 10);
//             n->col_end = col - 1;
//             syms.push_back(*n);
//             delete(n);
//             n = nullptr;
//             digits.clear();
//         }

//         if (!isdigit(line_str[col]) && line_str[col] != '.') { // some symbol, we collect it
//             n = new symbol();
//             n->value = line_str[col];
//             n->type = line_str[col] == '*'? EN_SYM_GEAR: EN_SYM_OTH;
//             n->ispartnum = false;
//             n->row = row;
//             n->col_start = col;
//             n->col_end = col;
//             n->gear_count = 0;
//             n->isgear = false;
//             syms.push_back(*n);
//             delete(n);
//             n = nullptr;
//         }
//     }

//     if (n) {
//         std::string number_str = std::string(digits.begin(), digits.end());
//         n->value = std::stoi(number_str, NULL, 10);
//         n->col_end = line.length() - 1;
//         syms.push_back(*n);
//         delete(n);
//         n = nullptr;
//         digits.clear();
//     }

//     return syms;
// }

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
