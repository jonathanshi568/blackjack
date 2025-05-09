#pragma once

#include <algorithm>
#include <iostream>
#include <map>
#include <memory>
#include <random>
#include <sstream>
#include <stdexcept>
#include <unordered_map>
#include <vector>

namespace jshi_blackjack {

constexpr char CARD_VALUES[] = {'2', '3', '4', '5', '6', '7', '8', '9', 'T', 'J', 'Q', 'K', 'A'};
constexpr char CARD_SUITS[] = {'S', 'H', 'D', 'C'};
constexpr int NUM_CARD_VALUES = 13;
constexpr int NUM_SUITS = 4; 
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
    Card() {}
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
    void popCard();
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

enum class DealerResults {
    _17 = 0,
    _18 = 1,
    _19 = 2,
    _20 = 3,
    _21 = 4,
    _BUST = 5,
    _BJ = 6,
    _DEALER_RESULTS_MAX = 7
};

const std::unordered_map<DealerResults, std::string> DEALER_RESULTS_TO_STR = {
    {DealerResults::_17, "17"},
    {DealerResults::_18, "18"},
    {DealerResults::_19, "19"},
    {DealerResults::_20, "20"},
    {DealerResults::_21, "21"},
    {DealerResults::_BJ, "BJ"},
    {DealerResults::_BUST, "BUST"},
};

} // namespace jshi_blackjack