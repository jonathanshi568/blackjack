#include "BlackjackEngine.hpp"

#include <iostream>

namespace jshi_blackjack {

void Seat::resetHand()
{
    hand.reset();
    boughtInsurance = false;
}

} // namespace jshi_blackjack