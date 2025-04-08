#include "BlackjackEngine.hpp"
#include "Sim.hpp"

using namespace jshi_blackjack;

int main() {
    // GameConfig* gameConfig = GameConfig::getInstance();
    // BlackjackEngine<MTDeck> bje;

    // std::string playerName{"shooba"};
    // auto interactivePlayer = std::make_shared<InteractivePlayer>(playerName, 10000);
    // bje.sitPlayer(interactivePlayer, 0);

    // while (true) {
    //     bje.startRound();
    // }
    Sim sim;
    sim.start();


    return 0;
}