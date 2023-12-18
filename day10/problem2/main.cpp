#include <fstream>
#include <iostream>
#include <cmath>
#include <string>
#include <vector>
#include <queue>
#include <set>
#include <unordered_set>
#include <map>

void usage(std::string app_name) {
    std::cout << "Usage: " << app_name << " <input_file>" << std::endl;
}

class Graph {
    char * V;
    int rows, cols, N, start_index;

    static const std::map<char, std::pair<std::string, std::string>> compatibles;

    public:
        Graph(std::ifstream &in);
        char at(size_t index) const;
        char at(int row, int col) const;
        std::vector<int> adj(int index, bool strict = true) const;
        std::vector<int> adj(int row, int col, bool strict = true) const;
        int start() const { return start_index; }
        int nelems() const { return N; }
        int nrows() const { return rows; }
        int ncols() const { return cols; }

        ~Graph() {
            delete[] V;
        }
};

const std::map<char, std::pair<std::string, std::string>> Graph::compatibles =
{
    {'-', {"-LFS" , "-J7S"}},
    {'F', {"|LJS", "-J7S"}},
    {'7', {"-FLS", "|LJS"}},
    {'L', {"|F7S", "-J7S"}},
    {'J', {"-LFS", "|F7S"}},
    {'|', {"|7FS", "|LJS"}}
};

Graph::Graph(std::ifstream &in) {
    std::vector<std::string> lines;
    std::string line;

    rows = cols = N = 0;
    while(std::getline(in, line)) {
        lines.push_back(line);
        rows++;
        if (cols == 0) cols = line.length();
    }

    N = rows * cols;
    if (N > 0) V = new char[N];
    start_index = -1;
    for (int i = 0; i < N; i++) {
        auto row = i / cols, col = i % cols;
        V[i] = lines[row].at(col);
        if (V[i] == 'S') start_index = i;
    }
}

char Graph::at(size_t index) const {
    if (V && (0 <= index && index < N)) return V[index];
    else return 0;
}

char Graph::at(int row, int col) const {
    size_t index = row * cols + col;
    return this->at(index);
}

std::vector<int> Graph::adj(int row, int col, bool strict) const {
    size_t index = row * cols + col;
    return this->adj(index, strict);
}

std::vector<int> Graph::adj(int index, bool strict) const {
    std::vector<int> adj_vec;

    if (index < 0 || index >= N) return adj_vec;
    int i = index / cols, j = index % cols;
    if (strict) {
        char v = V[index];
        switch(v) {
            case '-': {
                if (j > 0 && (compatibles.at(v).first.find(V[index - 1]) != std::string::npos)) {
                    adj_vec.push_back(index - 1);
                }

                if ((j + 1) < cols && compatibles.at(v).second.find(V[index + 1]) != std::string::npos) {
                    adj_vec.push_back(index + 1);
                }
            }
            break;
            case 'F': {
                if ((i + 1) < rows && (compatibles.at(v).first.find(V[index + cols]) != std::string::npos)) {
                    adj_vec.push_back(index + cols);
                }

                if ((j + 1) < cols && compatibles.at(v).second.find(V[index + 1]) != std::string::npos) {
                    adj_vec.push_back(index + 1);
                }
            }
            break;
            case '7': {
                if ((i + 1) < rows && (compatibles.at(v).second.find(V[index + cols]) != std::string::npos)) {
                    adj_vec.push_back(index + cols);
                }

                if (j > 0 && (compatibles.at(v).first.find(V[index - 1]) != std::string::npos)) {
                    adj_vec.push_back(index - 1);
                }
            }
            break;
            case 'L': {
                if (i > 0 && (compatibles.at(v).first.find(V[index - cols]) != std::string::npos)) {
                    adj_vec.push_back(index - cols);
                }

                if ((j + 1) < cols && compatibles.at(v).second.find(V[index + 1]) != std::string::npos) {
                    adj_vec.push_back(index + 1);
                }
            }
            break;
            case 'J': {
                if (j > 0 && (compatibles.at(v).first.find(V[index - 1]) != std::string::npos)) {
                    adj_vec.push_back(index - 1);
                }

                if (i > 0 && (compatibles.at(v).second.find(V[index - cols]) != std::string::npos)) {
                    adj_vec.push_back(index - cols);
                }
            }
            break;
            case '|': {
                if (i > 0 && (compatibles.at(v).first.find(V[index - cols]) != std::string::npos)) {
                    adj_vec.push_back(index - cols);
                }

                if ((i + 1) < rows && (compatibles.at(v).second.find(V[index + cols]) != std::string::npos)) {
                    adj_vec.push_back(index + cols);
                }
            }
            break;
            case 'S' : {
                if (i > 0 && (compatibles.at('|').first.find(V[index - cols]) != std::string::npos)) {
                    adj_vec.push_back(index - cols);
                }

                if ((i + 1) < rows && (compatibles.at('|').second.find(V[index + cols]) != std::string::npos)) {
                    adj_vec.push_back(index + cols);
                }

                if (j > 0 && (compatibles.at('-').first.find(V[index - 1]) != std::string::npos)) {
                    adj_vec.push_back(index - 1);
                }

                if ((j + 1) < cols && compatibles.at('-').second.find(V[index + 1]) != std::string::npos) {
                    adj_vec.push_back(index + 1);
                }
            }
            break;
            
            case '.':
            default: break;
        }
    } else /* if (!strict) */ {
        if (i > 0) adj_vec.push_back(index - cols);
        if (j > 0) adj_vec.push_back(index - 1);
        if (i + 1 < rows) adj_vec.push_back(index + cols);
        if (j + 1 < cols) adj_vec.push_back(index + 1);
    }

    return adj_vec;
}

std::vector<int> cycle(const Graph& g) {
    std::vector<int> path;
    std::map<int, bool> visited;
    int start, index, next;
    start = g.start();
    
    index = start;
    do {
        path.push_back(index);
        visited[index] = true;
        auto neigh = g.adj(index);

        next = neigh[0];
        if (visited[next]) next = neigh[1];
        if (visited[next]) break;

        index = next;
    } while (index != start);
    
    visited.clear();
    return path;
}

char connector(char top, char down, char left, char right) {
    std::set<char> candidates = { '-', '|', 'F', '7', 'J', 'L' };
    std::string top_compat = "7|F", down_compat = "J|L",
        left_compat = "F-L", right_compat = "7-J";

    if (top_compat.find(top) == std::string::npos) for (auto v : down_compat) candidates.erase(v);
    if (candidates.size() == 1) return *(candidates.begin());
    if (left_compat.find(left) == std::string::npos) for (auto v : right_compat) candidates.erase(v);
    if (candidates.size() == 1) return *(candidates.begin());

    if (down_compat.find(down) == std::string::npos) for (auto v : top_compat) candidates.erase(v);
    if (candidates.size() == 1) return *(candidates.begin());
    if (right_compat.find(right) == std::string::npos) for (auto v : left_compat) candidates.erase(v);
    if (candidates.size() == 1) return *(candidates.begin());

#ifdef CERR_DEBUG
    std::cerr << "Too many/Too few candidates left: " << candidates.size() << std::endl;
    for (auto v: candidates) std::cout << v << std::endl;
#endif
    return '\0';
}

bool inside(char ** grid, int rows, int cols, int c) {
    int count = 0;
    char stack;
    auto row = c / cols, col = c % cols;

    for (int j = col + 1; j < cols; j++) {
        switch (grid[row][j]) {
            case '|': count++; break;
            case 'F': stack = 'F'; break;
            case '7': if (stack == 'L') count++; stack = '\0'; break;
            case 'L': stack = 'L'; break;
            case 'J': if(stack == 'F') count++; stack = '\0'; break;
            break;
        }
    }
    return count % 2 != 0;
}

int num_inside(char ** grid, const std::vector<int>& path, int rows, int cols) {
    int count = 0;
    std::unordered_set<int> visited;

    for (auto v : path) visited.insert(v);

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            auto v = i * cols + j;
            if (!visited.count(v) && inside(grid, rows, cols, v)) {
                visited.insert(v);
#ifdef CERR_DEBUG
                std::cerr << i * cols + j << " : {" << i << ", " << j << "}" << std::endl;
#endif
                count++;
            }
        }
    }

    visited.clear();
    return count;
}

#ifdef CERR_DEBUG
void print_grid(const Graph& g, const std::vector<int>& path, const char symbol = '\0')
{
    char ** grid = new char*[g.nrows()];
    
    for (int i = 0; i < g.nrows(); i++) {
        grid[i]= new char[g.ncols()];
        for (int j = 0; j < g.ncols(); j++) {
            grid[i][j] = '.';
        }
    }
    
    for (auto v : path) {
        auto vr = v / g.ncols(), vc = v % g.ncols();
        if (symbol == '\0') grid[vr][vc] = g.at(v);
        else grid[vr][vc] = symbol;
    }

    std::cerr << std::endl;
    for (int i = 0; i < g.nrows(); i++) {
        for (int j = 0; j < g.ncols(); j++) {
            std::cerr << grid[i][j];
        }
        std::cerr << std::endl;
        delete[] grid[i];
    }
    std::cerr << std::endl;

    delete[] grid;
}
#endif

int main(int argc, char **argv) {
    if (argc != 2) {
        usage(argv[0]);
        return 0;
    }

    std::string filename = argv[1];
    std::ifstream f(filename);
    Graph g(f);
    f.close();

    auto path = cycle(g);
#ifdef CERR_DEBUG
    print_grid(g, path);
#endif

    char ** grid = new char*[g.nrows()];
    for (int i = 0; i < g.nrows(); i++) {
        grid[i]= new char[g.ncols()];
        for (int j = 0; j < g.ncols(); j++) {
            grid[i][j] = '.';
        }
    }

    for (auto v : path) {
        auto vr = v / g.ncols(), vc = v % g.ncols();
        auto c = g.at(v);

        if (c == 'S') {
            c = connector(
                (vr > 0)? g.at(v - g.ncols()) : '\0',
                (vr + 1 < g.nrows())? g.at(v + g.ncols()) : '\0',
                (vc > 0)? g.at(v - 1) : '\0',
                (vc + 1 < g.ncols())? g.at(v + 1) : '\0'
            );
        }

        grid[vr][vc] = c;
    }

#ifdef CERR_DEBUG
    std::cerr << "After guessing S: " << std::endl;
    for (int i = 0; i < g.nrows(); i++) {
        for (int j = 0; j < g.ncols(); j++) {
            std::cerr << grid[i][j];
        }
        std::cerr << std::endl;
    }
#endif

    std::cout << "Max dist: " << path.size() / 2 << std::endl;
    std::cout << "Num Inside: " << num_inside(grid, path, g.nrows(), g.ncols()) << std::endl;

    for (int i = 0; i < g.nrows(); i++) {
        delete[] grid[i];
    }

    delete[] grid;
}
