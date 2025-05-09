#include "Deck.hpp"
#include "util.hpp"

namespace jshi_blackjack {

MTDeck::MTDeck() : gen_{rd_()}
{
    const auto& gameConfig = *GameConfig::getInstance();
    numDecks = gameConfig.decks;
    penetration = gameConfig.penetration;
    shoe_ = generateShoe(numDecks);
    shuffle();
}

Card MTDeck::dealCard()
{
    return shoe_[--cardsRemaining];
}

void MTDeck::shuffle()
{
    cardsRemaining = shoe_.size();
    std::shuffle(shoe_.begin(), shoe_.end(), gen_);
}

bool MTDeck::needsShuffle() const
{
    return cardsRemaining < static_cast<int>(shoe_.size() * (1.0 - penetration));
}

void MTDeck::shuffleIfNeeded()
{
    if (needsShuffle()) {
        shuffle();
    }
}

InfiniteDeck::InfiniteDeck() : gen_{rd_()}
{
    for (int i = 0; i < NUM_CARD_VALUES; i++) {
        probabilities_.push_back(1.0 / NUM_CARD_VALUES);
    }
}

Card InfiniteDeck::dealCard()
{
    return Card{};
}

} // namespace jshi_blackjack