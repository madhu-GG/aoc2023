#include <fstream>
#include <iostream>
#include <utility>
#include <vector>
#include <map>
#include <deque>
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

class Graph {
    int rows, cols, start_index;
    char * vertices;
    bool * visited;
    int * dist;

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
            visited = new bool[rows * cols];
            dist = new int[rows * cols];
            for (int i = 0; i < rows; i++) {
                for (int j = 0; j < cols; j++) {
                    char v = lines[i].at(j);
                    int k = i * cols + j;
                    visited[k] = false;
                    dist[k] = -1;
                    vertices[k] = v;
                    if (v == 'S') start_index = k;
                }
            }
            dist[start_index] = 0;
        }

        int find_most_distant() {
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
            delete[] visited;
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

    std::cout << g.find_most_distant() << std::endl;
}
