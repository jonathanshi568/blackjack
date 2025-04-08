#pragma once

#include "Deck.hpp"
#include "Player.hpp"
#include "util.hpp"

namespace jshi_blackjack {

struct Seat {
    std::shared_ptr<IPlayer> player;
    Hand hand;
    int bet;
    bool boughtInsurance = false;

    void resetHand();
};

template <typename DeckT>
class BlackjackEngine
{
public:
    BlackjackEngine();

    void startRound();
    bool sitPlayer(const std::shared_ptr<IPlayer>& player, int seat);

private:
    void resetHands();
    void askForBets();
    void dealCards();
    void dealerAction();
    void payout();
    void askForInsurance();
    void payoutInsurance();
    HandResult getHandResult(const Hand& playerHand) const;

private:
    GameConfig gameConfig;
    DeckT deck_;
    Hand dealerHand_;
    std::vector<Seat> seats_;
};

template <typename DeckT>
BlackjackEngine<DeckT>::BlackjackEngine()
{
    const auto& gameConfig = *GameConfig::getInstance();
    seats_.resize(gameConfig.seats);
}

template <typename DeckT>
void BlackjackEngine<DeckT>::resetHands()
{
    dealerHand_.reset();
    for (Seat& seat : seats_) {
        seat.resetHand();
    }
}

template <typename DeckT>
void BlackjackEngine<DeckT>::askForBets()
{
    for (Seat& seat : seats_) {
        if (seat.player) {
            seat.bet = seat.player->getBetAmount();
        }
    }
}

template <typename DeckT>
void BlackjackEngine<DeckT>::dealCards()
{
    for (int i = 0; i < 2; i++) {
        for (Seat& seat : seats_) {
            if (seat.player) {
                seat.hand.addCard(deck_.dealCard());
            }
        }
        dealerHand_.addCard(deck_.dealCard());
    }
}

template <typename DeckT>
void BlackjackEngine<DeckT>::dealerAction()
{
    bool dealerNeedsToPlay = std::any_of(seats_.begin(), seats_.end(), [](const auto& seat) { return seat.player && !(seat.hand.busted() || seat.hand.isBlackjack()); });
    if (dealerNeedsToPlay) {
        while (true) {
            if (dealerHand_.total >= DEALER_STAND_VALUE) {
                // hit/stand soft 17
                break;
            }
            dealerHand_.addCard(deck_.dealCard());
        }
    }
}

template <typename DeckT>
HandResult BlackjackEngine<DeckT>::getHandResult(const Hand& playerHand) const
{
    if (!playerHand.busted()) {
        if (dealerHand_.busted() || playerHand.total > dealerHand_.total) {
            return HandResult::WIN;
        }
        else if (playerHand.total == dealerHand_.total) {
            return HandResult::PUSH;
        }
    }
    return HandResult::LOSE;
}

template <typename DeckT>
void BlackjackEngine<DeckT>::payout()
{
    for (Seat& seat : seats_) {
        if (seat.player) {
            HandResult handResult = getHandResult(seat.hand);
            switch (handResult) {
            case HandResult::WIN:
                seat.player->collect(seat.bet * 2);
                break;
            case HandResult::PUSH:
                seat.player->collect(seat.bet);
                break;
            default:
                break;
            }
        }
    }
}

template <typename DeckT>
void BlackjackEngine<DeckT>::askForInsurance()
{
    for (Seat& seat : seats_) {
        if (seat.player && seat.player->buyInsurance(seat.bet / 2.0)) {
            seat.boughtInsurance = true;
        }
    }
}

template <typename DeckT>
void BlackjackEngine<DeckT>::payoutInsurance()
{
    for (Seat& seat : seats_) {
        if (seat.player && seat.boughtInsurance) {
            seat.player->collect(seat.bet * 1.5);
        }
    }
}

template <typename DeckT>
void BlackjackEngine<DeckT>::startRound()
{
    resetHands();
    askForBets();
    dealCards();

    if (dealerHand_.cards[0].rank == 'A') {
        askForInsurance();
    }
    
    if (dealerHand_.total == HAND_VALUE_MAX) {
        payoutInsurance();
        return;
    }

    for (Seat& seat : seats_) {
        if (seat.player) {
            while (true) {
                Action a = seat.player->action(seat.hand, dealerHand_.cards[0]);
                bool done = false;
                switch (a) {
                case Action::HIT:
                    seat.hand.addCard(deck_.dealCard());
                    if (seat.hand.total >= HAND_VALUE_MAX) {
                        done = true;
                    }
                    break;
                case Action::DOUBLE:
                    seat.hand.addCard(deck_.dealCard());
                    done = true;
                    break;
                case Action::STAND:
                    done = true;
                    break;
                default:
                    throw std::runtime_error("Unrecognized action");
                }
                if (done) {
                    seat.player->notifyHandFinal(seat.hand);
                    break;
                }
            }
        }
    }

    dealerAction();

    std::cout << "Dealer is showing " << dealerHand_.toString() << std::endl;
    payout();
}

template <typename DeckT>
bool BlackjackEngine<DeckT>::sitPlayer(const std::shared_ptr<IPlayer>& player, int seat)
{
    if (seat >= static_cast<int>(seats_.size())) {
        return false;
    }

    if (seats_[seat].player) {
        return false;
    }

    seats_[seat].player = player;
    return true;
}

} // namespace jshi_blackjack