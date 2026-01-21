#include "WiFiScreen.hpp"
#include <M5Unified.h>
#include <Preferences.h>
#include <algorithm>
#include "../../app/Navigation.hpp"
#include "../../utils/Log.hpp"
#include "../../utils/Sound.hpp"
#include "SettingsApp.hpp"

static constexpr const char* WIFI_PREF_NS = "wifi";
static constexpr const char* PREF_SSID = "ssid";
static constexpr const char* PREF_PASS = "pass";

WiFiScreen::WiFiScreen(SettingsApp* app) : HeaderScreen("SELECT WIFI NETWORK"), _app(app) {}

void WiFiScreen::onEnter() {
    _networks.clear();
    _scrollOffset = 0;
    _selectedIndex = -1;
    _scanning = false;
    _connecting = false;
    if (_keyboard) {
        delete _keyboard;
        _keyboard = nullptr;
    }

    // Set up navigation buttons - pop back to system settings
    setLeftButton("< BACK", []() { Navigation::instance().popScreen(); });

    // Set up scan button (always available when not connected)
    updateDisconnectButton();

    setNeedsFullRedraw(true);
    // Don't auto-scan - user presses SCAN button
}

void WiFiScreen::onExit() {
    if (_keyboard) {
        delete _keyboard;
        _keyboard = nullptr;
    }
}

void WiFiScreen::startScan(bool showSplash) {
    if (_scanning || _connecting) {
        LOG_D("[WiFi] Scan skipped - already busy");
        return;
    }

    LOG_D("[WiFi] Starting scan...");
    _scanning = true;
    _lastScanTime = millis();

    // Show scanning splash immediately (before blocking)
    if (showSplash) {
        drawScanningSplash();
    }

    // Clean up any previous scan results
    WiFi.scanDelete();

    // Ensure WiFi is in station mode (don't disconnect - preserves current connection)
    WiFi.mode(WIFI_STA);

    // Small delay to let WiFi settle after mode change
    delay(100);

    // Use synchronous scan for reliability
    int16_t result = WiFi.scanNetworks(false, true);  // sync, show hidden
    LOG_D("[WiFi] Scan complete, found %d networks", result);

    _scanning = false;
    _networks.clear();

    if (result < 0) {
        LOG_D("[WiFi] Scan failed with error: %d", result);
        setNeedsFullRedraw(true);
        return;
    }

    String currentSSID = WiFi.SSID();
    LOG_D("[WiFi] Currently connected to: '%s'", currentSSID.c_str());

    for (int i = 0; i < result; i++) {
        WiFiNetwork net;
        net.ssid = WiFi.SSID(i);
        net.rssi = WiFi.RSSI(i);
        net.secured = (WiFi.encryptionType(i) != WIFI_AUTH_OPEN);
        net.connected = (net.ssid == currentSSID && WiFi.status() == WL_CONNECTED);

        LOG_D("[WiFi]   %d: '%s' (%d dBm) %s %s", i, net.ssid.c_str(), net.rssi,
              net.secured ? "[secured]" : "[open]", net.connected ? "[connected]" : "");

        // Skip empty SSIDs
        if (net.ssid.length() > 0) {
            _networks.push_back(net);
        }
    }

    // Sort by signal strength (strongest first), but connected network always first
    std::sort(_networks.begin(), _networks.end(), [](const WiFiNetwork& a, const WiFiNetwork& b) {
        if (a.connected != b.connected)
            return a.connected;
        return a.rssi > b.rssi;
    });

    WiFi.scanDelete();
    updateDisconnectButton();
    setNeedsFullRedraw(true);
    LOG_D("[WiFi] Final network count: %d", (int)_networks.size());
}

void WiFiScreen::updateScanResults() {
    // No longer used - scan is now synchronous
}

void WiFiScreen::drawScanningSplash() {
    M5GFX* gfx = &M5.Display;

    // Modal dimensions
    int16_t boxW = 400;
    int16_t boxH = 100;
    int16_t boxX = (Layout::screenW() - boxW) / 2;
    int16_t boxY = (Layout::screenH() - boxH) / 2;

    // Draw modal box with double border
    gfx->fillRect(boxX, boxY, boxW, boxH, TFT_WHITE);
    gfx->drawRect(boxX, boxY, boxW, boxH, TFT_BLACK);
    gfx->drawRect(boxX + 2, boxY + 2, boxW - 4, boxH - 4, TFT_BLACK);

    // Title
    gfx->setTextColor(TFT_BLACK);
    gfx->setTextDatum(MC_DATUM);
    gfx->setTextSize(2);
    gfx->drawString("SCANNING...", boxX + boxW / 2, boxY + boxH / 2);

    gfx->display();
}

void WiFiScreen::drawConnectingSplash(const String& ssid) {
    M5GFX* gfx = &M5.Display;

    // Modal dimensions
    int16_t boxW = 400;
    int16_t boxH = 120;
    int16_t boxX = (Layout::screenW() - boxW) / 2;
    int16_t boxY = (Layout::screenH() - boxH) / 2;

    // Draw modal box with double border
    gfx->fillRect(boxX, boxY, boxW, boxH, TFT_WHITE);
    gfx->drawRect(boxX, boxY, boxW, boxH, TFT_BLACK);
    gfx->drawRect(boxX + 2, boxY + 2, boxW - 4, boxH - 4, TFT_BLACK);

    // Title
    gfx->setTextColor(TFT_BLACK);
    gfx->setTextDatum(MC_DATUM);
    gfx->setTextSize(2);
    gfx->drawString("CONNECTING...", boxX + boxW / 2, boxY + 35);

    // Network name
    gfx->setTextSize(1);
    gfx->drawString(ssid.c_str(), boxX + boxW / 2, boxY + 75);

    gfx->display();
}

void WiFiScreen::connectToNetwork(const String& ssid, const String& password) {
    _connecting = true;

    // Show connecting splash immediately (before blocking)
    drawConnectingSplash(ssid);

    WiFi.begin(ssid.c_str(), password.c_str());

    // Wait for connection (with timeout)
    unsigned long start = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - start < 10000) {
        delay(100);
    }

    if (WiFi.status() == WL_CONNECTED) {
        // Save credentials
        Preferences prefs;
        prefs.begin(WIFI_PREF_NS, false);
        prefs.putString(PREF_SSID, ssid);
        prefs.putString(PREF_PASS, password);
        prefs.end();
    }

    _connecting = false;

    // Update the connected status in our cached network list
    String connectedSSID = WiFi.SSID();
    for (auto& net : _networks) {
        net.connected = (net.ssid == connectedSSID && WiFi.status() == WL_CONNECTED);
    }

    updateDisconnectButton();
    setNeedsFullRedraw(true);
}

void WiFiScreen::disconnectFromNetwork() {
    LOG_I("[WiFi] Disconnecting...");
    WiFi.disconnect(true);  // true = also erase stored credentials

    // Clear saved credentials
    Preferences prefs;
    prefs.begin(WIFI_PREF_NS, false);
    prefs.remove(PREF_SSID);
    prefs.remove(PREF_PASS);
    prefs.end();

    // Update the connected status in our cached network list
    for (auto& net : _networks) {
        net.connected = false;
    }

    updateDisconnectButton();
    setNeedsFullRedraw(true);
}

void WiFiScreen::updateDisconnectButton() {
    // Always show SCAN button - user can rescan anytime
    // Tapping connected network will disconnect
    setRightButton("SCAN", [this]() {
        Sound::click();
        startScan();
    });
}

void WiFiScreen::onUpdate() {
    if (_scanning) {
        updateScanResults();
    }
}

void WiFiScreen::onHeaderFullRedraw(M5GFX* gfx) {
    drawNetworkList(gfx);
    if (_keyboard) {
        _keyboard->setDirty(true);
        _keyboard->draw(gfx);
    }
}

bool WiFiScreen::onDraw(M5GFX* gfx) {
    bool needsDisplay = false;

    // Draw keyboard overlay if active
    if (_keyboard && _keyboard->isDirty()) {
        _keyboard->draw(gfx);
        needsDisplay = true;
    }

    return needsDisplay;
}

void WiFiScreen::drawNetworkList(M5GFX* gfx) {
    int16_t y = LIST_START_Y;

    // Section header
    gfx->setTextColor(TFT_BLACK);
    gfx->setTextDatum(MC_DATUM);
    gfx->setTextSize(1);

    if (_scanning) {
        gfx->drawString("-- Scanning for networks... --", Layout::centerX(), y - 25);
    } else if (_connecting) {
        gfx->drawString("-- Connecting... --", Layout::centerX(), y - 25);
    } else if (_networks.empty()) {
        gfx->drawString("-- No networks found --", Layout::centerX(), y - 25);
    } else {
        char header[40];
        snprintf(header, sizeof(header), "-- Available Networks (%d) --", (int)_networks.size());
        gfx->drawString(header, Layout::centerX(), y - 25);
    }

    // Draw visible networks
    int endIdx = std::min(_scrollOffset + MAX_VISIBLE_NETWORKS, (int)_networks.size());
    for (int i = _scrollOffset; i < endIdx; i++) {
        int16_t rowY = y + (i - _scrollOffset) * ROW_HEIGHT;
        drawNetwork(gfx, rowY, _networks[i], i == _selectedIndex);
    }
}

void WiFiScreen::drawNetwork(M5GFX* gfx, int16_t y, const WiFiNetwork& network, bool selected) {
    int16_t x = ROW_PADDING;
    int16_t w = Layout::screenW() - ROW_PADDING * 2;
    int16_t h = ROW_HEIGHT - 4;

    // Background
    if (selected) {
        gfx->fillRect(x, y, w, h, TFT_BLACK);
        gfx->setTextColor(TFT_WHITE);
    } else {
        gfx->drawRect(x, y, w, h, TFT_BLACK);
        gfx->setTextColor(TFT_BLACK);
    }

    // Signal bars (left side)
    gfx->setTextDatum(ML_DATUM);
    gfx->setTextSize(2);
    const char* bars = getSignalBars(network.rssi);
    gfx->drawString(bars, x + 15, y + h / 2);

    // Network name (center-left)
    gfx->setTextDatum(ML_DATUM);
    gfx->setTextSize(1);
    gfx->drawString(network.ssid.c_str(), x + 100, y + h / 2);

    // Status (right side)
    gfx->setTextDatum(MR_DATUM);
    gfx->setTextSize(1);
    if (network.connected) {
        gfx->drawString("* CONNECTED", x + w - 15, y + h / 2);
    } else if (network.secured) {
        gfx->drawString("[SECURED]", x + w - 15, y + h / 2);
    }
}

const char* WiFiScreen::getSignalBars(int32_t rssi) {
    // RSSI typically ranges from -100 (weak) to -30 (strong)
    if (rssi >= -50)
        return "####";  // Excellent
    if (rssi >= -60)
        return "###-";  // Good
    if (rssi >= -70)
        return "##--";  // Fair
    if (rssi >= -80)
        return "#---";  // Weak
    return "----";      // Very weak
}

Rect WiFiScreen::getNetworkRect(int index) const {
    int visibleIndex = index - _scrollOffset;
    if (visibleIndex < 0 || visibleIndex >= MAX_VISIBLE_NETWORKS) {
        return Rect(0, 0, 0, 0);  // Off-screen
    }
    int16_t y = LIST_START_Y + visibleIndex * ROW_HEIGHT;
    return Rect(ROW_PADDING, y, Layout::screenW() - ROW_PADDING * 2, ROW_HEIGHT - 4);
}

void WiFiScreen::onKeyboardComplete(const char* password, bool confirmed) {
    if (_keyboard) {
        delete _keyboard;
        _keyboard = nullptr;
    }

    if (confirmed && !_pendingSSID.isEmpty()) {
        connectToNetwork(_pendingSSID, password);
    }
    _pendingSSID = "";
    setNeedsFullRedraw(true);
}

bool WiFiScreen::onTouch(int16_t x, int16_t y, bool pressed, bool released) {
    // Keyboard takes priority
    if (_keyboard) {
        if (_keyboard->handleTouch(x, y, pressed, released)) {
            return true;
        }
        // Touch outside keyboard dismisses it
        if (released && !_keyboard->contains(x, y)) {
            onKeyboardComplete("", false);
            return true;
        }
        return pressed;
    }

    if (!released)
        return pressed;

    // Check network selection
    for (int i = _scrollOffset;
         i < _scrollOffset + MAX_VISIBLE_NETWORKS && i < (int)_networks.size(); i++) {
        Rect r = getNetworkRect(i);
        if (r.contains(x, y)) {
            Sound::click();
            const WiFiNetwork& net = _networks[i];

            if (net.connected) {
                // Tapping connected network disconnects
                disconnectFromNetwork();
            } else if (net.secured) {
                // Show keyboard for password
                _pendingSSID = net.ssid;
                _selectedIndex = i;
                _keyboard = new Keyboard("", [this](const char* result, bool confirmed) {
                    onKeyboardComplete(result, confirmed);
                });
                setNeedsFullRedraw(true);
            } else {
                // Open network, connect directly
                connectToNetwork(net.ssid, "");
            }
            return true;
        }
    }

    return false;
}
