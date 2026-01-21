#pragma once

#include "../../ui/Button.hpp"
#include "../../ui/HeaderScreen.hpp"

class MTGApp;
class GameState;

class MTGSettingsScreen : public HeaderScreen {
   public:
    explicit MTGSettingsScreen(MTGApp* app);
    ~MTGSettingsScreen();

    const char* screenId() const override { return "settings"; }

    void onEnter() override;
    void onExit() override;

   protected:
    void onUpdate() override;
    void onHeaderFullRedraw(M5GFX* gfx) override;
    bool onTouch(int16_t x, int16_t y, bool pressed, bool released) override;

   private:
    MTGApp* _app;

    // UI Buttons
    Button* _playerButtons[5] = {nullptr};  // 2,3,4,5,6 players
    Button* _lifeButtons[4] = {nullptr};    // 20,25,30,40 life
    Button* _resetLifeButton = nullptr;
    Button* _newGameButton = nullptr;

    // Confirm dialog state
    bool _showingConfirm = false;
    bool _confirmIsNewGame = false;
    Button* _confirmCancelButton = nullptr;
    Button* _confirmOkButton = nullptr;

    GameState& gameState();  // Helper to access via App

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
