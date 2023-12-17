#include <fstream>
#include <iostream>
#include <utility>
#include <vector>
#include <map>
#include <deque>
#include <set>
#include <string>
#include <cmath>

void usage(std::string app_name) {
    std::cout << "Usage: " << app_name << " <input_file>" << std::endl;
}

std::map<char, std::pair<std::string, std::string>> compatibles = {
    {'-', {"-LFS" , "-J7S"}},
    {'F', {"|LJS", "-J7S"}},
    {'7', {"-FLS", "|LJS"}},
    {'L', {"|F7S", "-J7S"}},
    {'J', {"-LFS", "|F7S"}},
    {'|', {"|7FS", "|LJS"}}
};

constexpr double pi = 3.14159265358979323846;

class Graph {
    int rows, cols, start_index;
    char * vertices;
    int * dist;
    bool * inside_cycle, * outside_cycle, * on_cycle;
    size_t inside_count, outside_count;
    std::vector<int> cycle;
    // std::set<int> inside_cycle, outside_cycle, on_cycle;

    using point = std::pair<int, int>;
    double angle(point a, point b, point c) {
        int ac_x = a.second - c.second;
        int ac_y = a.first - c.first;
        int bc_x = b.second - c.second;
        int bc_y = b.first - c.first;
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

    std::vector<int> get_adj_relaxed(int cur, bool * visited) {
        std::vector<int> adj;
        int i = cur / cols, j = cur % cols;
        if (i < 0 || j < 0 || i >= rows || j >= cols) return adj;        
        if (i > 0 && !visited[cur - cols] && !(inside_cycle[cur - cols] || on_cycle[cur - cols])) adj.push_back(cur - cols);
        if (j > 0 && !visited[cur - 1] && !(inside_cycle[cur - 1] || on_cycle[cur - 1])) adj.push_back(cur - 1);
        if (i + 1 < rows && !visited[cur + cols] && !(inside_cycle[cur + cols] || on_cycle[cur + cols])) adj.push_back(cur + cols);
        if (j + 1 < cols && !visited[cur + 1] && !(inside_cycle[cur + 1] || on_cycle[cur + 1])) adj.push_back(cur + 1);

        return adj;
    }

    std::vector<int> get_adj(int i, int j) {
        std::vector<int> adj;

        if (i < 0 || j < 0 || i >= rows || j >= cols) return adj;

        int cur = i * cols + j;
        char v = vertices[cur];
        switch(v) {
            case '-': {
                if (j > 0 && (compatibles[v].first.find(vertices[cur - 1]) != std::string::npos)) {
                    adj.push_back(cur - 1);
                }

                if ((j + 1) < cols && compatibles[v].second.find(vertices[cur + 1]) != std::string::npos) {
                    adj.push_back(cur + 1);
                }
            }
            break;
            case 'F': {
                if ((i + 1) < rows && (compatibles[v].first.find(vertices[cur + cols]) != std::string::npos)) {
                    adj.push_back(cur + cols);
                }

                if ((j + 1) < cols && compatibles[v].second.find(vertices[cur + 1]) != std::string::npos) {
                    adj.push_back(cur + 1);
                }
            }
            break;
            case '7': {
                if ((i + 1) < rows && (compatibles[v].second.find(vertices[cur + cols]) != std::string::npos)) {
                    adj.push_back(cur + cols);
                }

                if (j > 0 && (compatibles[v].first.find(vertices[cur - 1]) != std::string::npos)) {
                    adj.push_back(cur - 1);
                }
            }
            break;
            case 'L': {
                if (i > 0 && (compatibles[v].first.find(vertices[cur - cols]) != std::string::npos)) {
                    adj.push_back(cur - cols);
                }

                if ((j + 1) < cols && compatibles[v].second.find(vertices[cur + 1]) != std::string::npos) {
                    adj.push_back(cur + 1);
                }
            }
            break;
            case 'J': {
                if (j > 0 && (compatibles[v].first.find(vertices[cur - 1]) != std::string::npos)) {
                    adj.push_back(cur - 1);
                }

                if (i > 0 && (compatibles[v].second.find(vertices[cur - cols]) != std::string::npos)) {
                    adj.push_back(cur - cols);
                }
            }
            break;
            case '|': {
                if (i > 0 && (compatibles[v].first.find(vertices[cur - cols]) != std::string::npos)) {
                    adj.push_back(cur - cols);
                }

                if ((i + 1) < rows && (compatibles[v].second.find(vertices[cur + cols]) != std::string::npos)) {
                    adj.push_back(cur + cols);
                }
            }
            break;
            case 'S' : {
                if (i > 0 && (compatibles['|'].first.find(vertices[cur - cols]) != std::string::npos)) {
                    adj.push_back(cur - cols);
                }

                if ((i + 1) < rows && (compatibles['|'].second.find(vertices[cur + cols]) != std::string::npos)) {
                    adj.push_back(cur + cols);
                }

                if (j > 0 && (compatibles['-'].first.find(vertices[cur - 1]) != std::string::npos)) {
                    adj.push_back(cur - 1);
                }

                if ((j + 1) < cols && compatibles['-'].second.find(vertices[cur + 1]) != std::string::npos) {
                    adj.push_back(cur + 1);
                }
            }
            break;
            
            case '.':
            default: break;
        }

        return adj;
    }

    std::vector<int> get_cycle(bool * visited) {
        std::vector<int> c;

        int cur = start_index;
        do {
            c.push_back(cur);
            outside_cycle[cur] = false;
            outside_count--;
            on_cycle[cur] = true;
            visited[cur] = true;

            int i = cur / cols, j = cur % cols;
            auto neigh = get_adj(i, j);
            int v = neigh[0];
            if (on_cycle[v]) v = neigh[1];
            if (on_cycle[v]) break;

            cur = v;
        } while (cur != start_index);
        return c;
    }

    // bool collinear(int a, int b, int c) {
    //     auto ax = a % cols, ay = a / cols;
    //     auto bx = b % cols, by = b / cols;
    //     auto cx = c % cols, cy = c / cols;

    //     return ((ax == bx && ax == cx) || (ay == by) && (ay == cy));
    // }

    // std::vector<int> trim_cycle(const std::vector<int>& cycle) {
    //     std::vector<int> trimmed;
    //     for (int i = 2; i < cycle.size(); i++) {
    //         auto a = cycle[i],
    //              b = cycle[i - 1],
    //              c = cycle[i - 2];

    //         if (collinear(a, b, c)) trimmed.push_back(b);
    //     }

    //     return trimmed;
    // }


    void flood_fill(int ind, bool * visited, bool is_outside = false) {
        std::deque<int> q;
        q.push_back(ind);

        while (!q.empty()) {
            auto cur = q.front();
            q.pop_front();
            for (auto a : get_adj_relaxed(cur, visited)) {
                if (!is_outside) {
                    outside_cycle[a] = false;
                    outside_count--;
                    inside_cycle[a] = true;
                    inside_count++;
                }
                q.push_back(a);
            }

            visited[cur] = true;
        }
    }

    bool inside(int row, int col) {
        double winding = 0.0;
        for (int j = 0; j < cycle.size(); j++) {
            int a, b;
            if (j == 0) a = cycle[cycle.size() - 1];
            else a = cycle[j - 1];

            b = cycle[j];

            auto theta= angle({ a / cols, a % cols },
                                    { b / cols, b % cols },
                                    {row, col});
#ifdef CERR_DEBUG
            // fprintf(stderr, "a: {%d, %d}, b: {%d, %d}, c: {%d, %d}, angle: %f\n", 
            //         a / cols, a % cols, b / cols, b % cols, row, col, theta * 180 / pi);
#endif
            winding += theta;
        }

#ifdef CERR_DEBUG
        fprintf(stderr, "loc: {%d, %d}, angle(rad=%lf): %lf\n", row, col, winding, winding * 180 / pi);
#endif

        return std::abs(winding) >= pi;
    }

    public:
        Graph(std::ifstream& inp) {
            rows = cols = 0;
            std::vector<std::string> lines;
            std::string line;

            std::getline(inp, line);
            lines.push_back(line);
            cols = line.length();
            rows++;

            while(std::getline(inp, line)) {
                lines.push_back(line);
                rows++;
            }

            vertices = new char[rows * cols];
            inside_cycle = new bool[rows * cols];
            inside_count = 0;
            outside_cycle = new bool[rows * cols];
            outside_count = 0;
            on_cycle = new bool[rows * cols];
            dist = new int[rows * cols];
            for (int i = 0; i < rows; i++) {
                for (int j = 0; j < cols; j++) {
                    char v = lines[i].at(j);
                    int k = i * cols + j;
                    dist[k] = -1;
                    vertices[k] = v;
                    outside_cycle[k] = true;
                    outside_count++;
                    on_cycle[k] = inside_cycle[k] = false;
                    if (v == 'S') start_index = k;
                }
            }
            dist[start_index] = 0;
            bool * visited = new bool[rows * cols];
            for (int i = 0; i < rows * cols; i++) {
                visited[i] = false;
            }

            cycle = get_cycle(visited);
            // trimmed_cycle = trim_cycle(cycle);

            for (int ind = 0; ind < rows * cols; ind++) {
                if (visited[ind]) continue;
                visited[ind] = true;
                bool in = inside(ind / cols, ind % cols);
                if (in) {
#ifdef CERR_DEBUG
                    fprintf(stderr, "{%d, %d}: inside\n", ind / cols, ind % cols);
#endif                            
                    outside_cycle[ind] = false;
                    outside_count--;
                    inside_cycle[ind] = true;
                    inside_count++;
                }

                // flood_fill(ind, visited, !in);
            }
        }

        size_t num_inside() {
            return inside_count;
        }

        int find_most_distant() {
            bool * visited = new bool[rows * cols];
            std::deque<int> s;
            s.push_back(start_index);
            int max_index = start_index;

            while (!s.empty()) {
                int cur = s.front();
                int i = cur / cols, j = cur % cols;
                s.pop_front();

#ifdef CERR_DEBUG
                std::cerr << "Visiting: " << vertices[cur] << " index: " << cur << ", dist: " << dist[cur] << " (" << i << ", " << j << ")" << std::endl;
#endif
                if (dist[cur] > dist[max_index]) {
                    max_index = cur;
                }

                visited[cur] = true;
                auto next = get_adj(i, j);
#ifdef CERR_DEBUG
                std::cerr << "Num adj: " << next.size() << std::endl;
#endif
                for (auto a: next) {
                    if (!visited[a]) {
                        dist[a] = dist[cur] + 1;
                        s.push_back(a);
                    }
                }
            }

            return dist[max_index];
        }

        ~Graph() {
            delete[] vertices;
            delete[] dist;
        }
};

int main(int argc, char **argv) {
    if (argc != 2) {
        usage(argv[0]);
        return 0;
    }

    std::string filename = argv[1];
    std::ifstream f(filename);
    Graph g(f);
    f.close();

    std::cout << g.num_inside() << std::endl;
}
