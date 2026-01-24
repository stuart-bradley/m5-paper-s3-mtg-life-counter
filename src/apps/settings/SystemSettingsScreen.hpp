#pragma once

#include "../../ui/HeaderScreen.hpp"

class SettingsApp;
class Settings;

class SystemSettingsScreen : public HeaderScreen {
   public:
    explicit SystemSettingsScreen(SettingsApp* app);

    void onEnter() override;
    void onExit() override;

   protected:
    void onHeaderFullRedraw(M5GFX* gfx) override;
    bool onTouch(int16_t x, int16_t y, bool pressed, bool released) override;

   private:
    static constexpr int16_t ROW_HEIGHT = 60;
    static constexpr int16_t BUTTON_W = 80;
    static constexpr int16_t BUTTON_H = 40;
    static constexpr int16_t LABEL_X = 60;
    static constexpr int16_t BUTTONS_X = 300;
    static constexpr int16_t WIFI_BUTTON_W = 200;
    static constexpr int16_t AUTO_LABEL_X = 520;
    static constexpr int16_t AUTO_BUTTONS_X = 620;

    SettingsApp* _app;

    Settings& settings();  // Helper to access via App
    void saveSettings();   // Save settings to NVS immediately

    void drawButtons(M5GFX* gfx, int16_t x, int16_t y, const char* options[], int optionCount,
                     int selectedIndex);
    void drawRow(M5GFX* gfx, int16_t y, const char* label, const char* options[], int optionCount,
                 int selectedIndex);
    int getSoundIndex() const;
    int getSleepIndex() const;
    int getAutoConnectIndex() const;
    Rect getButtonRect(int row, int buttonIndex) const;
};
