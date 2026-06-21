#pragma once

#include "../../ui/HeaderScreen.hpp"
#include "../../utils/Rect.hpp"

class MTGApp;
class GameState;

class MTGSettingsScreen : public HeaderScreen {
   public:
    explicit MTGSettingsScreen(MTGApp* app);

    void onEnter() override;
    void onExit() override;

   protected:
    void onHeaderFullRedraw(M5GFX* gfx) override;
    bool onTouch(int16_t x, int16_t y, bool pressed, bool released) override;

   private:
    MTGApp* _app;

    // Tap targets (these are drawn manually, so plain rects suffice)
    Rect _playerButtons[5];  // 2,3,4,5,6 players
    Rect _lifeButtons[4];    // 20,25,30,40 life
    Rect _resetLifeButton;
    Rect _newGameButton;

    // Confirm dialog state
    bool _showingConfirm = false;
    bool _confirmIsNewGame = false;
    Rect _confirmCancelButton;  // positioned in drawConfirmDialog()
    Rect _confirmOkButton;

    GameState& gameState();  // Helper to access via App

    void layoutButtons();

    void onPlayerCountSelect(uint8_t count);
    void onStartingLifeSelect(int16_t life);
    void onResetLifeTapped();
    void onNewGameTapped();
    void showConfirmDialog(bool isNewGame);
    void hideConfirmDialog();
    void onConfirmAction();

    void drawSection(M5GFX* gfx, int16_t x, int16_t y, int16_t w, int16_t h, const char* title);
    void drawSelectButton(M5GFX* gfx, const Rect& r, const char* label, bool selected);
    void drawConfirmDialog(M5GFX* gfx);
};
