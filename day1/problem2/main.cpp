#include <fstream>
#include <iostream>
#include <string>
#include <cstring>
#include <cerrno>
#include <vector>

using namespace std;

const vector<string> tokens = {
    "zero", "one", "two", "three",
    "four", "five", "six",
    "seven", "eight", "nine"
};

void usage(string app_name) {
    cout << "Usage: " << app_name << " <input_file>" << endl;
}

typedef struct token {
    bool failed;
    int charind;
} token;

int next(vector<token>& fsm, char inp, bool& matches) {
    matches = false;
    for (int i = 0; i < fsm.size(); i++) {
        if (fsm[i].failed) continue;
        int cur = fsm[i].charind;
        if (inp == tokens[i][cur]) {
            matches = true;
            cur++;
            int end = tokens[i].length();
            if (cur == end) {
                return i;
            }
        } else {
            fsm[i].failed = true;
            cur = 0;
        }

        fsm[i].charind = cur;
    }

    return -1;
}


int get_token(const char * str, int len) {
    if (isdigit(str[0])) return str[0];

    vector<token> text_fsm;
    text_fsm.resize(tokens.size(), {false, 0});
    
    int index;
    bool any_match = true;
    for (int j = 0; j < len && any_match; j++) {
        index = next(text_fsm, str[j], any_match);
        if (index != -1) break;
    }

    if (index != -1) index += 48;

    return index;
}

int find_number(const string str) {
    vector<char> digits;

    const char * line_str = str.c_str();
    int max_len = str.length();
    for (int i = 0; i < max_len; i++) {
        int digit = get_token(&line_str[i], max_len - i);
        if (digit > -1) {
            digits.push_back((char) digit);
        }
    }

    int num = 0;
    if (digits.size() > 0) {

        char num_str[3] = { digits.front(), digits.back(), '\0'};
        errno = 0;
        num = strtol(num_str, NULL, 10);
        if (num == 0 && errno != 0) {
            cerr << "failed converting str to number:" << num_str << endl;
        }
    }

    cerr << " string: " << str <<  ", number = " << num << endl;

    return num;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        usage(argv[0]);
        return 0;
    }

    string filename = argv[1];
    ifstream f(filename);

    string line;

    int sum = 0, first, last;
    while(f >> line) {
        sum += find_number(line);
    }

    f.close();

    cout << "sum is: " << sum << endl;
}
