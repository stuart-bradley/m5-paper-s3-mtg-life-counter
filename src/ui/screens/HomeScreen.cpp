#include "HomeScreen.hpp"
#include "../../app/ScreenManager.hpp"
#include "../../assets/icons.hpp"
#include "../../utils/Sound.hpp"

HomeScreen::HomeScreen(ScreenManager* manager) : _manager(manager) {}

void HomeScreen::onEnter() {
    setNeedsFullRedraw(true);
}

Rect HomeScreen::getSettingsCardRect() const {
    int16_t totalWidth = 2 * CARD_WIDTH + CARD_SPACING;
    int16_t startX = (960 - totalWidth) / 2;
    int16_t y = Toolbar::HEIGHT + (540 - Toolbar::HEIGHT - CARD_HEIGHT) / 2;
    return Rect(startX, y, CARD_WIDTH, CARD_HEIGHT);
}

Rect HomeScreen::getMTGCardRect() const {
    int16_t totalWidth = 2 * CARD_WIDTH + CARD_SPACING;
    int16_t startX = (960 - totalWidth) / 2;
    int16_t y = Toolbar::HEIGHT + (540 - Toolbar::HEIGHT - CARD_HEIGHT) / 2;
    return Rect(startX + CARD_WIDTH + CARD_SPACING, y, CARD_WIDTH, CARD_HEIGHT);
}

void HomeScreen::drawAppCard(M5GFX* gfx, Rect r, const uint8_t* icon, const char* label) {
    // Card background
    gfx->fillRect(r.x, r.y, r.w, r.h, TFT_WHITE);
    gfx->drawRect(r.x, r.y, r.w, r.h, TFT_BLACK);

    // Icon centered in upper portion
    int16_t iconX = r.x + (r.w - ICON_SIZE) / 2;
    int16_t iconY = r.y + 20;
    drawIcon(gfx, icon, iconX, iconY, TFT_BLACK);

    // Label below icon (larger text)
    gfx->setTextColor(TFT_BLACK);
    gfx->setTextDatum(MC_DATUM);
    gfx->setTextSize(2);
    gfx->drawString(label, r.x + r.w / 2, r.y + r.h - 24);
}

void HomeScreen::draw(M5GFX* gfx) {
    bool needsDisplay = false;

    if (needsFullRedraw()) {
        gfx->fillScreen(TFT_WHITE);
        setNeedsFullRedraw(false);
        _toolbar.setDirty(true);
        needsDisplay = true;

        // Draw app cards (only on full redraw)
        drawAppCard(gfx, getSettingsCardRect(), ICON_SETTINGS, "Settings");
        drawAppCard(gfx, getMTGCardRect(), ICON_MTG, "MTG Life");
    }

    // Update toolbar time
    _toolbar.update();

    // Draw toolbar only if dirty
    if (_toolbar.isDirty()) {
        _toolbar.draw(gfx);
        needsDisplay = true;
    }

    if (needsDisplay) {
        gfx->display();
    }
}

bool HomeScreen::handleTouch(int16_t x, int16_t y, bool pressed, bool released) {
    if (!released)
        return pressed;

    Rect settingsR = getSettingsCardRect();
    if (settingsR.contains(x, y)) {
        Sound::click();
        if (_settingsScreen) {
            _manager->setScreen(_settingsScreen);
        }
        return true;
    }

    Rect mtgR = getMTGCardRect();
    if (mtgR.contains(x, y)) {
        Sound::click();
        if (_mtgScreen) {
            _manager->setScreen(_mtgScreen);
        }
        return true;
    }

    return false;
}
