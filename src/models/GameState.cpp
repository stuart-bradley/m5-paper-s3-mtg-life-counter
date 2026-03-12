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
    resetCommanderDamage();
}

void GameState::reset() {
    initDefaults();
}

void GameState::resetLifeTotals() {
    for (uint8_t i = 0; i < playerCount; i++) {
        players[i].reset(startingLife);
    }
    resetCommanderDamage();
}

void GameState::resetCommanderDamage() {
    for (uint8_t t = 0; t < MAX_PLAYERS; t++) {
        for (uint8_t s = 0; s < MAX_PLAYERS; s++) {
            commanderDamage[t][s] = 0;
        }
    }
}

void GameState::adjustCommanderDamage(uint8_t target, uint8_t source, int16_t delta) {
    if (target >= MAX_PLAYERS || source >= MAX_PLAYERS || target == source)
        return;
    int32_t val = static_cast<int32_t>(commanderDamage[target][source]) + delta;
    if (val < 0)
        val = 0;
    commanderDamage[target][source] = static_cast<int16_t>(val);
}

int16_t GameState::getCommanderDamage(uint8_t target, uint8_t source) const {
    if (target >= MAX_PLAYERS || source >= MAX_PLAYERS)
        return 0;
    return commanderDamage[target][source];
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

    // Load commander damage
    resetCommanderDamage();
    for (uint8_t t = 0; t < playerCount; t++) {
        for (uint8_t s = 0; s < playerCount; s++) {
            if (t == s)
                continue;
            char key[5];
            snprintf(key, sizeof(key), "cd%d%d", t, s);
            commanderDamage[t][s] = prefs.getShort(key, 0);
        }
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

    // Save commander damage for all slots to clear stale keys
    for (uint8_t t = 0; t < MAX_PLAYERS; t++) {
        for (uint8_t s = 0; s < MAX_PLAYERS; s++) {
            if (t == s)
                continue;
            char key[5];
            snprintf(key, sizeof(key), "cd%d%d", t, s);
            prefs.putShort(key, commanderDamage[t][s]);
        }
    }

    prefs.end();
    return true;
}
