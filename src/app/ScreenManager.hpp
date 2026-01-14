#pragma once

#include <M5GFX.h>
#include "../ui/Screen.hpp"

class ScreenManager {
   public:
    void setScreen(Screen* newScreen);
    void goBack();

    Screen* getCurrentScreen() { return _current; }
    Screen* getPreviousScreen() { return _previous; }

    void update();
    void draw(M5GFX* gfx);
    bool handleTouch(int16_t x, int16_t y, bool pressed, bool released);

   private:
    Screen* _current = nullptr;
    Screen* _previous = nullptr;
};
