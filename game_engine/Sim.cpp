#include "Sim.hpp"

namespace jshi_blackjack {

void Sim::start()
{
  calculateDealerProbabilities();
  calculateBasicStrategy();
}

void Sim::calculateDealerProbabilities()
{
  dealerOdds_.resize(NUM_CARD_VALUES);
  for (auto& odds : dealerOdds_) {
    odds.resize(static_cast<int>(DealerResults::_DEALER_RESULTS_MAX));
  }
  const auto& probabilities = deck_.probabilities();
  auto helper = [&] (int upCardIdx, double currentProb, Hand& currentHand, auto&& helper) -> void {
    for (int i = 0; i < NUM_CARD_VALUES; i++) {
      currentHand.addCard(Card(CARD_VALUES[i], 'S'));
      double newProb = currentProb * probabilities[i];
      if (currentHand.isBlackjack()) {
        dealerOdds_[upCardIdx][static_cast<int>(DealerResults::_BJ)] += newProb;
      } else if (currentHand.total > HAND_VALUE_MAX) {
        dealerOdds_[upCardIdx][static_cast<int>(DealerResults::_BUST)] += newProb;
      } else if (currentHand.total >= 17) {
        dealerOdds_[upCardIdx][currentHand.total-17] += newProb;
      } else {
        helper(upCardIdx, newProb, currentHand, helper);
      }
      currentHand.popCard();
    }
  };
  
  Hand hand;
  for (int i = 0; i < NUM_CARD_VALUES; i++) {
    hand.reset();
    Card upCard(CARD_VALUES[i], 'S');
    hand.addCard(upCard);
    helper(i, 1.0, hand, helper);
  }

  std::cout << "R 17 18 19 20 21 BJ BUST" << std::endl;
  for (int i = 0; i < NUM_CARD_VALUES; i++) {
    std::cout << CARD_VALUES[i] << " ";
    for (int j = 0; j < static_cast<int>(DealerResults::_DEALER_RESULTS_MAX); j++) {
      std::cout << dealerOdds_[i][j] << " ";
    }
    std::cout << std::endl;
  }
}

void Sim::calculateBasicStrategy()
{
  // stand ev
  Hand h;
  hardTotalStandEv_.resize(HAND_VALUE_MAX + 1);
  for (int handTotal = 0; handTotal < hardTotalStandEv_.size(); handTotal++) {
    hardTotalStandEv_[handTotal].resize(NUM_CARD_VALUES);
    std::cout << handTotal << " ";
    for (int upCardIdx = 0; upCardIdx < NUM_CARD_VALUES; upCardIdx++) {
      double ev = 0;
      for (int result = static_cast<int>(DealerResults::_17); result <= static_cast<int>(DealerResults::_21); result++) {
        int dealerTotal = result + 17;
        if (dealerTotal > handTotal) {
          ev -= dealerOdds_[upCardIdx][result];
        } else if (dealerTotal < handTotal) {
          ev += dealerOdds_[upCardIdx][result];
        }
      }
      ev -= dealerOdds_[upCardIdx][static_cast<int>(DealerResults::_BJ)];
      ev += dealerOdds_[upCardIdx][static_cast<int>(DealerResults::_BUST)];
      hardTotalStandEv_[handTotal][upCardIdx] = ev;
      std::cout << ev << " ";
    }
    std::cout << std::endl;
  }

  // hit ev
  const auto& deckProbabilities = deck_.probabilities();
  hardTotalHitEv_.resize(HAND_VALUE_MAX + 1);
  hardTotalHitEv_[HAND_VALUE_MAX].assign(NUM_CARD_VALUES, -1.0);
  Hand hand;
  for (int handTotal = HAND_VALUE_MAX - 1; handTotal >= 0; handTotal--) {
    hardTotalHitEv_[handTotal].resize(NUM_CARD_VALUES);
    std::cout << handTotal << " ";
    for (int upCardIdx = 0; upCardIdx < NUM_CARD_VALUES; upCardIdx++) {
      double ev = 0;
      for (int i = 0; i < NUM_CARD_VALUES; i++) {
        hand.reset();
        hand.total = handTotal;
        hand.addCard(Card(CARD_VALUES[i], 'S'));
        if (hand.total > HAND_VALUE_MAX) {
          ev -= deckProbabilities[i];
        } else {
          ev += deckProbabilities[i] * std::max(hardTotalStandEv_[hand.total][upCardIdx], hardTotalHitEv_[hand.total][upCardIdx]);
        }
      }
      hardTotalHitEv_[handTotal][upCardIdx] = ev;
      std::cout << ev << " ";
    }
    std::cout << std::endl;
  }

    for (int handTotal = 0; handTotal < HAND_VALUE_MAX; handTotal++) {
      std::cout << handTotal << " ";
      for (int upCardIdx = 0; upCardIdx < NUM_CARD_VALUES; upCardIdx++) {
        if (hardTotalHitEv_[handTotal][upCardIdx] > hardTotalStandEv_[handTotal][upCardIdx]) {
          std::cout << "H ";
        } else if (hardTotalHitEv_[handTotal][upCardIdx] < hardTotalStandEv_[handTotal][upCardIdx]) {
          std::cout << "S ";
        } else {
          std::cout << "T ";
        }
      }
      std::cout << std::endl;
    }
  
}

}