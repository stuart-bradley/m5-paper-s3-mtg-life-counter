#pragma once

#include <functional>
#include "Component.hpp"
#include "../models/Player.hpp"

class PlayerCard : public Component {
public:
    using NameTapCallback = std::function<void()>;

    PlayerCard(Player* player, NameTapCallback onNameTap = nullptr);

    void draw(M5GFX* gfx) override;
    bool handleTouch(int16_t x, int16_t y, bool pressed, bool released) override;

    void setPlayer(Player* player);
    Player* getPlayer() { return _player; }

private:
    static constexpr int16_t BUTTON_HEIGHT = 36;
    static constexpr int16_t BUTTON_WIDTH = 60;
    static constexpr int16_t NAME_HEIGHT = 28;
    static constexpr uint32_t DEBOUNCE_MS = 100;

    Player* _player;
    NameTapCallback _onNameTap;
    int16_t _lastLife = 0;
    uint32_t _lastTouchTime = 0;

    Rect getNameRect() const;
    Rect getLifeRect() const;
    Rect getButtonRect(int index) const;  // 0=-5, 1=-1, 2=+1, 3=+5

    void drawButton(M5GFX* gfx, Rect r, const char* label);
};
