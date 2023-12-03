#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <deque>
#include <cstring>

void usage(std::string app_name) {
    std::cout << "Usage: " << app_name << " <input_file>" << std::endl;
}

typedef struct sym {
    int value;
    bool isnum, ispartnum;
    int row, col_start, col_end;
} symbol;

std::vector<symbol> get_row_symbols(std::string line, int row) {
    const char * line_str = line.c_str();
    std::vector<symbol> syms;
    std::vector<char> digits;
    symbol * n = nullptr;
    for (int col = 0; col < line.length(); col++) {
        if (isdigit(line_str[col])) {
            digits.push_back(line_str[col]);
            if (!n) {
                n = new symbol();
                n->col_start = col;
                n->value = 0;
                n->row = row;
                n->isnum = true;
                n->ispartnum = false;
            }
        } else if (n) {
            std::string number_str = std::string(digits.begin(), digits.end());
            n->value = std::stoi(number_str, NULL, 10);
            n->col_end = col - 1;
            syms.push_back(*n);
            delete(n);
            n = nullptr;
            digits.clear();
        }

        if (!isdigit(line_str[col]) && line_str[col] != '.') { // some symbol, we collect it
            n = new symbol();
            n->value = line_str[col];
            n->isnum = false;
            n->ispartnum = false;
            n->row = row;
            n->col_start = col;
            n->col_end = col;
            syms.push_back(*n);
            delete(n);
            n = nullptr;
        }
    }

    if (n) {
        std::string number_str = std::string(digits.begin(), digits.end());
        n->value = std::stoi(number_str, NULL, 10);
        n->col_end = line.length() - 1;
        syms.push_back(*n);
        delete(n);
        n = nullptr;
        digits.clear();
    }

    return syms;
}

typedef enum sym_type {
    EN_SYM_NONE=0,
    EN_SYM_NUM=1,
    EN_SYM_OTH=2
} en_sym_type;

en_sym_type * get_row_map(std::vector<symbol> symbols, const int max_cols) {
    en_sym_type * row = new en_sym_type[max_cols];
    memset(row, 0, sizeof(en_sym_type));

    for (symbol s : symbols) {
        if (s.isnum) {
            for (int j = s.col_start; j <= s.col_end; j++) row[j] = EN_SYM_NUM;
        } else {
            row[s.col_start] = EN_SYM_OTH;
        }
    }

    return row;
}

bool update_has_part(symbol& sym, en_sym_type * row_map, const int max_cols) {
    for (int j = sym.col_start - 1; j <= sym.col_end + 1; j++) {
        if (j > 0 && j < max_cols && row_map[j] == EN_SYM_OTH) {
            sym.ispartnum = true;
        }
    }

    return sym.ispartnum;
}

void update_symbols(std::vector<symbol> &syms, en_sym_type * row_map, const int max_cols) {
    for (symbol& sym : syms) {
        if (sym.isnum && !sym.ispartnum)
            update_has_part(sym, row_map, max_cols);
    }
}

void problem(std::deque<std::vector<symbol>> &kernel, int row, int max_cols) {
    en_sym_type * prev_row_map = nullptr, * cur_row_map = nullptr;

    cur_row_map = get_row_map(kernel[row], max_cols);
    // update kernel[row - 1] based on current row
    if (row > 0) {
        prev_row_map = get_row_map(kernel[row - 1], max_cols);
        update_symbols(kernel[row - 1], cur_row_map, max_cols);

        // update kernel[row] based on previous row
        update_symbols(kernel[row], prev_row_map, max_cols);
    }

    // update kernel[row] based on current row
    update_symbols(kernel[row], cur_row_map, max_cols);
}

int main(int argc, char **argv) {
    if (argc != 2) {
        usage(argv[0]);
        return 0;
    }

    std::string filename = argv[1];
    std::ifstream f(filename);

    std::string line;
    int rows = 0, max_cols;
    std::deque<std::vector<symbol>> kernel;
    while(std::getline(f, line)) {
        max_cols = line.length();
        std::vector<symbol> symbols = get_row_symbols(line, rows);
        kernel.push_back(symbols);
        problem(kernel, rows, max_cols);
        rows++;
    }

    f.close();

    long sum = 0;
    while(!kernel.empty()) {
        std::vector<symbol> syms = kernel.front();

        for (symbol s: syms) {
            if (s.ispartnum) sum += s.value;
        }

        kernel.pop_front();
    }

    std::cout << "sum: " << sum << std::endl;
}
