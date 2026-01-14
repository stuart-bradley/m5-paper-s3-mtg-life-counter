#include <M5Unified.h>
#include "app/ScreenManager.hpp"
#include "ui/screens/HomeScreen.hpp"
#include "ui/screens/MTGLifeScreen.hpp"
#include "utils/Sound.hpp"

// Global instances
ScreenManager screenManager;
HomeScreen* homeScreen = nullptr;
MTGLifeScreen* mtgLifeScreen = nullptr;

void setup() {
    auto cfg = M5.config();
    cfg.serial_baudrate = 115200;
    M5.begin(cfg);

    M5.Display.setRotation(1);  // Landscape (960x540)
    M5.Display.setEpdMode(epd_fastest);
    M5.Display.fillScreen(TFT_WHITE);

    Sound::init();

    Serial.println("\n========================================");
    Serial.println("M5Paper S3 MTG Life Counter");
    Serial.println("========================================\n");

    // Create screens
    homeScreen = new HomeScreen(&screenManager);
    mtgLifeScreen = new MTGLifeScreen(&screenManager);

    // Start with home screen
    // For now, go directly to MTG life screen for testing
    screenManager.setScreen(mtgLifeScreen);

    Serial.println("Setup complete. Starting main loop.");
}

void loop() {
    M5.update();

    // Handle touch
    if (M5.Touch.getCount() > 0) {
        auto touch = M5.Touch.getDetail();
        bool pressed = touch.isPressed();
        bool released = touch.wasReleased();

        if (pressed || released) {
            screenManager.handleTouch(touch.x, touch.y, pressed, released);
        }
    }

    // Update current screen
    screenManager.update();

    // Draw current screen
    screenManager.draw(&M5.Display);

    delay(20);  // ~50fps update rate
}
