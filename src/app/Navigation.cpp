#include "Navigation.hpp"
#include "../ui/Screen.hpp"
#include "../utils/Log.hpp"
#include "App.hpp"

Navigation& Navigation::instance() {
    static Navigation nav;
    return nav;
}

void Navigation::registerApp(App* app) {
    if (!app) {
        LOG_W("Navigation: Attempted to register null app");
        return;
    }
    if (_appCount >= MAX_APPS) {
        LOG_E("Navigation: Cannot register app '%s' - MAX_APPS (%d) exceeded",
              app->metadata().id, MAX_APPS);
        return;
    }
    _apps[_appCount++] = app;
    if (!app->metadata().showInLauncher) {
        _homeApp = app;
    }
}

int Navigation::launchableAppCount() const {
    int count = 0;
    for (int i = 0; i < _appCount; i++) {
        if (_apps[i]->metadata().showInLauncher) {
            count++;
        }
    }
    return count;
}

App* Navigation::getLaunchableApp(int index) {
    int seen = 0;
    for (int i = 0; i < _appCount; i++) {
        if (_apps[i]->metadata().showInLauncher) {
            if (seen == index) {
                return _apps[i];
            }
            seen++;
        }
    }
    return nullptr;
}

Screen* Navigation::currentScreen() const {
    if (_stackDepth > 0) {
        return _screenStack[_stackDepth - 1];
    }
    return nullptr;
}

void Navigation::clearStack() {
    // Call onExit for each stacked screen (top to bottom)
    for (int i = _stackDepth - 1; i >= 0; i--) {
        if (_screenStack[i]) {
            _screenStack[i]->onExit();
            _screenStack[i] = nullptr;
        }
    }
    _stackDepth = 0;
}

void Navigation::launchApp(App* app) {
    if (!app)
        return;

    // Suspend current app if exists
    if (_currentApp) {
        _currentApp->onSuspend();
    }

    // Clear the screen stack
    clearStack();

    // Set new app
    _currentApp = app;
    _currentApp->onLaunch();

    // Push the app's main screen
    Screen* mainScreen = _currentApp->getMainScreen();
    if (mainScreen) {
        _screenStack[0] = mainScreen;
        _stackDepth = 1;
        mainScreen->onEnter();
        mainScreen->setNeedsFullRedraw(true);
    }
}

void Navigation::exitApp() {
    goHome();
}

void Navigation::goHome() {
    if (_homeApp && _homeApp != _currentApp) {
        launchApp(_homeApp);
    }
}

void Navigation::pushScreen(Screen* screen) {
    if (!screen || _stackDepth >= MAX_DEPTH)
        return;

    // Exit current screen
    Screen* current = currentScreen();
    if (current) {
        current->onExit();
    }

    // Push new screen
    _screenStack[_stackDepth++] = screen;
    screen->onEnter();
    screen->setNeedsFullRedraw(true);
}

void Navigation::popScreen() {
    // If on main screen (depth 1) or stack empty, exit app and go home
    if (_stackDepth <= 1) {
        goHome();
        return;
    }

    // Exit current screen
    Screen* current = currentScreen();
    if (current) {
        current->onExit();
        _screenStack[_stackDepth - 1] = nullptr;
    }
    _stackDepth--;

    // Re-enter previous screen
    Screen* prev = currentScreen();
    if (prev) {
        prev->onEnter();
        prev->setNeedsFullRedraw(true);
    }
}

void Navigation::update() {
    Screen* screen = currentScreen();
    if (screen) {
        screen->update();
    }
}

void Navigation::draw(M5GFX* gfx) {
    Screen* screen = currentScreen();
    if (screen) {
        screen->draw(gfx);
    }
}

bool Navigation::handleTouch(int16_t x, int16_t y, bool pressed, bool released) {
    Screen* screen = currentScreen();
    if (screen) {
        return screen->handleTouch(x, y, pressed, released);
    }
    return false;
}
