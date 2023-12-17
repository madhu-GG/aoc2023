#include <fstream>
#include <iostream>
#include <utility>
#include <vector>
#include <map>
#include <deque>
#include <set>
#include <string>

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
    std::vector<int> cycle;
    std::set<int> inside_cycle, outside_cycle, on_cycle;

    std::vector<int> get_adj_relaxed(int cur) {
        std::vector<int> adj;
        int i = cur / cols, j = cur % cols;
        if (i < 0 || j < 0 || i >= rows || j >= cols) return adj;        
        if (i > 0) adj.push_back(cur - cols);
        if (j > 0) adj.push_back(cur - 1);
        if (i + 1 < rows) adj.push_back(cur + cols);
        if (j + 1 < cols) adj.push_back(cur + 1);

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
            outside_cycle.erase(cur);
            on_cycle.insert(cur);
            visited[cur] = true;

            int i = cur / cols, j = cur % cols;
            auto neigh = get_adj(i, j);
            int v = neigh[0];
            if (on_cycle.find(v) != on_cycle.end()) v = neigh[1];
            if (on_cycle.find(v) != on_cycle.end()) break;

            cur = v;
        } while (cur != start_index);
        return c;
    }

    void flood_fill(int ind, bool * visited) {
        std::deque<int> q;
        q.push_back(ind);

        while (!q.empty()) {
            auto cur = q.front();
            q.pop_front();
            visited[cur] = true;
            for (auto a : get_adj_relaxed(cur)) {
                if (visited[a] || a == cur) continue;
                inside_cycle.insert(a);
                q.push_back(a);
            }
        }
    }

    bool inside(int row, int col) {
        int count = 0, ind = row * cols + col;
        if (on_cycle.find(ind) != on_cycle.end()) return false;
        if (inside_cycle.find(ind) != inside_cycle.end()) return true;
        for (int j = col + 1; j < cols; j++) {
            int cur = row * cols + j;
            if (on_cycle.find(cur) != on_cycle.end()) count++;
        }

        if (count % 2 != 0) {
            return true;
        }

        return false;
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
            
            dist = new int[rows * cols];
            for (int i = 0; i < rows; i++) {
                for (int j = 0; j < cols; j++) {
                    char v = lines[i].at(j);
                    int k = i * cols + j;
                    dist[k] = -1;
                    vertices[k] = v;
                    outside_cycle.insert(k);
                    if (v == 'S') start_index = k;
                }
            }
            dist[start_index] = 0;
            bool * visited = new bool[rows * cols];
            cycle = get_cycle(visited);

            for (int i = 0; i < rows; i++) {
                for (int j = 0; j < cols; j++) {
                    int ind = i * cols + j;
                    if (visited[ind]) continue;
                    if (outside_cycle.count(ind) > 0) {
                        if (inside(i, j)) {
                            outside_cycle.erase(ind);
                            inside_cycle.insert(ind);
                            flood_fill(ind, visited);
                        }
                    }
                }
            }
        }

        size_t num_inside() {
            return inside_cycle.size();
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
