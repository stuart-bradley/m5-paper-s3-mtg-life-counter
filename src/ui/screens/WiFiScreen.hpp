#pragma once

#include <WiFi.h>
#include <vector>
#include "../HeaderBar.hpp"
#include "../Keyboard.hpp"
#include "../Screen.hpp"
#include "../Toolbar.hpp"

class ScreenManager;

struct WiFiNetwork {
    String ssid;
    int32_t rssi;
    bool secured;
    bool connected;
};

class WiFiScreen : public Screen {
   public:
    WiFiScreen(ScreenManager* manager);

    ScreenId getScreenId() const override { return ScreenId::WiFi; }

    void setSettingsScreen(Screen* screen);

    void onEnter() override;
    void onExit() override;
    void update() override;
    void draw(M5GFX* gfx) override;
    bool handleTouch(int16_t x, int16_t y, bool pressed, bool released) override;

   private:
    static constexpr int16_t ROW_HEIGHT = 50;
    static constexpr int16_t LIST_START_Y = Toolbar::HEIGHT + HeaderBar::HEIGHT + 50;
    static constexpr int16_t MAX_VISIBLE_NETWORKS = 7;
    static constexpr int16_t ROW_PADDING = 20;

    ScreenManager* _manager;
    Screen* _settingsScreen = nullptr;
    Toolbar _toolbar;
    HeaderBar _headerBar;
    Keyboard* _keyboard = nullptr;

    std::vector<WiFiNetwork> _networks;
    int _scrollOffset = 0;
    int _selectedIndex = -1;
    bool _scanning = false;
    bool _connecting = false;
    unsigned long _lastScanTime = 0;
    String _pendingSSID;

    void startScan(bool showSplash = true);
    void updateScanResults();
    void connectToNetwork(const String& ssid, const String& password);
    void disconnectFromNetwork();
    void updateDisconnectButton();
    void drawNetworkList(M5GFX* gfx);
    void drawNetwork(M5GFX* gfx, int16_t y, const WiFiNetwork& network, bool selected);
    void drawConnectingSplash(const String& ssid);
    void drawScanningSplash();
    void drawSignalBars(M5GFX* gfx, int16_t x, int16_t y, int32_t rssi);
    const char* getSignalBars(int32_t rssi);
    void onKeyboardComplete(const char* password, bool confirmed);

    Rect getNetworkRect(int index) const;
};
