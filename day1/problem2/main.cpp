#include <fstream>
#include <iostream>
#include <string>
#include <cstring>
#include <cerrno>
#include <vector>

using namespace std;

void usage(string app_name) {
    cout << "Usage: " << app_name << " <input_file>" << endl;
}

const vector<string> tokens = {
    "zero", "one", "two", "three",
    "four", "five", "six",
    "seven", "eight", "nine"
};

typedef struct token {
    bool failed; /* is this particular FSM instance dead because of a mismatch in text */
    int charind; /* what's the current expected letter */
} token;

/*
 *  Advance the FSM for all the alive instances with given input char.
 *
 *  args:
 *    fsm (in) -> The set of FSM instances, one for each expected token.
 *    inp (in) -> The input on which the FSM is transitioning.
 *    is_any_alive (inout) -> Are there any tokens still possible to be matched? 
 *  return:
 *    int -> a number >= 0 if the FSM matched a token, representing the digit;
 *           -1 if there is no match yet or no tokens can be matched.
 */
int next(vector<token>& fsm, char inp, bool& is_any_alive) {
    is_any_alive = false;
    for (int i = 0; i < fsm.size(); i++) {
        if (fsm[i].failed) continue;
        int cur = fsm[i].charind;
        if (inp == tokens[i][cur]) {
            is_any_alive = true;
            cur++;
            int end = tokens[i].length();
            if (cur == end) { // an FSM instance matched a complete token
                return i;
            }
        } else { // no more out transitions for the given FSM instance i
            fsm[i].failed = true;
            cur = 0;
        }

        fsm[i].charind = cur; // write back i
    }

    return -1; // no FSM instance alive, OR no completed token match
}

/*
 * Returns the digit located at the start of str; or
 * -1 if the str doesn't start with a digit in either number or word form.
 *
 * Uses FSM to detect digits written as words.
 */
int starts_with_token(const char * str, int len) {
    if (isdigit(str[0])) return str[0];

    vector<token> text_fsm;
    text_fsm.resize(tokens.size(), {false, 0});
    
    int digit;
    bool any_match = true;
    for (int j = 0; j < len && any_match; j++) {
        digit = next(text_fsm, str[j], any_match);
        if (digit != -1) break;
    }

    if (digit != -1) digit += 48;

    return digit;
}

/*
 * Combines the first and last digit in the given string and returns it as a
 * 2-digit number.
 *
 * Implementation Notes:
 * * Digits written as words are detected using FSM.
 * * If there is only one digit, it's taken as both first and last.
 * * If there are two overlapping words that can be read as digits, then both are considered.
 *   - i.e., after reading a token, don't skip ahead in the corpus upto it's length.
 *
 * Optimizations:
 * * It's probably faster to scan from right to left for the last digit.
 */
int find_number(const string str) {
    vector<char> digits;

    const char * line_str = str.c_str();
    int max_len = str.length();
    for (int i = 0; i < max_len; i++) {
        int digit = starts_with_token(&line_str[i], max_len - i);
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

    // uncomment for debug
    //cerr << " string: " << str <<  ", number = " << num << endl;

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
    while(f >> line) { // f.eof() only true after reading past EOF.
        sum += find_number(line);
    }

    f.close();

    cout << "sum is: " << sum << endl;
}
