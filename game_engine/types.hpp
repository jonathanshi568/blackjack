#pragma once

#include <algorithm>
#include <iostream>
#include <memory>
#include <random>
#include <sstream>
#include <stdexcept>
#include <unordered_map>
#include <vector>

namespace jshi_blackjack {

constexpr char CARD_VALUES[] = {'2', '3', '4', '5', '6', '7', '8', '9', 'T', 'J', 'Q', 'K', 'A'};
constexpr char CARD_SUITS[] = {'S', 'H', 'D', 'C'};
constexpr int HAND_VALUE_MAX = 21;
constexpr int DEALER_STAND_VALUE = 17;
const std::unordered_map<char, short> RANK_TO_VALUE = {
    {'2', 2},
    {'3', 3},
    {'4', 4},
    {'5', 5},
    {'6', 6},
    {'7', 7},
    {'8', 8},
    {'9', 9},
    {'T', 10},
    {'J', 10},
    {'Q', 10},
    {'K', 10},
    {'A', 11}
};

enum class Action {
    STAND,
    HIT,
    DOUBLE,
    SPLIT,
    SURRENDER
};

const std::unordered_map<std::string, Action> STRING_TO_ACTION = {
    {"STD", Action::STAND},
    {"HIT", Action::HIT},
    {"DBL", Action::DOUBLE},
    {"SPL", Action::SPLIT},
    {"SUR", Action::SURRENDER},
};

enum class HandResult {
    WIN,
    PUSH,
    LOSE
};

struct Card {
    Card(char rank, char suit) : value{RANK_TO_VALUE.at(rank)}, rank{rank}, suit{suit} {}
    short value;
    char rank;
    char suit;
};

struct Hand {
    int total;
    int betSize;
    std::vector<Card> cards;
    bool soft;
    bool canDouble;
    bool canSplit;
    bool canSurrender;

    void addCard(const Card& card);
    Hand split();
    void reset();
    bool busted() const;
    bool isBlackjack() const;
    std::string toString() const;
};

class GameConfig {
private:
    static GameConfig* instance;
public:
    double penetration = 0.75;
    double bjPayout = 1.5;
    int decks = 6;
    int minBet = 15;
    int maxBet = 1000;
    int seats = 6;
    int maxSplitHands = 4;
    bool hitSoft17 = true;
    bool rsa = true;
    bool das = true;
    bool surrender = true;

    static GameConfig* getInstance() {
        if (instance == nullptr) {
            instance = new GameConfig();
        }
        return instance;
    }
};

} // namespace jshi_blackjack