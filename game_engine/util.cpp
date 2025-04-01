#include "util.hpp"

namespace jshi_blackjack {

std::vector<Card> generateShoe(int decks)
{
    if (decks <= 0) {
        throw std::runtime_error("Number of decks <= 0");
    }

    std::vector<Card> shoe;
    for (int i = 0; i < decks; i++) {
        for (char v : CARD_VALUES) {
            for (char s : CARD_SUITS) {
                shoe.emplace_back(v, s);
            }
        }
    }

    return shoe;
}

} // namespace jshi_blackjack
