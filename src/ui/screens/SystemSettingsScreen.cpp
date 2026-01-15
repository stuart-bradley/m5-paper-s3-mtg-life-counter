#include "SystemSettingsScreen.hpp"
#include "../../app/ScreenManager.hpp"

SystemSettingsScreen::SystemSettingsScreen(ScreenManager* manager) : _manager(manager) {
    _headerBar.setTitle("SYSTEM SETTINGS");
}

void SystemSettingsScreen::setHomeScreen(Screen* screen) {
    _homeScreen = screen;
    _headerBar.setLeftButton("< HOME", [this]() {
        if (_homeScreen) {
            _manager->setScreen(_homeScreen);
        }
    });
}

void SystemSettingsScreen::onEnter() {
    setNeedsFullRedraw(true);
}

void SystemSettingsScreen::draw(M5GFX* gfx) {
    bool needsDisplay = false;

    if (needsFullRedraw()) {
        gfx->fillScreen(TFT_WHITE);
        setNeedsFullRedraw(false);
        _toolbar.setDirty(true);
        needsDisplay = true;

        // Draw header bar
        _headerBar.draw(gfx);

        // Coming soon message
        gfx->setTextColor(TFT_BLACK);
        gfx->setTextDatum(MC_DATUM);
        gfx->setTextSize(2);
        gfx->drawString("Coming soon...", 480, 300);
    }

    // Update toolbar time
    _toolbar.update();

    // Draw toolbar only if dirty
    if (_toolbar.isDirty()) {
        _toolbar.draw(gfx);
        needsDisplay = true;
    }

    if (needsDisplay) {
        gfx->display();
    }
}

bool SystemSettingsScreen::handleTouch(int16_t x, int16_t y, bool pressed, bool released) {
    // Header bar handles the home button
    if (_headerBar.handleTouch(x, y, pressed, released)) {
        return true;
    }

    return false;
}
