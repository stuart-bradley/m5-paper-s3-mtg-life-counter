#pragma once

#include "../Screen.hpp"
#include "../Toolbar.hpp"
#include "../PlayerCard.hpp"
#include "../Keyboard.hpp"
#include "../../models/GameState.hpp"
#include <Preferences.h>

class ScreenManager;

class MTGLifeScreen : public Screen {
public:
    MTGLifeScreen(ScreenManager* manager);

    void onEnter() override;
    void onExit() override;
    void update() override;
    void draw(M5GFX* gfx) override;
    bool handleTouch(int16_t x, int16_t y, bool pressed, bool released) override;

private:
    ScreenManager* _manager;
    Toolbar _toolbar;
    GameState _gameState;
    PlayerCard* _playerCards[GameState::MAX_PLAYERS] = {nullptr};
    Keyboard* _keyboard = nullptr;
    int8_t _editingPlayerIndex = -1;

    bool _dirty = true;
    uint32_t _lastSaveTime = 0;
    static constexpr uint32_t SAVE_INTERVAL_MS = 5000;

    void loadState();
    void saveState();
    void createPlayerCards();
    void destroyPlayerCards();
    void layoutPlayerCards();
    Rect getPlayerCardRect(int index, int playerCount) const;

    void showKeyboard(int playerIndex);
    void hideKeyboard(bool confirmed);
};
