#include <fstream>
#include <iostream>

#include <unordered_map>
#include <vector>

#include <split>
#include <trim>

void usage(std::string app_name) {
    std::cout << "Usage: " << app_name << " <input_file>" << std::endl;
}

long problem(std::string line) {
    std::vector<std::string> halves = split(line, "|");
    std::string winning, holding;
    winning = halves[0];
    holding = halves[1];

    std::vector<std::string> winning_details = split(winning, ":");
    std::vector<std::string> card = split(winning_details[0], " ");

    int card_id = 0;
    for (auto id: card) {
        if (!(id == "Card" || id == ""))
            card_id = stoi(id, NULL, 10);
    }

    std::unordered_map<int, int> winning_cards, holding_cards;

    std::vector<std::string> winning_cards_as_str = split(trim(winning_details[1]), " ");

    for (int j = 0; j < winning_cards_as_str.size(); j++) {
        if (trim(winning_cards_as_str[j]) == "") {
            continue;
        }
        winning_cards[stoi(winning_cards_as_str[j], NULL, 10)] = j;
    }

    std::vector<std::string> holding_cards_as_str = split(trim(holding), " ");
    for (int j = 0; j < holding_cards_as_str.size(); j++) {
        if (trim(holding_cards_as_str[j]) == "") {
            continue;
        }
        holding_cards[stoi(holding_cards_as_str[j], NULL, 10)] = j;
    }

    long sum = 0;
    unsigned long shift = 0;
    bool first = true;

    for (auto card : holding_cards) {
        if (winning_cards.count(card.first) > 0) {
            sum += 1 << shift;
            if (!first) shift++;
            else first = false;
        }
    }

    return sum;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        usage(argv[0]);
        return 0;
    }

    std::string filename = argv[1];
    std::ifstream f(filename);

    std::string line;
    long sum = 0;
    while(std::getline(f, line)) {
        sum += problem(line);
    }

    f.close();

    std::cout << "sum: " << sum << std::endl;
}
