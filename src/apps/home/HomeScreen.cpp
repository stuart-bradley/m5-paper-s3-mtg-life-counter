#include "HomeScreen.hpp"
#include "../../app/AppRegistry.hpp"
#include "../../app/Navigation.hpp"
#include "../../assets/icons.hpp"
#include "../../ui/Layout.hpp"
#include "../../utils/Sound.hpp"

void HomeScreen::onEnter() {
    setNeedsFullRedraw(true);
}

Rect HomeScreen::getAppCardRect(int index, int totalApps) const {
    int16_t totalWidth = totalApps * CARD_WIDTH + (totalApps - 1) * CARD_SPACING;
    int16_t startX = (Layout::screenW() - totalWidth) / 2;
    int16_t y = Toolbar::HEIGHT + (Layout::screenH() - Toolbar::HEIGHT - CARD_HEIGHT) / 2;
    return Rect(startX + index * (CARD_WIDTH + CARD_SPACING), y, CARD_WIDTH, CARD_HEIGHT);
}

void HomeScreen::drawAppCard(M5GFX* gfx, Rect r, const uint8_t* icon, const char* label) {
    // Card background
    gfx->fillRect(r.x, r.y, r.w, r.h, TFT_WHITE);
    gfx->drawRect(r.x, r.y, r.w, r.h, TFT_BLACK);

    // Icon centered in upper portion
    int16_t iconX = r.x + (r.w - ICON_SIZE) / 2;
    int16_t iconY = r.y + 20;
    if (icon) {
        drawIcon(gfx, icon, iconX, iconY, TFT_BLACK);
    }

    // Label below icon (larger text)
    gfx->setTextColor(TFT_BLACK);
    gfx->setTextDatum(MC_DATUM);
    gfx->setTextSize(2);
    gfx->drawString(label, r.x + r.w / 2, r.y + r.h - 24);
}

void HomeScreen::onFullRedraw(M5GFX* gfx) {
    auto& registry = AppRegistry::instance();
    int appCount = registry.launchableAppCount();

    for (int i = 0; i < appCount; i++) {
        App* app = registry.getLaunchableApp(i);
        if (app) {
            Rect r = getAppCardRect(i, appCount);
            drawAppCard(gfx, r, app->metadata().icon, app->metadata().name);
        }
    }
}

bool HomeScreen::handleTouch(int16_t x, int16_t y, bool pressed, bool released) {
    if (!released)
        return pressed;

    auto& registry = AppRegistry::instance();
    int appCount = registry.launchableAppCount();

    for (int i = 0; i < appCount; i++) {
        Rect r = getAppCardRect(i, appCount);
        if (r.contains(x, y)) {
            App* app = registry.getLaunchableApp(i);
            if (app) {
                Sound::click();
                Navigation::instance().launchApp(app);
            }
            return true;
        }
    }

    return false;
}
