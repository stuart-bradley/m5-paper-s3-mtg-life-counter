#include "Navigation.hpp"
#include <Preferences.h>
#include <cstring>
#include "../ui/Screen.hpp"
#include "../utils/Log.hpp"
#include "App.hpp"
#include "AppRegistry.hpp"

static constexpr const char* PREF_NAMESPACE = "nav";
static constexpr const char* PREF_APP_ID = "appId";
static constexpr const char* PREF_SCREEN_ID = "screenId";

Navigation& Navigation::instance() {
    static Navigation nav;
    return nav;
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

    saveState();
}

void Navigation::launchApp(const char* appId) {
    App* app = AppRegistry::instance().findApp(appId);
    if (app) {
        launchApp(app);
    }
}

void Navigation::exitApp() {
    goHome();
}

void Navigation::goHome() {
    App* home = AppRegistry::instance().homeApp();
    if (home && home != _currentApp) {
        launchApp(home);
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

    saveState();
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
    saveState();
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

void Navigation::saveState() {
    Preferences prefs;
    prefs.begin(PREF_NAMESPACE, false);

    if (_currentApp) {
        prefs.putString(PREF_APP_ID, _currentApp->metadata().id);

        // Save screen identifier for restore using Screen::screenId()
        Screen* screen = currentScreen();
        if (screen) {
            prefs.putString(PREF_SCREEN_ID, screen->screenId());
        }
    }

    prefs.end();
}

void Navigation::restoreState() {
    Preferences prefs;
    prefs.begin(PREF_NAMESPACE, true);  // Read-only

    String appId = prefs.getString(PREF_APP_ID, "home");
    String screenId = prefs.getString(PREF_SCREEN_ID, "main");
    prefs.end();

    LOG_D("[Nav] Restoring: app='%s', screen='%s'", appId.c_str(), screenId.c_str());

    // Find and launch the app
    App* app = AppRegistry::instance().findApp(appId.c_str());
    if (!app) {
        app = AppRegistry::instance().homeApp();
    }

    if (app) {
        launchApp(app);

        // If screen is not "main", try to push the specific screen
        if (screenId != "main") {
            Screen* screen = app->getScreen(screenId.c_str());
            if (screen) {
                pushScreen(screen);
            }
        }
    }
}
