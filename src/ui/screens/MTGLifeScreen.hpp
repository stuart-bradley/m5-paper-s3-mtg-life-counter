#pragma once

#include <Preferences.h>
#include "../../models/GameState.hpp"
#include "../HeaderBar.hpp"
#include "../Keyboard.hpp"
#include "../PlayerCard.hpp"
#include "../Screen.hpp"
#include "../Toolbar.hpp"

class ScreenManager;

class MTGLifeScreen : public Screen {
   public:
    MTGLifeScreen(ScreenManager* manager);

    ScreenId getScreenId() const override { return ScreenId::MTGLife; }

    void onEnter() override;
    void onExit() override;
    void update() override;
    void draw(M5GFX* gfx) override;
    bool handleTouch(int16_t x, int16_t y, bool pressed, bool released) override;

    void setSettingsScreen(Screen* screen);
    void setHomeScreen(Screen* screen);

   private:
    ScreenManager* _manager;
    Screen* _settingsScreen = nullptr;
    Screen* _homeScreen = nullptr;
    Toolbar _toolbar;
    HeaderBar _headerBar;
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
