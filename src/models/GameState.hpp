#pragma once

#include <Preferences.h>
#include <cstdint>
#include "Player.hpp"

struct GameState {
    static constexpr uint8_t MAX_PLAYERS = 6;
    static constexpr uint8_t DEFAULT_PLAYER_COUNT = 2;
    static constexpr int16_t DEFAULT_STARTING_LIFE = 20;

    uint8_t playerCount = DEFAULT_PLAYER_COUNT;
    int16_t startingLife = DEFAULT_STARTING_LIFE;
    Player players[MAX_PLAYERS];

    void initDefaults();
    void reset();
    void resetLifeTotals();
    bool load(Preferences& prefs);
    bool save(Preferences& prefs);
};
