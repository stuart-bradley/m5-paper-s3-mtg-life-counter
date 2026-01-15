#include "WiFiScreen.hpp"
#include <M5Unified.h>
#include <Preferences.h>
#include "../../app/ScreenManager.hpp"
#include "../../utils/Sound.hpp"

static constexpr const char* WIFI_PREF_NS = "wifi";
static constexpr const char* PREF_SSID = "ssid";
static constexpr const char* PREF_PASS = "pass";

WiFiScreen::WiFiScreen(ScreenManager* manager) : _manager(manager) {
    _headerBar.setTitle("SELECT WIFI NETWORK");
}

void WiFiScreen::setSettingsScreen(Screen* screen) {
    _settingsScreen = screen;
    _headerBar.setLeftButton("< BACK", [this]() {
        if (_settingsScreen) {
            _manager->setScreen(_settingsScreen);
        }
    });
}

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

    // Set up scan button (always available when not connected)
    // Disconnect button will replace it when connected
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
        Serial.println("[WiFi] Scan skipped - already busy");
        return;
    }

    Serial.println("[WiFi] Starting scan...");
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
    Serial.printf("[WiFi] Scan complete, found %d networks\n", result);

    _scanning = false;
    _networks.clear();

    if (result < 0) {
        Serial.printf("[WiFi] Scan failed with error: %d\n", result);
        setNeedsFullRedraw(true);
        return;
    }

    String currentSSID = WiFi.SSID();
    Serial.printf("[WiFi] Currently connected to: '%s'\n", currentSSID.c_str());

    for (int i = 0; i < result; i++) {
        WiFiNetwork net;
        net.ssid = WiFi.SSID(i);
        net.rssi = WiFi.RSSI(i);
        net.secured = (WiFi.encryptionType(i) != WIFI_AUTH_OPEN);
        net.connected = (net.ssid == currentSSID && WiFi.status() == WL_CONNECTED);

        Serial.printf("[WiFi]   %d: '%s' (%d dBm) %s %s\n", i, net.ssid.c_str(), net.rssi,
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
    Serial.printf("[WiFi] Final network count: %d\n", (int)_networks.size());
}

void WiFiScreen::updateScanResults() {
    // No longer used - scan is now synchronous
}

void WiFiScreen::drawScanningSplash() {
    M5GFX* gfx = &M5.Display;

    // Modal dimensions
    int16_t boxW = 400;
    int16_t boxH = 100;
    int16_t boxX = (960 - boxW) / 2;
    int16_t boxY = (540 - boxH) / 2;

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
    int16_t boxX = (960 - boxW) / 2;
    int16_t boxY = (540 - boxH) / 2;

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
    Serial.println("[WiFi] Disconnecting...");
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
    _headerBar.setRightButton("SCAN", [this]() {
        Sound::click();
        startScan();
    });
}

void WiFiScreen::update() {
    if (_scanning) {
        updateScanResults();
    }
}

void WiFiScreen::draw(M5GFX* gfx) {
    bool needsDisplay = false;

    if (needsFullRedraw()) {
        gfx->fillScreen(TFT_WHITE);
        setNeedsFullRedraw(false);
        _toolbar.setDirty(true);
        needsDisplay = true;

        _headerBar.draw(gfx);
        drawNetworkList(gfx);
    }

    // Draw keyboard overlay if active
    if (_keyboard && _keyboard->isDirty()) {
        _keyboard->draw(gfx);
        needsDisplay = true;
    }

    _toolbar.update();
    if (_toolbar.isDirty()) {
        _toolbar.draw(gfx);
        needsDisplay = true;
    }

    if (needsDisplay) {
        gfx->display();
    }
}

void WiFiScreen::drawNetworkList(M5GFX* gfx) {
    int16_t y = LIST_START_Y;

    // Section header
    gfx->setTextColor(TFT_BLACK);
    gfx->setTextDatum(MC_DATUM);
    gfx->setTextSize(1);

    if (_scanning) {
        gfx->drawString("-- Scanning for networks... --", 480, y - 25);
    } else if (_connecting) {
        gfx->drawString("-- Connecting... --", 480, y - 25);
    } else if (_networks.empty()) {
        gfx->drawString("-- No networks found --", 480, y - 25);
    } else {
        char header[40];
        snprintf(header, sizeof(header), "-- Available Networks (%d) --", (int)_networks.size());
        gfx->drawString(header, 480, y - 25);
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
    int16_t w = 960 - ROW_PADDING * 2;
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
    // Using ASCII bars: _ for low, = for medium, # for high
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
    return Rect(ROW_PADDING, y, 960 - ROW_PADDING * 2, ROW_HEIGHT - 4);
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

bool WiFiScreen::handleTouch(int16_t x, int16_t y, bool pressed, bool released) {
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

    // Header bar handles back button
    if (_headerBar.handleTouch(x, y, pressed, released)) {
        return true;
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
