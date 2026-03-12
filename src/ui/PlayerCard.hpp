#pragma once

#include <functional>
#include "../models/Player.hpp"
#include "Component.hpp"

enum class CardMode : uint8_t { LIFE, CMDR };

struct OpponentInfo {
    const char* name;  // opponent name (not owned)
    int16_t* damage;   // pointer into commanderDamage matrix
    uint8_t sourceIndex;
};

class PlayerCard : public Component {
   public:
    using NameTapCallback = std::function<void()>;
    using CmdrDamageCallback = std::function<void(uint8_t target, uint8_t source, int16_t delta)>;

    PlayerCard(Player* player, NameTapCallback onNameTap = nullptr);

    void draw(M5GFX* gfx) override;
    bool handleTouch(int16_t x, int16_t y, bool pressed, bool released) override;

    void setPlayer(Player* player);
    Player* getPlayer() { return _player; }
    void checkDeltaExpiry();

    void setMode(CardMode mode);
    // Must be called after createPlayerCards() and after load().
    void setOpponents(OpponentInfo* opponents, uint8_t count, uint8_t playerIndex,
                      CmdrDamageCallback callback);

   private:
    // Horizontal layout (wider cards)
    static constexpr int16_t BUTTON_HEIGHT = 48;
    static constexpr int16_t BUTTON_WIDTH = 80;
    static constexpr int16_t BUTTON_MARGIN = 12;
    // Stacked layout (narrow cards) - same size buttons, just arranged differently
    static constexpr int16_t STACK_BUTTON_HEIGHT = 44;
    static constexpr int16_t STACK_BUTTON_WIDTH = 70;
    // Threshold for switching layouts
    static constexpr int16_t NARROW_THRESHOLD = 350;

    static constexpr int16_t NAME_HEIGHT = 56;
    static constexpr int16_t CMDR_NAME_HEIGHT = 28;
    static constexpr int16_t CMDR_BTN_W = 50;
    static constexpr int16_t CMDR_BTN_H = 32;
    static constexpr int16_t CMDR_BTN_GAP = 6;
    static constexpr int16_t LETHAL_DAMAGE = 21;
    static constexpr uint8_t MAX_OPPONENTS = 5;
    static constexpr uint32_t DEBOUNCE_MS = 100;
    static constexpr uint32_t DELTA_DISPLAY_MS = 5000;

    Player* _player;
    NameTapCallback _onNameTap;
    int16_t _lastLife = 0;
    uint32_t _lastTouchTime = 0;
    int16_t _pendingDelta = 0;
    uint32_t _lastDeltaTime = 0;

    CardMode _mode = CardMode::LIFE;
    OpponentInfo _opponents[MAX_OPPONENTS];
    uint8_t _opponentCount = 0;
    uint8_t _playerIndex = 0;
    CmdrDamageCallback _cmdrCallback;

    bool useStackedLayout() const { return _bounds.w < NARROW_THRESHOLD; }

    Rect getNameRect() const;
    Rect getLifeRect() const;
    Rect getButtonRect(int index) const;  // 0=-5, 1=-1, 2=+1, 3=+5

    void drawLifeMode(M5GFX* gfx);
    void drawCmdrMode(M5GFX* gfx);
    bool handleLifeTouch(int16_t x, int16_t y, uint32_t now);
    bool handleCmdrTouch(int16_t x, int16_t y, uint32_t now);
    void drawButton(M5GFX* gfx, Rect r, const char* label);
    void getCmdrRowLayout(uint8_t row, int16_t& ry, int16_t& rowH, Rect& minusBtn,
                          Rect& plusBtn) const;
};
