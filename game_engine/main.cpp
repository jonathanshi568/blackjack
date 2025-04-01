#include "BlackjackEngine.hpp"

using namespace jshi_blackjack;

int main() {
    GameConfig* gameConfig = GameConfig::getInstance();
    // tweak gameConfig
    BlackjackEngine<MTShuffler> bje;

    std::string playerName{"shooba"};
    auto interactivePlayer = std::make_shared<InteractivePlayer>(playerName, 10000);
    bje.sitPlayer(interactivePlayer, 0);

    while (true) {
        bje.startRound();
    }

    return 0;
}