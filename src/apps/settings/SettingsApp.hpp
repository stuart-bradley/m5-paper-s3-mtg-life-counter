#pragma once

#include "../../app/App.hpp"
#include "../../assets/icons.hpp"
#include "../../models/Settings.hpp"
#include "SystemSettingsScreen.hpp"
#include "WiFiScreen.hpp"

class SettingsApp : public App {
   public:
    SettingsApp();

    const AppMetadata& metadata() const override { return _metadata; }

    void onLaunch() override;
    void onSuspend() override;

    Screen* getMainScreen() override { return &_systemScreen; }
    Screen* getScreen(const char* id) override;

    // State access for screens
    Settings& settings() { return _settings; }

    // Screen access
    WiFiScreen* wifiScreen() { return &_wifiScreen; }

   private:
    static constexpr AppMetadata _metadata = {
        "settings", "Settings", ICON_SETTINGS,
        true  // Show in launcher
    };

    Settings _settings;
    SystemSettingsScreen _systemScreen;
    WiFiScreen _wifiScreen;
};
