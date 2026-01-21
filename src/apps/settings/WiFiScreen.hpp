#pragma once

#include <WiFi.h>
#include <vector>
#include "../../ui/HeaderScreen.hpp"
#include "../../ui/Keyboard.hpp"
#include "../../ui/Layout.hpp"

class SettingsApp;

struct WiFiNetwork {
    String ssid;
    int32_t rssi;
    bool secured;
    bool connected;
};

class WiFiScreen : public HeaderScreen {
   public:
    explicit WiFiScreen(SettingsApp* app);

    const char* screenId() const override { return "wifi"; }

    void onEnter() override;
    void onExit() override;

   protected:
    void onUpdate() override;
    void onHeaderFullRedraw(M5GFX* gfx) override;
    bool onDraw(M5GFX* gfx) override;
    bool onTouch(int16_t x, int16_t y, bool pressed, bool released) override;

   private:
    static constexpr int16_t ROW_HEIGHT = 50;
    static constexpr int16_t LIST_START_Y = Layout::TOOLBAR_H + Layout::HEADER_H + 50;
    static constexpr int16_t MAX_VISIBLE_NETWORKS = 7;
    static constexpr int16_t ROW_PADDING = 20;

    SettingsApp* _app;
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
    const char* getSignalBars(int32_t rssi);
    void onKeyboardComplete(const char* password, bool confirmed);

    Rect getNetworkRect(int index) const;
};
