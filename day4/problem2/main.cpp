#include <fstream>
#include <iostream>

#include <unordered_map>
#include <vector>

#include <deque>
#include <split>
#include <trim>

void usage(std::string app_name) {
    std::cout << "Usage: " << app_name << " <input_file>" << std::endl;
}

class Card {
    int id;
    long points, num_matching;
    std::unordered_map<int, int> winning_numbers, holding_numbers;

    public:
        Card(std::string line) {
            std::vector<std::string> halves = split(line, "|");
            std::string winning, holding;

            winning = halves[0];
            std::vector<std::string> winning_details = split(winning, ":");
            std::vector<std::string> card = split(winning_details[0], " ");

            for (auto i: card) {
                if (!(i == "Card" || i == ""))
                    this->id = stoi(i, NULL, 10);
            }

            std::vector<std::string> winning_nums_as_str = split(trim(winning_details[1]), " ");
            for (int j = 0; j < winning_nums_as_str.size(); j++) {
                if (trim(winning_nums_as_str[j]) == "") {
                    continue;
                }
                this->winning_numbers[stoi(winning_nums_as_str[j], NULL, 10)] = j;
            }

            holding = halves[1];
            std::vector<std::string> holding_nums_as_str = split(trim(holding), " ");
            for (int j = 0; j < holding_nums_as_str.size(); j++) {
                if (trim(holding_nums_as_str[j]) == "") {
                    continue;
                }
                this->holding_numbers[stoi(holding_nums_as_str[j], NULL, 10)] = j;
            }

            unsigned long shift = 0;
            bool first = true;

            points = 0;
            num_matching = 0;
            for (auto number : this->holding_numbers) {
                if (this->winning_numbers.count(number.first) > 0) {
                    points += 1 << shift;
                    if (!first) shift++;
                    else first = false;

                    num_matching++;
                }
            }
        }

        Card(const Card& other) {
            id = other.id;
            points = other.points;
            winning_numbers = other.winning_numbers;
            holding_numbers = other.holding_numbers;
            num_matching = other.num_matching;
        }

        int get_id() {
            return this->id;
        }

        long get_points() {
            return this->points;
        }

        long get_num_matching() {
            return this->num_matching;
        }
};

std::vector<long> problem(std::deque<int> queue, std::unordered_map<int, Card*>& cards) {
    long sum = 0;
    long cards_count = 0;
    std::unordered_map<int, int> counts;

    while(!queue.empty()) {
        int index = queue.front(), cur_id;
        Card * cur = cards[index];
        cur_id = cur->get_id();
        queue.pop_front();

        counts[cur_id] = counts[cur_id] + 1;
        int clone_count = cur->get_num_matching();

        // std::cerr << "Card " << id << ", points: " << clone_count <<  std::endl;
        sum += clone_count;
        for (int copy_id = cur_id + 1; copy_id <= cur_id + clone_count; copy_id++) {
            queue.push_back(copy_id);
        }
    }

    for (auto c : counts) {
        cards_count += c.second;
    }

    return std::vector<long> {sum, cards_count};
}

int main(int argc, char **argv) {
    if (argc != 2) {
        usage(argv[0]);
        return 0;
    }

    std::string filename = argv[1];
    std::ifstream f(filename);

    std::string line;
    long sum = 0, cards_count = 0;
    std::deque<int> card_queue;
    std::unordered_map<int, Card*> cards;

    while(std::getline(f, line)) {
        Card * c = new Card(line);
        card_queue.push_back(c->get_id());
        cards[c->get_id()] = c;
    }
    f.close();

    auto result = problem(card_queue, cards);
    sum = result[0];
    cards_count = result[1];

    std::cout << "sum: " << sum << ", count: " << cards_count << std::endl;
}
