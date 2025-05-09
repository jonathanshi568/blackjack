#include "Deck.hpp"

namespace jshi_blackjack {

class Sim
{
public:
  void start();

private:
  void calculateDealerProbabilities();
  void calculateBasicStrategy();

  void printDealerProbabilities();
  void printBasicStrategy();

  InfiniteDeck deck_;

  /*
  first index is index of CARD_VALUE
  second index is for DealerResults
  */
  std::vector<std::vector<double>> dealerOdds_;
  std::vector<std::vector<double>> dealerOddsAfterCheckingBlackjack_;

  /*
  first index is current total
  second index is dealer upcard
  */
  std::vector<std::vector<double>> standEv_;
  std::vector<std::vector<double>> hardTotalDoubleEv_;
  std::vector<std::vector<double>> softTotalDoubleEv_;
  std::vector<std::vector<double>> hardTotalHitEv_;
  std::vector<std::vector<double>> softTotalHitEv_;
  std::vector<std::vector<double>> splitEvNoDAS_;
  std::vector<std::vector<double>> splitEvDAS_;
};
}