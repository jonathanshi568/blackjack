#include "Shuffler.hpp"
#include "util.hpp"

namespace jshi_blackjack {

MTShuffler::MTShuffler() : gen_{rd_()}
{
    const auto& gameConfig = *GameConfig::getInstance();
    numDecks = gameConfig.decks;
    penetration = gameConfig.penetration;
    shoe_ = generateShoe(numDecks);
    shuffle();
}

Card MTShuffler::dealCard()
{
    return shoe_[--cardsRemaining];
}

void MTShuffler::shuffle()
{
    cardsRemaining = shoe_.size();
    std::shuffle(shoe_.begin(), shoe_.end(), gen_);
}

bool MTShuffler::needsShuffle() const
{
    return cardsRemaining < static_cast<int>(shoe_.size() * (1.0 - penetration));
}

void MTShuffler::shuffleIfNeeded()
{
    if (needsShuffle()) {
        shuffle();
    }
}

} // namespace jshi_blackjack