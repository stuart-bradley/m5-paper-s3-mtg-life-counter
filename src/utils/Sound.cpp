#include "Sound.hpp"
#include <M5Unified.h>

namespace Sound {

static bool s_enabled = true;

void init() {
    M5.Speaker.begin();
}

void setEnabled(bool enabled) {
    s_enabled = enabled;
}

bool isEnabled() {
    return s_enabled;
}

void click() {
    if (!s_enabled)
        return;
    M5.Speaker.tone(1000, 20);
}

void lifeUp() {
    if (!s_enabled)
        return;
    // Ascending tone: two quick notes
    M5.Speaker.tone(800, 25);
    delay(30);
    M5.Speaker.tone(1200, 25);
}

void lifeDown() {
    if (!s_enabled)
        return;
    // Descending tone: two quick notes
    M5.Speaker.tone(1200, 25);
    delay(30);
    M5.Speaker.tone(800, 25);
}

void alert() {
    if (!s_enabled)
        return;
    M5.Speaker.tone(500, 200);
}

}  // namespace Sound
