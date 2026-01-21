#pragma once

#include "../../ui/HeaderScreen.hpp"
#include "../../ui/Keyboard.hpp"
#include "../../ui/PlayerCard.hpp"

class MTGApp;
class GameState;

class MTGLifeScreen : public HeaderScreen {
   public:
    explicit MTGLifeScreen(MTGApp* app);

    void onEnter() override;
    void onExit() override;

   protected:
    void onUpdate() override;
    void onHeaderFullRedraw(M5GFX* gfx) override;
    bool onDraw(M5GFX* gfx) override;
    bool onTouch(int16_t x, int16_t y, bool pressed, bool released) override;

   private:
    MTGApp* _app;
    PlayerCard* _playerCards[6] = {nullptr};  // MAX_PLAYERS = 6
    Keyboard* _keyboard = nullptr;
    int8_t _editingPlayerIndex = -1;

    uint32_t _lastSaveTime = 0;
    static constexpr uint32_t SAVE_INTERVAL_MS = 5000;

    GameState& gameState();  // Helper to access via App

    void createPlayerCards();
    void destroyPlayerCards();
    void layoutPlayerCards();
    Rect getPlayerCardRect(int index, int playerCount) const;

    void showKeyboard(int playerIndex);
    void hideKeyboard(bool confirmed);
};
