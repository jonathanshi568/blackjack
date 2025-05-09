#include "Sim.hpp"

namespace jshi_blackjack {

void Sim::start()
{
  calculateDealerProbabilities();
  calculateBasicStrategy();
  printDealerProbabilities();
  printBasicStrategy();
}

void Sim::calculateDealerProbabilities()
{
  dealerOdds_.resize(NUM_CARD_VALUES);
  dealerOddsAfterCheckingBlackjack_.resize(NUM_CARD_VALUES);
  for (int i = 0; i < NUM_CARD_VALUES; i++) {
    dealerOdds_[i].resize(static_cast<int>(DealerResults::_DEALER_RESULTS_MAX));
    dealerOddsAfterCheckingBlackjack_[i].resize(static_cast<int>(DealerResults::_DEALER_RESULTS_MAX) - 1);
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

    double bjProbability = dealerOdds_[upCardIdx][static_cast<int>(DealerResults::_BJ)];
    for (int i = 0; i <= static_cast<int>(DealerResults::_BUST); i++) {
      dealerOddsAfterCheckingBlackjack_[upCardIdx][i] = dealerOdds_[upCardIdx][i] / (1 - bjProbability);
    }
  };
  
  Hand hand;
  for (int i = 0; i < NUM_CARD_VALUES; i++) {
    hand.reset();
    hand.addCard(Card(CARD_VALUES[i], 'S'));
    helper(i, 1.0, hand, helper);
  }
}

void Sim::calculateBasicStrategy()
{
  // stand ev
  Hand hand;
  standEv_.resize(HAND_VALUE_MAX + 1);
  for (int handTotal = 0; handTotal <= HAND_VALUE_MAX; handTotal++) {
    standEv_[handTotal].resize(NUM_CARD_VALUES);
    for (int upCardIdx = 0; upCardIdx < NUM_CARD_VALUES; upCardIdx++) {
      double ev = 0;
      for (int result = static_cast<int>(DealerResults::_17); result <= static_cast<int>(DealerResults::_21); result++) {
        int dealerTotal = result + 17;
        if (dealerTotal > handTotal) {
          ev -= dealerOddsAfterCheckingBlackjack_[upCardIdx][result];
        } else if (dealerTotal < handTotal) {
          ev += dealerOddsAfterCheckingBlackjack_[upCardIdx][result];
        }
      }
      ev -= dealerOddsAfterCheckingBlackjack_[upCardIdx][static_cast<int>(DealerResults::_BJ)];
      ev += dealerOddsAfterCheckingBlackjack_[upCardIdx][static_cast<int>(DealerResults::_BUST)];
      standEv_[handTotal][upCardIdx] = ev;
    }
  }

  // hit ev
  hardTotalHitEv_.resize(HAND_VALUE_MAX + 1);
  softTotalHitEv_.resize(HAND_VALUE_MAX + 1);
  hardTotalHitEv_[HAND_VALUE_MAX].assign(NUM_CARD_VALUES, -1.0);
  const auto& deckProbabilities = deck_.probabilities();

  auto calculateHitEv = [&] (int handTotal) {
    hardTotalHitEv_[handTotal].resize(NUM_CARD_VALUES);
    for (int upCardIdx = 0; upCardIdx < NUM_CARD_VALUES; upCardIdx++) {
      double ev = 0;
      for (int i = 0; i < NUM_CARD_VALUES; i++) {
        hand.reset();
        hand.total = handTotal;
        hand.addCard(Card(CARD_VALUES[i], 'S'));
        if (hand.total > HAND_VALUE_MAX) {
          ev -= deckProbabilities[i];
        } else {
          ev += deckProbabilities[i] * std::max(standEv_[hand.total][upCardIdx], hardTotalHitEv_[hand.total][upCardIdx]);
        }
      }
      hardTotalHitEv_[handTotal][upCardIdx] = ev;
    }
  };

  // hard totals 11-21
  for (int handTotal = HAND_VALUE_MAX - 1; handTotal >= 11; handTotal--) {
    calculateHitEv(handTotal);
  }

  // soft totals hit ev
  softTotalHitEv_[HAND_VALUE_MAX].resize(NUM_CARD_VALUES);
  for (int upCardIdx = 0; upCardIdx < NUM_CARD_VALUES; upCardIdx++) {
    softTotalHitEv_[HAND_VALUE_MAX][upCardIdx] = standEv_[HAND_VALUE_MAX][upCardIdx];
  }

  for (int softTotal = HAND_VALUE_MAX - 1; softTotal >= 12; softTotal--) {
    softTotalHitEv_[softTotal].resize(NUM_CARD_VALUES);
    for (int upCardIdx = 0; upCardIdx < NUM_CARD_VALUES; upCardIdx++) {
      double ev = 0;
      for (int i = 0; i < NUM_CARD_VALUES; i++) {
        hand.reset();
        hand.soft = true;
        hand.total = softTotal;
        hand.addCard(Card(CARD_VALUES[i], 'S'));
        if (hand.soft) {
          ev += deckProbabilities[i] * std::max(standEv_[hand.total][upCardIdx], softTotalHitEv_[hand.total][upCardIdx]);
        } else {
          ev += deckProbabilities[i] * std::max(standEv_[hand.total][upCardIdx], hardTotalHitEv_[hand.total][upCardIdx]);
        }
      }
      softTotalHitEv_[softTotal][upCardIdx] = ev;
    }
  }

  // hard totals 0-10
  for (int handTotal = 10; handTotal >= 0; handTotal--) {
    calculateHitEv(handTotal);
  }

  // hard total double totals
  hardTotalDoubleEv_.resize(HAND_VALUE_MAX + 1);
  for (int handTotal = 0; handTotal <= HAND_VALUE_MAX; handTotal++) {
    hardTotalDoubleEv_[handTotal].resize(NUM_CARD_VALUES);
    for (int upCardIdx = 0; upCardIdx < NUM_CARD_VALUES; upCardIdx++) {
      double ev = 0;
      for (int i = 0; i < NUM_CARD_VALUES; i++) {
        hand.reset();
        hand.total = handTotal;
        hand.addCard(Card(CARD_VALUES[i], 'S'));
        if (hand.total > HAND_VALUE_MAX) {
          ev -= deckProbabilities[i];
        } else {
          for (int result = static_cast<int>(DealerResults::_17); result <= static_cast<int>(DealerResults::_21); result++) {
            int dealerTotal = result + 17;
            if (dealerTotal > hand.total) {
              ev -= deckProbabilities[i] * dealerOddsAfterCheckingBlackjack_[upCardIdx][result];
            } else if (hand.total > dealerTotal) {
              ev += deckProbabilities[i] * dealerOddsAfterCheckingBlackjack_[upCardIdx][result];
            }
          }
          ev -= deckProbabilities[i] * dealerOddsAfterCheckingBlackjack_[upCardIdx][static_cast<int>(DealerResults::_BJ)]; // should be 0
          ev += deckProbabilities[i] * dealerOddsAfterCheckingBlackjack_[upCardIdx][static_cast<int>(DealerResults::_BUST)];
        }
      }
      hardTotalDoubleEv_[handTotal][upCardIdx] = ev * 2;
    }
  }

  // soft total double totals
  softTotalDoubleEv_.resize(HAND_VALUE_MAX + 1);
  for (int handTotal = 12; handTotal <= HAND_VALUE_MAX - 1; handTotal++) {
    softTotalDoubleEv_[handTotal].resize(NUM_CARD_VALUES);
    for (int upCardIdx = 0; upCardIdx < NUM_CARD_VALUES; upCardIdx++) {
      double ev = 0;
      for (int i = 0; i < NUM_CARD_VALUES; i++) {
        hand.reset();
        hand.total = handTotal;
        hand.soft = true;
        hand.addCard(Card(CARD_VALUES[i], 'S'));
        if (hand.total > HAND_VALUE_MAX) {
          ev -= deckProbabilities[i];
        } else {
          for (int result = static_cast<int>(DealerResults::_17); result <= static_cast<int>(DealerResults::_21); result++) {
            int dealerTotal = result + 17;
            if (dealerTotal > hand.total) {
              ev -= deckProbabilities[i] * dealerOddsAfterCheckingBlackjack_[upCardIdx][result];
            } else if (hand.total > dealerTotal) {
              ev += deckProbabilities[i] * dealerOddsAfterCheckingBlackjack_[upCardIdx][result];
            }
          }
          ev -= deckProbabilities[i] * dealerOddsAfterCheckingBlackjack_[upCardIdx][static_cast<int>(DealerResults::_BJ)]; // should be 0
          ev += deckProbabilities[i] * dealerOddsAfterCheckingBlackjack_[upCardIdx][static_cast<int>(DealerResults::_BUST)];
        }
      }
      softTotalDoubleEv_[handTotal][upCardIdx] = ev * 2;
    }
  }

  // split ev
  splitEvDAS_.resize(NUM_CARD_VALUES);
  splitEvNoDAS_.resize(NUM_CARD_VALUES);
  auto splitHelper = [&] (int myCardIdx, int dealerCardIdx, int& numSplitHands, double& evDAS, double& evNoDAS, double prob, auto&& splitHelper) -> void {
    Hand splitHand;
    for (int i = 0; i < NUM_CARD_VALUES; i++) {
      splitHand.reset();
      splitHand.addCard(Card(CARD_VALUES[myCardIdx], 'S'));
      splitHand.addCard(Card(CARD_VALUES[i], 'S'));
      if (i == myCardIdx && numSplitHands < GameConfig::getInstance()->maxSplitHands) {
        numSplitHands++;
        splitHelper(myCardIdx, dealerCardIdx, numSplitHands, evDAS, evNoDAS, deckProbabilities[i] * prob, splitHelper);
        splitHelper(myCardIdx, dealerCardIdx, numSplitHands, evDAS, evNoDAS, deckProbabilities[i] * prob, splitHelper);
        numSplitHands--;
      } else {
        double maxEvNoDAS = std::max(standEv_[splitHand.total][dealerCardIdx], hardTotalHitEv_[splitHand.total][dealerCardIdx]);
        double maxEvDAS = GameConfig::getInstance()->das ? std::max(maxEvNoDAS, hardTotalDoubleEv_[splitHand.total][dealerCardIdx]) : maxEvNoDAS;
        evDAS += maxEvDAS * deckProbabilities[i] * prob;
        evNoDAS += maxEvNoDAS * deckProbabilities[i] * prob;
      }
    }
  };

  for (int i = 0; i < NUM_CARD_VALUES; i++) {
    splitEvDAS_[i].resize(NUM_CARD_VALUES);
    splitEvNoDAS_[i].resize(NUM_CARD_VALUES);
    for (int upCardIdx = 0; upCardIdx < NUM_CARD_VALUES; upCardIdx++) {
      int numSplitHands = 1;
      double evDAS = 0;
      double evNoDAS = 0;
      splitHelper(i, upCardIdx, numSplitHands, evDAS, evNoDAS, 1.0, splitHelper);
      splitHelper(i, upCardIdx, numSplitHands, evDAS, evNoDAS, 1.0, splitHelper);
      splitEvDAS_[i][upCardIdx] = evDAS;
      splitEvNoDAS_[i][upCardIdx] = evNoDAS;
    }
  }
}

void Sim::printDealerProbabilities()
{
  std::cout << "Dealer probabilities:" << std::endl;
  for (int i = 0; i < NUM_CARD_VALUES; i++) {
    std::cout << i << " ";
    for (int j = 0; j < static_cast<int>(DealerResults::_DEALER_RESULTS_MAX); j++) {
      std::cout << dealerOdds_[i][j] << " ";
    }
    std::cout << std::endl;
  }

  std::cout << "Dealer probabilities after checking blackjack:" << std::endl;
  for (int i = 0; i < NUM_CARD_VALUES; i++) {
    std::cout << i << " ";
    for (int j = 0; j < static_cast<int>(DealerResults::_DEALER_RESULTS_MAX) - 1; j++) {
      std::cout << dealerOddsAfterCheckingBlackjack_[i][j] << " ";
    }
    std::cout << std::endl;
  }
}

void Sim::printBasicStrategy()
{
  // print stand ev
  std::cout << "Stand evs:" << std::endl;
  for (int i = 0; i <= HAND_VALUE_MAX; i++) {
    std::cout << i << " ";
    for (int upCardIdx = 0; upCardIdx < NUM_CARD_VALUES; upCardIdx++) {
      std::cout << standEv_[i][upCardIdx] << " ";
    }
    std::cout << std::endl;
  }

  // print hard total hit ev
  std::cout << "Hard total hit evs:" << std::endl;
  for (int i = 0; i <= HAND_VALUE_MAX; i++) {
    std::cout << i << " ";
    for (int upCardIdx = 0; upCardIdx < NUM_CARD_VALUES; upCardIdx++) {
      std::cout << hardTotalHitEv_[i][upCardIdx] << " ";
    }
    std::cout << std::endl;
  }

  // print soft total hit ev
  std::cout << "Soft total hit evs:" << std::endl;
  for (int i = 12; i <= 20; i++) {
    std::cout << i << " ";
    for (int upCardIdx = 0; upCardIdx < NUM_CARD_VALUES; upCardIdx++) {
      std::cout << softTotalHitEv_[i][upCardIdx] << " ";
    }
    std::cout << std::endl;
  }

  std::cout << "Hard total double evs:" << std::endl;
  for (int i = 0; i <= HAND_VALUE_MAX; i++) {
    std::cout << i << " ";
    for (int upCardIdx = 0; upCardIdx < NUM_CARD_VALUES; upCardIdx++) {
      std::cout << hardTotalDoubleEv_[i][upCardIdx] << " ";
    }
    std::cout << std::endl;
  }

  std::cout << "Soft total double evs:" << std::endl;
  for (int i = 12; i <= HAND_VALUE_MAX - 1; i++) {
    std::cout << i << " ";
    for (int upCardIdx = 0; upCardIdx < NUM_CARD_VALUES; upCardIdx++) {
      std::cout << softTotalDoubleEv_[i][upCardIdx] << " ";
    }
    std::cout << std::endl;
  }

  std::cout << "Split evs (DAS):" << std::endl;
  for (int i = 0; i < NUM_CARD_VALUES; i++) {
    std::cout << CARD_VALUES[i] << " ";
    for (int upCardIdx = 0; upCardIdx < NUM_CARD_VALUES; upCardIdx++) {
      std::cout << splitEvDAS_[i][upCardIdx] << " ";
    }
    std::cout << std::endl;
  }
  std::cout << "Split evs (no DAS):" << std::endl;
  for (int i = 0; i < NUM_CARD_VALUES; i++) {
    std::cout << CARD_VALUES[i] << " ";
    for (int upCardIdx = 0; upCardIdx < NUM_CARD_VALUES; upCardIdx++) {
      std::cout << splitEvNoDAS_[i][upCardIdx] << " ";
    }
    std::cout << std::endl;
  }

  // print hard total basic strategy chart
  std::cout << "Hard total basic strategy chart:" << std::endl;
  for (int handTotal = 0; handTotal < HAND_VALUE_MAX; handTotal++) {
    std::cout << handTotal << " ";
    for (int upCardIdx = 0; upCardIdx < NUM_CARD_VALUES; upCardIdx++) {
      if (hardTotalHitEv_[handTotal][upCardIdx] >= standEv_[handTotal][upCardIdx]) {
        if (hardTotalDoubleEv_[handTotal][upCardIdx] >= hardTotalHitEv_[handTotal][upCardIdx]) {
          std::cout << "D/H ";
        } else {
          std::cout << "H ";
        }
      } else if (hardTotalHitEv_[handTotal][upCardIdx] < standEv_[handTotal][upCardIdx]) {
        if (hardTotalDoubleEv_[handTotal][upCardIdx] >= standEv_[handTotal][upCardIdx]) {
          std::cout << "D/S ";
        } else {
          std::cout << "S ";
        }
      }
    }
    std::cout << std::endl;
  }

  // print soft total basic strategy chart
  std::cout << "Soft total basic strategy chart:" << std::endl;
  for (int softTotal = 12; softTotal < HAND_VALUE_MAX; softTotal++) {
    std::cout << softTotal << " ";
    for (int upCardIdx = 0; upCardIdx < NUM_CARD_VALUES; upCardIdx++) {
      if (softTotalHitEv_[softTotal][upCardIdx] > standEv_[softTotal][upCardIdx]) {
        if (softTotalDoubleEv_[softTotal][upCardIdx] >= softTotalHitEv_[softTotal][upCardIdx]) {
          std::cout << "D/H ";
        } else {
          std::cout << "H ";
        }
      } else if (softTotalHitEv_[softTotal][upCardIdx] < standEv_[softTotal][upCardIdx]) {
        if (softTotalDoubleEv_[softTotal][upCardIdx] >= standEv_[softTotal][upCardIdx]) {
          std::cout << "D/S ";
        } else {
          std::cout << "S ";
        }
      }
    }
    std::cout << std::endl;
  }
}

}