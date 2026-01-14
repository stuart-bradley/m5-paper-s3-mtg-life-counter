#include "GameState.hpp"

static const char* NVS_NAMESPACE = "mtg";
static const char* KEY_PLAYER_COUNT = "playerCnt";
static const char* KEY_STARTING_LIFE = "startLife";
static const char* PLAYER_NAME_KEYS[] = {"p1name", "p2name", "p3name",
                                         "p4name", "p5name", "p6name"};
static const char* PLAYER_LIFE_KEYS[] = {"p1life", "p2life", "p3life",
                                         "p4life", "p5life", "p6life"};

static const char* DEFAULT_NAMES[] = {"Player 1", "Player 2", "Player 3",
                                      "Player 4", "Player 5", "Player 6"};

void GameState::initDefaults() {
    playerCount = DEFAULT_PLAYER_COUNT;
    startingLife = DEFAULT_STARTING_LIFE;
    for (uint8_t i = 0; i < MAX_PLAYERS; i++) {
        players[i].setName(DEFAULT_NAMES[i]);
        players[i].life = startingLife;
    }
}

void GameState::reset() {
    initDefaults();
}

void GameState::resetLifeTotals() {
    for (uint8_t i = 0; i < playerCount; i++) {
        players[i].reset(startingLife);
    }
}

bool GameState::load(Preferences& prefs) {
    if (!prefs.begin(NVS_NAMESPACE, true)) {
        initDefaults();
        return false;
    }

    playerCount = prefs.getUChar(KEY_PLAYER_COUNT, DEFAULT_PLAYER_COUNT);
    if (playerCount < 2)
        playerCount = 2;
    if (playerCount > MAX_PLAYERS)
        playerCount = MAX_PLAYERS;

    startingLife = prefs.getShort(KEY_STARTING_LIFE, DEFAULT_STARTING_LIFE);

    for (uint8_t i = 0; i < MAX_PLAYERS; i++) {
        String name = prefs.getString(PLAYER_NAME_KEYS[i], DEFAULT_NAMES[i]);
        players[i].setName(name.c_str());
        players[i].life = prefs.getShort(PLAYER_LIFE_KEYS[i], startingLife);
    }

    prefs.end();
    return true;
}

bool GameState::save(Preferences& prefs) {
    if (!prefs.begin(NVS_NAMESPACE, false)) {
        return false;
    }

    prefs.putUChar(KEY_PLAYER_COUNT, playerCount);
    prefs.putShort(KEY_STARTING_LIFE, startingLife);

    for (uint8_t i = 0; i < MAX_PLAYERS; i++) {
        prefs.putString(PLAYER_NAME_KEYS[i], players[i].name);
        prefs.putShort(PLAYER_LIFE_KEYS[i], players[i].life);
    }

    prefs.end();
    return true;
}
