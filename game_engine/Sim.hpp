#include "Deck.hpp"

namespace jshi_blackjack {

class Sim
{
public:
  void start();

private:
void calculateDealerProbabilities();
void calculateBasicStrategy();

  InfiniteDeck deck_;

  /*
  first index is index of CARD_VALUE
  second index is for DealerResults
  */
  std::vector<std::vector<double>> dealerOdds_;

  /*
  first index is current total
  second index is dealer upcard
  */
  std::vector<std::vector<double>> hardTotalStandEv_;
  std::vector<std::vector<double>> hardTotalHitEv_;

};
}