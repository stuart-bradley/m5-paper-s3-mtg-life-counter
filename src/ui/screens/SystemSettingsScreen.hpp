#pragma once

#include <Preferences.h>
#include "../../models/Settings.hpp"
#include "../HeaderBar.hpp"
#include "../Screen.hpp"
#include "../Toolbar.hpp"

class ScreenManager;

class SystemSettingsScreen : public Screen {
   public:
    SystemSettingsScreen(ScreenManager* manager);

    ScreenId getScreenId() const override { return ScreenId::SystemSettings; }

    void setHomeScreen(Screen* screen);

    void onEnter() override;
    void onExit() override;
    void draw(M5GFX* gfx) override;
    bool handleTouch(int16_t x, int16_t y, bool pressed, bool released) override;

   private:
    static constexpr int16_t ROW_HEIGHT = 60;
    static constexpr int16_t BUTTON_W = 80;
    static constexpr int16_t BUTTON_H = 40;
    static constexpr int16_t LABEL_X = 60;
    static constexpr int16_t BUTTONS_X = 300;

    ScreenManager* _manager;
    Screen* _homeScreen = nullptr;
    Toolbar _toolbar;
    HeaderBar _headerBar;
    Settings _settings;

    void loadSettings();
    void saveSettings();
    void drawRow(M5GFX* gfx, int16_t y, const char* label, const char* options[], int optionCount,
                 int selectedIndex);
    int getSoundIndex() const;
    int getSleepIndex() const;
    Rect getButtonRect(int row, int buttonIndex) const;
};
