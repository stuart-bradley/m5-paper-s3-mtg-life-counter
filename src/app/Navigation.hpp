#pragma once

#include <M5GFX.h>

class App;
class Screen;

class Navigation {
   public:
    static Navigation& instance();

    // App launching
    void launchApp(App* app);
    void launchApp(const char* appId);
    void exitApp();
    void goHome();

    // Screen stack within current app
    void pushScreen(Screen* screen);
    void popScreen();

    // Main loop
    void update();
    void draw(M5GFX* gfx);
    bool handleTouch(int16_t x, int16_t y, bool pressed, bool released);

    // Accessors
    App* currentApp() const { return _currentApp; }
    Screen* currentScreen() const;

    // State persistence
    void saveState();
    void restoreState();

   private:
    Navigation() = default;

    App* _currentApp = nullptr;
    static constexpr int MAX_DEPTH = 4;
    Screen* _screenStack[MAX_DEPTH] = {nullptr};
    int _stackDepth = 0;

    void clearStack();
};
