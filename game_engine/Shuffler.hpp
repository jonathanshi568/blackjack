#pragma once

#include "types.hpp"

namespace jshi_blackjack {

/*
Shufflers must implement the following methods:
void initialize(const GameConfig& gameConfig);
Card dealCard();
void shuffle();
bool needsShuffle() const;
*/

class MTShuffler {
public:
    MTShuffler();

    Card dealCard();
    void shuffleIfNeeded();

private:
    void shuffle();
    bool needsShuffle() const;

private:
    std::random_device rd_;
    std::mt19937 gen_;

    std::vector<Card> shoe_;
    int numDecks{};
    int cardsRemaining{};
    int penetration{};

};

} // namespace jshi_blackjack