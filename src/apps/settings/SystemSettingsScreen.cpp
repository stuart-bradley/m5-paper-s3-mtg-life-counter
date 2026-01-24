#include "SystemSettingsScreen.hpp"
#include <Preferences.h>
#include "../../app/Navigation.hpp"
#include "../../ui/Layout.hpp"
#include "../../utils/Sound.hpp"
#include "SettingsApp.hpp"

static const char* BOOL_OPTIONS[] = {"OFF", "ON"};
static const char* SLEEP_OPTIONS[] = {"OFF", "1m", "5m", "10m"};
static const uint16_t SLEEP_VALUES[] = {0, 60, 300, 600};

SystemSettingsScreen::SystemSettingsScreen(SettingsApp* app)
    : HeaderScreen("SYSTEM SETTINGS"), _app(app) {}

Settings& SystemSettingsScreen::settings() {
    return _app->settings();
}

void SystemSettingsScreen::saveSettings() {
    Preferences prefs;
    settings().save(prefs);
}

void SystemSettingsScreen::onEnter() {
    // Load settings from NVS
    Preferences prefs;
    settings().load(prefs);
    Sound::setEnabled(settings().soundEnabled);

    // Set up navigation buttons
    setLeftButton("< HOME", []() { Navigation::instance().goHome(); });

    setNeedsFullRedraw(true);
}

void SystemSettingsScreen::onExit() {
    Preferences prefs;
    settings().save(prefs);
}

int SystemSettingsScreen::getSoundIndex() const {
    return _app->settings().soundEnabled ? 1 : 0;
}

int SystemSettingsScreen::getSleepIndex() const {
    for (int i = 0; i < 4; i++) {
        if (_app->settings().sleepTimeoutSecs == SLEEP_VALUES[i]) {
            return i;
        }
    }
    return 2;  // Default to 5m if not found
}

int SystemSettingsScreen::getAutoConnectIndex() const {
    return _app->settings().wifiAutoConnect ? 1 : 0;
}

Rect SystemSettingsScreen::getButtonRect(int row, int buttonIndex) const {
    int16_t y = Toolbar::HEIGHT + HeaderBar::HEIGHT + 40 + row * ROW_HEIGHT;
    int16_t x = BUTTONS_X + buttonIndex * (BUTTON_W + 10);
    return Rect(x, y, BUTTON_W, BUTTON_H);
}

void SystemSettingsScreen::drawButtons(M5GFX* gfx, int16_t x, int16_t y, const char* options[],
                                       int optionCount, int selectedIndex) {
    for (int i = 0; i < optionCount; i++) {
        int16_t bx = x + i * (BUTTON_W + 10);
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

void SystemSettingsScreen::drawRow(M5GFX* gfx, int16_t y, const char* label, const char* options[],
                                   int optionCount, int selectedIndex) {
    gfx->setTextColor(TFT_BLACK);
    gfx->setTextDatum(ML_DATUM);
    gfx->setTextSize(2);
    gfx->drawString(label, LABEL_X, y + BUTTON_H / 2);

    drawButtons(gfx, BUTTONS_X, y, options, optionCount, selectedIndex);
}

void SystemSettingsScreen::onHeaderFullRedraw(M5GFX* gfx) {
    // Draw setting rows
    int16_t startY = Layout::headerContentY() + 40;

    // Row 0: WiFi (navigation row + auto-connect toggle)
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

    // Auto-connect label and buttons
    gfx->setTextColor(TFT_BLACK);
    gfx->setTextDatum(ML_DATUM);
    gfx->setTextSize(2);
    gfx->drawString("Auto:", AUTO_LABEL_X, startY + BUTTON_H / 2);
    drawButtons(gfx, AUTO_BUTTONS_X, startY, BOOL_OPTIONS, 2, getAutoConnectIndex());

    // Row 1: Sound
    drawRow(gfx, startY + ROW_HEIGHT, "Sound:", BOOL_OPTIONS, 2, getSoundIndex());

    // Row 2: Auto-Sleep
    drawRow(gfx, startY + ROW_HEIGHT * 2, "Auto-Sleep:", SLEEP_OPTIONS, 4, getSleepIndex());
}

bool SystemSettingsScreen::onTouch(int16_t x, int16_t y, bool pressed, bool released) {
    if (!released)
        return pressed;

    // Check WiFi button (row 0)
    int16_t startY = Layout::headerContentY() + 40;
    Rect wifiRect(BUTTONS_X, startY, WIFI_BUTTON_W, BUTTON_H);
    if (wifiRect.contains(x, y)) {
        Sound::click();
        Navigation::instance().pushScreen(_app->wifiScreen());
        return true;
    }

    // Check auto-connect buttons (row 0)
    for (int i = 0; i < 2; i++) {
        Rect r(AUTO_BUTTONS_X + i * (BUTTON_W + 10), startY, BUTTON_W, BUTTON_H);
        if (r.contains(x, y)) {
            bool newValue = (i == 1);
            if (settings().wifiAutoConnect != newValue) {
                settings().wifiAutoConnect = newValue;
                saveSettings();
                Sound::click();
                setNeedsFullRedraw(true);
            }
            return true;
        }
    }

    // Check sound buttons (row 1)
    for (int i = 0; i < 2; i++) {
        Rect r = getButtonRect(1, i);
        if (r.contains(x, y)) {
            bool newValue = (i == 1);
            if (settings().soundEnabled != newValue) {
                settings().soundEnabled = newValue;
                Sound::setEnabled(newValue);
                saveSettings();
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
            if (settings().sleepTimeoutSecs != SLEEP_VALUES[i]) {
                settings().sleepTimeoutSecs = SLEEP_VALUES[i];
                saveSettings();
                Sound::click();
                setNeedsFullRedraw(true);
            }
            return true;
        }
    }

    return false;
}
