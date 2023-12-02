#include <fstream>
#include <iostream>
#include <string>
#include <cstring>
#include <cerrno>

using namespace std;

void usage(string app_name) {
    cout << "Usage: " << app_name << " <input_file>" << endl;
}

int get_number_in_str(const string str) {
    int num = 0, first, last;
    char num_str[3] = {0};
    const char * line_str = str.c_str();
    first = last = -1;
    for (int i = 0; i < str.length(); i++)
    {
        if (isdigit(line_str[i])) {
	   if (first == -1) {
               first = i;
	   }

	   last = i;
        }
    }

    if (first != -1 && last != -1) {
        num_str[0] = line_str[first];
        num_str[1] = line_str[last];
        errno = 0;
        num = strtol(num_str, NULL, 10);
        if (num == 0 && errno != 0) {
            cout << "error converting: "
                 << num_str
                 << "errno(="
                 << errno << "): "
                 << strerror(errno) << endl;
        }
    }

    cout << num_str << " : " << str << endl;

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
        sum += get_number_in_str(line);
    }

    f.close();

    cout << "sum is: " << sum << endl;
}
