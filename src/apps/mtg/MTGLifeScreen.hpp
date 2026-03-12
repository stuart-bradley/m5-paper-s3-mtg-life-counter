#pragma once

#include "../../models/GameState.hpp"
#include "../../ui/HeaderScreen.hpp"
#include "../../ui/Keyboard.hpp"
#include "../../ui/PlayerCard.hpp"

class MTGApp;

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
    PlayerCard* _playerCards[6] = {nullptr};
    Keyboard* _keyboard = nullptr;
    int8_t _editingPlayerIndex = -1;

    uint32_t _lastSaveTime = 0;
    static constexpr uint32_t SAVE_INTERVAL_MS = 5000;

    GameState& gameState();

    void createPlayerCards();
    void destroyPlayerCards();
    void layoutPlayerCards();
    Rect getPlayerCardRect(int index, int playerCount) const;

    void onModeToggle(bool cmdr);
    void configureCardMode();

    void showKeyboard(int playerIndex);
    void hideKeyboard(bool confirmed);
};
