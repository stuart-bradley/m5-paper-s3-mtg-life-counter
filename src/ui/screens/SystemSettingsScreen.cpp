#include "SystemSettingsScreen.hpp"
#include "../../app/ScreenManager.hpp"
#include "../../utils/Sound.hpp"

static const char* SOUND_OPTIONS[] = {"OFF", "ON"};
static const char* SLEEP_OPTIONS[] = {"Off", "1m", "5m", "10m"};
static const uint16_t SLEEP_VALUES[] = {0, 60, 300, 600};

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

void SystemSettingsScreen::setWiFiScreen(Screen* screen) {
    _wifiScreen = screen;
}

void SystemSettingsScreen::loadSettings() {
    Preferences prefs;
    _settings.load(prefs);
    Sound::setEnabled(_settings.soundEnabled);
}

void SystemSettingsScreen::saveSettings() {
    Preferences prefs;
    _settings.save(prefs);
}

void SystemSettingsScreen::onEnter() {
    loadSettings();
    setNeedsFullRedraw(true);
}

void SystemSettingsScreen::onExit() {
    saveSettings();
}

int SystemSettingsScreen::getSoundIndex() const {
    return _settings.soundEnabled ? 1 : 0;
}

int SystemSettingsScreen::getSleepIndex() const {
    for (int i = 0; i < 4; i++) {
        if (_settings.sleepTimeoutSecs == SLEEP_VALUES[i]) {
            return i;
        }
    }
    return 2;  // Default to 5m if not found
}

Rect SystemSettingsScreen::getButtonRect(int row, int buttonIndex) const {
    int16_t y = Toolbar::HEIGHT + HeaderBar::HEIGHT + 40 + row * ROW_HEIGHT;
    int16_t x = BUTTONS_X + buttonIndex * (BUTTON_W + 10);
    return Rect(x, y, BUTTON_W, BUTTON_H);
}

void SystemSettingsScreen::drawRow(M5GFX* gfx, int16_t y, const char* label, const char* options[],
                                   int optionCount, int selectedIndex) {
    // Draw label
    gfx->setTextColor(TFT_BLACK);
    gfx->setTextDatum(ML_DATUM);
    gfx->setTextSize(2);
    gfx->drawString(label, LABEL_X, y + BUTTON_H / 2);

    // Draw option buttons
    for (int i = 0; i < optionCount; i++) {
        int16_t bx = BUTTONS_X + i * (BUTTON_W + 10);
        bool selected = (i == selectedIndex);

        if (selected) {
            gfx->fillRect(bx, y, BUTTON_W, BUTTON_H, TFT_BLACK);
            gfx->setTextColor(TFT_WHITE);
        } else {
            gfx->fillRect(bx, y, BUTTON_W, BUTTON_H, TFT_WHITE);
            gfx->drawRect(bx, y, BUTTON_W, BUTTON_H, TFT_BLACK);
            gfx->setTextColor(TFT_BLACK);
        }

        gfx->setTextDatum(MC_DATUM);
        gfx->setTextSize(1);
        gfx->drawString(options[i], bx + BUTTON_W / 2, y + BUTTON_H / 2);
    }
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

        // Draw setting rows
        int16_t startY = Toolbar::HEIGHT + HeaderBar::HEIGHT + 40;

        // Row 0: WiFi (navigation row)
        gfx->setTextColor(TFT_BLACK);
        gfx->setTextDatum(ML_DATUM);
        gfx->setTextSize(2);
        gfx->drawString("WiFi:", LABEL_X, startY + BUTTON_H / 2);
        // Draw navigate button
        int16_t wifiButtonX = BUTTONS_X;
        gfx->fillRect(wifiButtonX, startY, WIFI_BUTTON_W, BUTTON_H, TFT_WHITE);
        gfx->drawRect(wifiButtonX, startY, WIFI_BUTTON_W, BUTTON_H, TFT_BLACK);
        gfx->setTextDatum(MC_DATUM);
        gfx->setTextSize(1);
        gfx->drawString("Configure >", wifiButtonX + WIFI_BUTTON_W / 2, startY + BUTTON_H / 2);

        // Row 1: Sound
        drawRow(gfx, startY + ROW_HEIGHT, "Sound:", SOUND_OPTIONS, 2, getSoundIndex());

        // Row 2: Auto-Sleep
        drawRow(gfx, startY + ROW_HEIGHT * 2, "Auto-Sleep:", SLEEP_OPTIONS, 4, getSleepIndex());
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

    if (!released)
        return pressed;

    // Check WiFi button (row 0)
    int16_t startY = Toolbar::HEIGHT + HeaderBar::HEIGHT + 40;
    Rect wifiRect(BUTTONS_X, startY, WIFI_BUTTON_W, BUTTON_H);
    if (wifiRect.contains(x, y)) {
        Sound::click();
        if (_wifiScreen) {
            _manager->setScreen(_wifiScreen);
        }
        return true;
    }

    // Check sound buttons (row 1)
    for (int i = 0; i < 2; i++) {
        Rect r = getButtonRect(1, i);
        if (r.contains(x, y)) {
            bool newValue = (i == 1);
            if (_settings.soundEnabled != newValue) {
                _settings.soundEnabled = newValue;
                Sound::setEnabled(newValue);
                Sound::click();
                setNeedsFullRedraw(true);
            }
            return true;
        }
    }

    // Check sleep buttons (row 2)
    for (int i = 0; i < 4; i++) {
        Rect r = getButtonRect(2, i);
        if (r.contains(x, y)) {
            if (_settings.sleepTimeoutSecs != SLEEP_VALUES[i]) {
                _settings.sleepTimeoutSecs = SLEEP_VALUES[i];
                Sound::click();
                setNeedsFullRedraw(true);
            }
            return true;
        }
    }

    return false;
}
