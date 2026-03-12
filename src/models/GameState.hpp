#pragma once

#include <Preferences.h>
#include <cstdint>
#include "Player.hpp"

enum class ViewMode : uint8_t { LIFE, CMDR };

struct GameState {
    static constexpr uint8_t MAX_PLAYERS = 6;
    static constexpr uint8_t DEFAULT_PLAYER_COUNT = 2;
    static constexpr int16_t DEFAULT_STARTING_LIFE = 20;

    uint8_t playerCount = DEFAULT_PLAYER_COUNT;
    int16_t startingLife = DEFAULT_STARTING_LIFE;
    Player players[MAX_PLAYERS];
    ViewMode viewMode = ViewMode::LIFE;
    int16_t commanderDamage[MAX_PLAYERS][MAX_PLAYERS] = {};  // [target][source]

    void initDefaults();
    void reset();
    void resetLifeTotals();
    void resetCommanderDamage();
    void adjustCommanderDamage(uint8_t target, uint8_t source, int16_t delta);
    int16_t getCommanderDamage(uint8_t target, uint8_t source) const;
    bool load(Preferences& prefs);
    bool save(Preferences& prefs);
};
