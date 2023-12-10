#include <fstream>
#include <iostream>
#include <map>

#include <cstring>
#include <split>

void usage(std::string app_name) {
    std::cout << "Usage: " << app_name << " <input_file>" << std::endl;
}

enum HandType {
    EN_HAND_TYPE_HIGH = 0,
    EN_HAND_TYPE_ONE,
    EN_HAND_TYPE_TWO,
    EN_HAND_TYPE_THREE,
    EN_HAND_TYPE_FULLHOUSE,
    EN_HAND_TYPE_FOUR,
    EN_HAND_TYPE_FIVE,
};

class Hand {
    constexpr static const char * card_value = "23456789TJQKA";
    const std::string cards;
    std::map<char, int> value;
    std::map<char, int> count;
    int max_count;
    HandType hand_type;

    void calc_type() {
        switch(count.size()) {
            case 1: hand_type = EN_HAND_TYPE_FIVE; break;
            case 2: hand_type = max_count == 4? EN_HAND_TYPE_FOUR : EN_HAND_TYPE_FULLHOUSE; break;
            case 3: {
                if (max_count == 3) {
                    hand_type = EN_HAND_TYPE_THREE;
                } else if (max_count == 2) {
                    hand_type = EN_HAND_TYPE_TWO;
                }

                break;
            }
            case 4: hand_type = EN_HAND_TYPE_ONE; break;
            case 5: hand_type = EN_HAND_TYPE_HIGH; break;
        }
    }

    public:
        Hand(const std::string& s) : cards{s} {
            max_count = -1;
            for  (auto c : cards) {
                count[c] += 1;
                if (max_count <= count[c]) max_count = count[c];
            }

            for (int j = 0; j < strlen(Hand::card_value); j++) {
                value[card_value[j]] = j + 1;
            }

            calc_type();
        }

        HandType get_type() const {
            return hand_type;
        }

        const std::string& get_cards() const {
            return cards;
        }

        friend bool operator <(const Hand& lhs, const Hand& rhs) {
            if (lhs.hand_type < rhs.hand_type) return true;
            else if (lhs.hand_type > rhs.hand_type) return false;
            else {
                for (int j = 0; j < lhs.cards.size(); j++) {
                    if (lhs.value.at(lhs.cards[j]) < rhs.value.at(rhs.cards[j])) return true;
                    else return false;
                }

                return false;
            }
        }

        inline friend bool operator==(const Hand& lhs, const Hand& rhs) {
            return !(lhs < rhs) && !(lhs > rhs);
        }

        inline friend bool operator> (const Hand& lhs, const Hand& rhs) { return rhs < lhs; }
        inline friend bool operator<=(const Hand& lhs, const Hand& rhs) { return !(lhs > rhs); }
        inline friend bool operator>=(const Hand& lhs, const Hand& rhs) { return !(lhs < rhs); }
};

int main(int argc, char **argv) {
    if (argc != 2) {
        usage(argv[0]);
        return 0;
    }

    std::string filename = argv[1];
    std::ifstream f(filename);

    std::string line;
    std::map<Hand, int> hands;
    std::map<Hand, int> rank;
    while(std::getline(f, line)) {
        auto data = split(line, " ");
        auto cards = data[0];
        auto bids = data[1];
        Hand hand(cards);
        hands[hand] = stol(bids);
    }

    f.close();

    int j = 0;
    long winnings = 0;
    for (auto m : hands) {
        rank[m.first] = ++j;
        auto hand = m.first;
        auto bids = m.second;
        std::cerr << "rank: " << j << ", type: " << hand.get_type() << ": " << hand.get_cards() << " ~ " << bids << std::endl;
        std::cerr << j << " * " << m.second << std::endl;
        winnings += j * m.second;
    }

    std::cout << "winnings: " << winnings << std::endl;
}
