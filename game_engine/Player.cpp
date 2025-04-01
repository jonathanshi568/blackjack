#include "Player.hpp"

namespace jshi_blackjack {

Action InteractivePlayer::action(const Hand& playerHand, const Card& dealerCard)
{
    std::string actionStr;
    std::cout << "Dealer is showing " << dealerCard.rank << dealerCard.suit << std::endl;
    while (true) {
        std::cout << "Your hand is ";
        for (const auto& c : playerHand.cards) {
            std::cout << c.rank << c.suit << " ";
        }
        std::cout << ", total=" << playerHand.total << std::endl;
        std::cout << "Please enter your action:" << std::endl;
        std::cin >> actionStr;
        if (auto it = STRING_TO_ACTION.find(actionStr); it != STRING_TO_ACTION.end()) {
            return it->second;
        }
        std::cout << "Unrecognized action, go again:" << std::endl;
        std::cin.ignore();
    }
    return Action::STAND;
}

double InteractivePlayer::getBetAmount()
{
    double betAmount;
    std::cout << "You have $" << bankroll_ << ". How much would you like to bet?" << std::endl;
    while (true) {
        std::cin >> betAmount;
        if (betAmount > bankroll_) {
            std::cout << "You do not have enough money, try again" << std::endl;
        } else {
            break;
        }
    }

    bet(betAmount);
    return betAmount;
}

void InteractivePlayer::notifyHandFinal(const Hand& hand)
{
    std::cout << "Final hand = " << hand.toString() << std::endl;
}

bool InteractivePlayer::buyInsurance(double cost)
{
    if (cost > bankroll_) {
        std::cout << "Cannot afford insurance" << std::endl;
        return false;
    }
    std::cout << "Dealer showing A, buy insurance? (y/N)" << std::endl;
    char c;
    std::cin >> c;

    if (c == 'y') {
        bankroll_ -= cost;
        return true;
    }
    return false;
}

// Action BasicStrategyPlayer::action(const Hand& playerHand, const Hand& dealerHand)
// {
//     return Action::STAND;
// }

} // namespace jshi_blackjack