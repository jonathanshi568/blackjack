#include "types.hpp"

namespace jshi_blackjack {

GameConfig* GameConfig::instance = nullptr;

void Hand::addCard(const Card& card)
{
    // same as first card -> split
    if (cards.size() == 1 && card.value == total) {
        canSplit = true;
    }

    total += card.value;

    if (card.rank == 'A') {
        soft = true;
    }

    if (soft && total > HAND_VALUE_MAX) {
        total -= 10;
        soft = false;
    }

    cards.push_back(card);
}

void Hand::popCard()
{
    auto tmpCards = cards;
    reset();
    for (int i = 0; i < tmpCards.size() - 1; i++) {
        addCard(tmpCards[i]);
    }
}

Hand Hand::split()
{
    return Hand{};
}

void Hand::reset()
{
    const auto& gameConfig = *GameConfig::getInstance();
    total = 0;
    cards.clear();
    soft = false;
    canDouble = true;
    canSplit = false;
    canSurrender = gameConfig.surrender;
}

bool Hand::busted() const
{
    return total > HAND_VALUE_MAX;
}

std::string Hand::toString() const
{
    std::ostringstream oss;
    for (const auto& c : cards) {
        oss << c.rank << c.suit << " ";
    }
    oss << "total=" << total;
    return oss.str();
}

bool Hand::isBlackjack() const
{
    return cards.size() == 2 && total == 21;
}

} // namespace jshi_blackjack