#pragma once

#include <Preferences.h>
#include "../../models/GameState.hpp"
#include "../Button.hpp"
#include "../Screen.hpp"
#include "../Toolbar.hpp"

class ScreenManager;

class MTGSettingsScreen : public Screen {
   public:
    MTGSettingsScreen(ScreenManager* manager);
    ~MTGSettingsScreen();

    void onEnter() override;
    void onExit() override;
    void update() override;
    void draw(M5GFX* gfx) override;
    bool handleTouch(int16_t x, int16_t y, bool pressed, bool released) override;

   private:
    ScreenManager* _manager;
    Toolbar _toolbar;
    GameState _gameState;

    // UI Buttons
    Button* _backButton = nullptr;
    Button* _playerButtons[5] = {nullptr};  // 2,3,4,5,6 players
    Button* _lifeButtons[4] = {nullptr};    // 20,25,30,40 life
    Button* _resetLifeButton = nullptr;
    Button* _newGameButton = nullptr;

    // Confirm dialog state
    bool _showingConfirm = false;
    bool _confirmIsNewGame = false;
    Button* _confirmCancelButton = nullptr;
    Button* _confirmOkButton = nullptr;

    bool _dirty = true;

    void loadState();
    void saveState();
    void createButtons();
    void destroyButtons();
    void updatePlayerButtonStates();
    void updateLifeButtonStates();

    void onPlayerCountSelect(uint8_t count);
    void onStartingLifeSelect(int16_t life);
    void onResetLifeTapped();
    void onNewGameTapped();
    void showConfirmDialog(bool isNewGame);
    void hideConfirmDialog();
    void onConfirmAction();

    void drawSection(M5GFX* gfx, int16_t x, int16_t y, int16_t w, int16_t h, const char* title);
    void drawConfirmDialog(M5GFX* gfx);
};
