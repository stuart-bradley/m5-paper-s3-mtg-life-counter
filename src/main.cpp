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

    // Link screens for navigation
    mtgLifeScreen->setSettingsScreen(mtgSettingsScreen);
    mtgLifeScreen->setHomeScreen(homeScreen);
    systemSettingsScreen->setHomeScreen(homeScreen);
    homeScreen->setMTGScreen(mtgLifeScreen);
    homeScreen->setSettingsScreen(systemSettingsScreen);

    // Start with home screen
    screenManager.setScreen(homeScreen);

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
