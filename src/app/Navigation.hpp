#pragma once

#include <M5GFX.h>

class App;
class Screen;

class Navigation {
   public:
    static Navigation& instance();

    // App registration (called once at startup). The non-launcher app is the home app.
    void registerApp(App* app);
    int launchableAppCount() const;
    App* getLaunchableApp(int index);

    // App launching
    void launchApp(App* app);
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

   private:
    Navigation() = default;

    static constexpr int MAX_APPS = 8;
    App* _apps[MAX_APPS] = {nullptr};
    int _appCount = 0;
    App* _homeApp = nullptr;

    App* _currentApp = nullptr;
    static constexpr int MAX_DEPTH = 4;
    Screen* _screenStack[MAX_DEPTH] = {nullptr};
    int _stackDepth = 0;

    void clearStack();
};
