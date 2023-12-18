#include <fstream>
#include <iostream>
#include <cmath>
#include <string>
#include <vector>
#include <queue>
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

const double pi = 3.14159265358979323846;

double angle(int ar, int ac, int br, int bc, int cr, int cc) {
    int ac_x = ac - cc;
    int ac_y = ar - cr;
    int bc_x = bc - cc;
    int bc_y = br - cr;
    double theta1 = std::atan2(ac_y, ac_x);
    double theta2 = std::atan2(bc_y, bc_x);
    double theta = theta2 - theta1;
#ifdef CERR_DEBUG
    // std::cerr << "ac angle: " << theta1 << ", bc angle: " << theta2 << std::endl;
#endif
    while (theta > pi) theta -= 2 * pi;
    while (theta < -pi) theta += 2 * pi;

    return theta;
}

double slope_diff(const Graph &g, int a, int b, int c) {
    auto ar = a / g.ncols(), ac = a % g.ncols();
    auto br = b / g.ncols(), bc = b % g.ncols();
    auto cr = c / g.ncols(), cc = c % g.ncols();

    //calculate slope of ab and ac
    auto abx = ac - bc, aby = ar - br;
    auto acx = ac - cc, acy = ar - cr;
    if (abx == 0 && acx == 0) return 0; // x-component is 0
    else if (abx == 0 || acx == 0) return 100.0; // one of the slopes is infinite

    double m_ab = aby / abx; if (m_ab < 0) m_ab = -m_ab;
    double m_ac = acy / acx; if (m_ac < 0) m_ac = -m_ac;
    return m_ac - m_ab;
}

std::vector<int> trim(const Graph &g, const std::vector<int>& path) {
    std::vector<int> trimmed;
    /*
    double diff, thres = 0.1;
    int len = path.size();
    for (int i = 2; i < len; i++) {
        diff = slope_diff(g, path[i], path[i - 1], path[i - 2]);

        // i, i - 1, and i - 2 are non-collinear, add only i - 1
        if (diff <= thres) trimmed.push_back(path[i - 1]);
    }

    diff = slope_diff(g, path[0], path[len - 1], path[len - 2]);
    if (diff <= thres) trimmed.push_back(path[len - 1]);
    diff = slope_diff(g, path[1], path[0], path[len - 1]);
    if (diff <= thres) trimmed.push_back(path[0]);
    */

    for (auto index : path) {
        char v = g.at(index);
        if (v != '-' && v != '|') trimmed.push_back(index);
    }

    return trimmed;
}

bool inside(const Graph &g, const std::vector<int>& path, int c) {
    double winding = 0.0;
    auto rows = g.nrows(), cols = g.ncols();
    auto row = c / cols, col = c % cols;

    for (int j = 0; j < path.size(); j++) {
        int a, b;
        if (j == 0) a = path[path.size() - 1];
        else a = path[j - 1];

        b = path[j];

        auto theta = angle(a / cols, a % cols, b / cols, b % cols,
                          row, col);
#ifdef CERR_DEBUG
        // fprintf(stderr, "a: {%d, %d}, b: {%d, %d}, c: {%d, %d}, angle: %f\n", 
        //         a / cols, a % cols, b / cols, b % cols, row, col, theta * 180 / pi);
#endif
        winding += theta;
    }

#ifdef CERR_DEBUG
    //fprintf(stderr, "loc: {%d, %d}, angle(rad=%lf): %lf\n", row, col, winding, winding * 180 / pi);
#endif

    return std::abs(winding) >= pi;
}

void flood(const Graph& g, int start, bool is_inside_loop, std::unordered_set<int>& visited, std::unordered_set<int>& unvisited, int& count) {
    std::queue<int> que;
    que.push(start);
    while(!que.empty()) {
        auto index = que.front();
        que.pop();
        visited.insert(index);
        unvisited.erase(index);
        if (is_inside_loop) count++;

        for (int next: g.adj(index, false)) {
            if (visited.count(next)) continue;
            que.push(next);
        }
    }
}

int num_inside(const Graph& g, const std::vector<int>& path) {
    int count = 0;
    std::unordered_set<int> visited, unvisited;
    int start, index;

    auto tpath = trim(g, path);

    for (int i = 0; i < g.nelems(); i++) {
        unvisited.insert(i);
    }

    for (auto v : path) {
        visited.insert(v);
        unvisited.erase(v);
    }

    std::queue<int> que;
    while(unvisited.size() > 0) {
        start = *(unvisited.begin());

        que.push(start);
        while (!que.empty()) {
            index = que.front();
            que.pop();

            if (visited.count(index) == 0) {
                bool is_inside_loop = inside(g, tpath, index);
                //flood(g, index, is_inside_loop, visited, unvisited, count);
                visited.insert(index);
                unvisited.erase(index);
                if (is_inside_loop) count++;
                for (int next: g.adj(index, false)) {
                    if (visited.count(next)) continue;
                    que.push(next);
                }
            }
        }
    }

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
    auto tpath = trim(g, path);
#ifdef CERR_DEBUG
    print_grid(g, path);
    print_grid(g, tpath);
#endif
    

    std::cout << "Max dist: " << path.size() / 2 << std::endl;
    std::cout << "Num Inside: " << num_inside(g, path) << std::endl;
}
