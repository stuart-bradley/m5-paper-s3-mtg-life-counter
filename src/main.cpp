#include <M5Unified.h>
#include <Preferences.h>
#include <WiFi.h>
#include "app/AppRegistry.hpp"
#include "app/Navigation.hpp"
#include "apps/home/HomeApp.hpp"
#include "apps/mtg/MTGApp.hpp"
#include "apps/settings/SettingsApp.hpp"
#include "models/Settings.hpp"
#include "utils/Log.hpp"
#include "utils/Power.hpp"
#include "utils/Sound.hpp"

// Global instances
Settings globalSettings;

// App instances
static HomeApp homeApp;
static MTGApp mtgApp;
static SettingsApp settingsApp;

void tryWifiAutoConnect() {
    Preferences prefs;
    if (!prefs.begin("wifi", true))
        return;  // read-only

    String ssid = prefs.getString("ssid", "");
    String pass = prefs.getString("pass", "");
    prefs.end();

    if (ssid.isEmpty()) {
        LOG_I("WiFi auto-connect: no saved network");
        return;
    }

    LOG_I("WiFi auto-connect: connecting to %s", ssid.c_str());

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid.c_str(), pass.c_str());

    // Wait up to 5 seconds for connection
    unsigned long start = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - start < 5000) {
        delay(100);
    }

    if (WiFi.status() == WL_CONNECTED) {
        LOG_I("WiFi auto-connect: success, IP=%s", WiFi.localIP().toString().c_str());
    } else {
        LOG_W("WiFi auto-connect: failed (timeout)");
        WiFi.disconnect();
    }
}

void setup() {
    auto cfg = M5.config();
    cfg.serial_baudrate = 115200;
    M5.begin(cfg);

    M5.Display.setRotation(1);  // Landscape (960x540)
    M5.Display.setEpdMode(epd_fastest);
    M5.Display.fillScreen(TFT_WHITE);

    Sound::init();
    Power::init();

    // Load settings for sleep timeout
    Preferences prefs;
    globalSettings.load(prefs);

    LOG_I("========================================");
    LOG_I("M5Paper S3 App Platform");
    LOG_I("========================================");
    LOG_I("Sleep timeout: %d seconds", globalSettings.sleepTimeoutSecs);

    // Auto-connect to WiFi if enabled
    if (globalSettings.wifiAutoConnect) {
        tryWifiAutoConnect();
    }

    // Register apps with the registry
    auto& registry = AppRegistry::instance();
    registry.registerApp(&homeApp);
    registry.registerApp(&mtgApp);
    registry.registerApp(&settingsApp);

    // Restore previous navigation state or go home
    Navigation::instance().restoreState();

    LOG_I("Setup complete. Starting main loop.");
}

void enterSleepMode() {
    LOG_I("Entering sleep mode...");

    // Save navigation state before sleep
    Navigation::instance().saveState();

    Power::powerOff();
}

void loop() {
    M5.update();

    auto& nav = Navigation::instance();

    // Handle touch
    if (M5.Touch.getCount() > 0) {
        auto touch = M5.Touch.getDetail();
        bool pressed = touch.isPressed();
        bool released = touch.wasReleased();

        if (pressed || released) {
            Power::resetInactivityTimer();
            nav.handleTouch(touch.x, touch.y, pressed, released);
        }
    }

    // Check for sleep timeout
    if (Power::shouldSleep(globalSettings.sleepTimeoutSecs)) {
        enterSleepMode();
        return;  // Won't reach here after powerOff
    }

    // Update and draw
    nav.update();
    nav.draw(&M5.Display);

    delay(20);  // ~50fps update rate
}
