#include <M5Unified.h>

void setup() {
    auto cfg = M5.config();
    M5.begin(cfg);
    
    M5.Display.setRotation(1);  // Landscape
    M5.Display.setTextSize(3);
    M5.Display.setTextColor(TFT_BLACK);
    M5.Display.fillScreen(TFT_WHITE);
    M5.Display.setCursor(50, 250);
    M5.Display.println("Celia I love you");
    M5.Display.println("<3 <3 <3");
}

void loop() {
    M5.update();
    delay(100);
}