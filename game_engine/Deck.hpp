#pragma once

#include "types.hpp"

namespace jshi_blackjack {

/*
Decks must implement the following methods:
void initialize(const GameConfig& gameConfig);
Card dealCard();
void shuffle();
bool needsShuffle() const;
std::vector<double> probabilities() const;
*/

class MTDeck {
public:
    MTDeck();

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

class InfiniteDeck
{
public:
    InfiniteDeck();

    Card dealCard();
    void shuffle() {};
    bool needsShuffle() const { return false; };
    std::vector<double> probabilities() { return probabilities_; }

private:
    std::random_device rd_;
    std::mt19937 gen_;

    std::vector<double> probabilities_;
};

} // namespace jshi_blackjack