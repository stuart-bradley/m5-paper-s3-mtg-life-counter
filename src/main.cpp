#include <M5Unified.h>
#include <Preferences.h>
#include "app/ScreenManager.hpp"
#include "models/Settings.hpp"
#include "ui/screens/HomeScreen.hpp"
#include "ui/screens/MTGLifeScreen.hpp"
#include "ui/screens/MTGSettingsScreen.hpp"
#include "ui/screens/SystemSettingsScreen.hpp"
#include "utils/Power.hpp"
#include "utils/Sound.hpp"

// Global instances
ScreenManager screenManager;
Settings globalSettings;

// Preference keys for screen restore (must match ScreenManager.cpp)
static constexpr const char* PREF_NAMESPACE = "app";
static constexpr const char* PREF_LAST_SCREEN = "lastScreen";

HomeScreen* homeScreen = nullptr;
MTGLifeScreen* mtgLifeScreen = nullptr;
MTGSettingsScreen* mtgSettingsScreen = nullptr;
SystemSettingsScreen* systemSettingsScreen = nullptr;

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

    Serial.println("\n========================================");
    Serial.println("M5Paper S3 MTG Life Counter");
    Serial.println("========================================\n");
    Serial.printf("Sleep timeout: %d seconds\n", globalSettings.sleepTimeoutSecs);

    // Create screens
    homeScreen = new HomeScreen(&screenManager);
    mtgLifeScreen = new MTGLifeScreen(&screenManager);
    mtgSettingsScreen = new MTGSettingsScreen(&screenManager);
    systemSettingsScreen = new SystemSettingsScreen(&screenManager);

    // Register screens for ID-based navigation
    screenManager.registerScreen(homeScreen);
    screenManager.registerScreen(mtgLifeScreen);
    screenManager.registerScreen(mtgSettingsScreen);
    screenManager.registerScreen(systemSettingsScreen);

    // Link screens for navigation
    mtgLifeScreen->setSettingsScreen(mtgSettingsScreen);
    mtgLifeScreen->setHomeScreen(homeScreen);
    systemSettingsScreen->setHomeScreen(homeScreen);
    homeScreen->setMTGScreen(mtgLifeScreen);
    homeScreen->setSettingsScreen(systemSettingsScreen);

    // Restore last screen or default to home
    Preferences appPrefs;
    appPrefs.begin(PREF_NAMESPACE, true);  // Read-only
    ScreenId savedScreenId = static_cast<ScreenId>(appPrefs.getUChar(PREF_LAST_SCREEN, 0));
    appPrefs.end();

    Screen* startScreen = screenManager.getScreenById(savedScreenId);
    if (startScreen) {
        Serial.printf("Restoring screen ID: %d\n", static_cast<int>(savedScreenId));
        screenManager.setScreen(startScreen);
    } else {
        Serial.println("Starting with home screen (no saved screen or invalid ID)");
        screenManager.setScreen(homeScreen);
    }

    Serial.println("Setup complete. Starting main loop.");
}

void enterSleepMode() {
    Serial.println("Entering sleep mode...");

    // Trigger current screen exit to save state
    Screen* current = screenManager.getCurrentScreen();
    if (current) {
        current->onExit();
    }

    Power::powerOff();
}

void loop() {
    M5.update();

    // Handle touch
    if (M5.Touch.getCount() > 0) {
        auto touch = M5.Touch.getDetail();
        bool pressed = touch.isPressed();
        bool released = touch.wasReleased();

        if (pressed || released) {
            Power::resetInactivityTimer();
            screenManager.handleTouch(touch.x, touch.y, pressed, released);
        }
    }

    // Check for sleep timeout
    if (Power::shouldSleep(globalSettings.sleepTimeoutSecs)) {
        enterSleepMode();
        return;  // Won't reach here after powerOff
    }

    // Update current screen
    screenManager.update();

    // Draw current screen
    screenManager.draw(&M5.Display);

    delay(20);  // ~50fps update rate
}
