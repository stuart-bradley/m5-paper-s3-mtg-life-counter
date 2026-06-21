#include "MTGSettingsScreen.hpp"
#include <Arduino.h>
#include <Preferences.h>
#include "../../app/Navigation.hpp"
#include "../../ui/Layout.hpp"
#include "../../utils/Sound.hpp"
#include "MTGApp.hpp"

// Layout constants
static constexpr int16_t HEADER_Y = Layout::TOOLBAR_H;
static constexpr int16_t HEADER_H = Layout::HEADER_H;
static constexpr int16_t CONTENT_Y = HEADER_Y + HEADER_H + 12;
static constexpr int16_t MARGIN = 16;
// Two-column layout
static constexpr int16_t LEFT_COL_X = MARGIN;
static constexpr int16_t LEFT_COL_W = 460;
static constexpr int16_t RIGHT_COL_X = LEFT_COL_X + LEFT_COL_W + MARGIN;
static constexpr int16_t RIGHT_COL_W = Layout::screenW() - RIGHT_COL_X - MARGIN;
// Section dimensions
static constexpr int16_t SECTION_H = 100;  // Extra padding at bottom
static constexpr int16_t SECTION_GAP = 12;
static constexpr int16_t SECTION_HEADER_H = 28;
// Button dimensions
static constexpr int16_t SELECT_BTN_W = 64;
static constexpr int16_t SELECT_BTN_H = 50;
static constexpr int16_t RESET_BTN_H = 70;

MTGSettingsScreen::MTGSettingsScreen(MTGApp* app) : HeaderScreen("GAME SETTINGS"), _app(app) {}

GameState& MTGSettingsScreen::gameState() {
    return _app->gameState();
}

void MTGSettingsScreen::onEnter() {
    // State is shared via App, but reload from NVS to be safe
    Preferences prefs;
    gameState().load(prefs);

    // Set up back button - pops back to the life screen
    setLeftButton("< BACK", []() { Navigation::instance().popScreen(); });

    layoutButtons();
    setNeedsFullRedraw(true);
}

void MTGSettingsScreen::onExit() {
    Preferences prefs;
    gameState().save(prefs);
}

void MTGSettingsScreen::layoutButtons() {
    // Left column, Section 1: Player count buttons (2-6)
    const int16_t playerBtnY = CONTENT_Y + SECTION_HEADER_H + 12;
    const int16_t playerBtnGap = 8;
    const int16_t playerBtnsWidth = 5 * SELECT_BTN_W + 4 * playerBtnGap;
    const int16_t playerStartX = LEFT_COL_X + (LEFT_COL_W - playerBtnsWidth) / 2;
    for (int i = 0; i < 5; i++) {
        _playerButtons[i] = Rect(playerStartX + i * (SELECT_BTN_W + playerBtnGap), playerBtnY,
                                 SELECT_BTN_W, SELECT_BTN_H);
    }

    // Left column, Section 2: Starting life buttons (20, 25, 30, 40)
    const int16_t lifeBtnY = CONTENT_Y + SECTION_H + SECTION_GAP + SECTION_HEADER_H + 12;
    const int16_t lifeBtnW = 72;
    const int16_t lifeBtnGap = 12;
    const int16_t lifeBtnsWidth = 4 * lifeBtnW + 3 * lifeBtnGap;
    const int16_t lifeStartX = LEFT_COL_X + (LEFT_COL_W - lifeBtnsWidth) / 2;
    for (int i = 0; i < 4; i++) {
        _lifeButtons[i] =
            Rect(lifeStartX + i * (lifeBtnW + lifeBtnGap), lifeBtnY, lifeBtnW, SELECT_BTN_H);
    }

    // Right column: Reset buttons stacked
    const int16_t resetBtnY = CONTENT_Y + SECTION_HEADER_H + 12;
    const int16_t resetBtnW = RIGHT_COL_W - 24;  // Padding inside section
    const int16_t resetBtnX = RIGHT_COL_X + 12;
    _resetLifeButton = Rect(resetBtnX, resetBtnY, resetBtnW, RESET_BTN_H);
    _newGameButton = Rect(resetBtnX, resetBtnY + RESET_BTN_H + 12, resetBtnW, RESET_BTN_H);
}

void MTGSettingsScreen::onPlayerCountSelect(uint8_t count) {
    if (gameState().playerCount != count) {
        uint8_t oldCount = gameState().playerCount;
        gameState().playerCount = count;
        // Zero commander damage involving removed players
        if (count < oldCount) {
            for (uint8_t i = 0; i < GameState::MAX_PLAYERS; i++) {
                for (uint8_t j = 0; j < GameState::MAX_PLAYERS; j++) {
                    if (i >= count || j >= count) {
                        gameState().commanderDamage[i][j] = 0;
                    }
                }
            }
        }
        Preferences prefs;
        gameState().save(prefs);
        setNeedsFullRedraw(true);
    }
}

void MTGSettingsScreen::onStartingLifeSelect(int16_t life) {
    if (gameState().startingLife != life) {
        gameState().startingLife = life;
        Preferences prefs;
        gameState().save(prefs);
        setNeedsFullRedraw(true);
    }
}

void MTGSettingsScreen::onResetLifeTapped() {
    showConfirmDialog(false);
}

void MTGSettingsScreen::onNewGameTapped() {
    showConfirmDialog(true);
}

void MTGSettingsScreen::showConfirmDialog(bool isNewGame) {
    _showingConfirm = true;
    _confirmIsNewGame = isNewGame;
    setNeedsFullRedraw(true);
}

void MTGSettingsScreen::hideConfirmDialog() {
    _showingConfirm = false;
    setNeedsFullRedraw(true);
}

void MTGSettingsScreen::onConfirmAction() {
    if (_confirmIsNewGame) {
        // Reset everything to defaults
        gameState().reset();
    } else {
        // Just reset life totals
        gameState().resetLifeTotals();
    }
    Preferences prefs;
    gameState().save(prefs);
    hideConfirmDialog();
}

void MTGSettingsScreen::onHeaderFullRedraw(M5GFX* gfx) {
    // Left column - Section 1: Players
    drawSection(gfx, LEFT_COL_X, CONTENT_Y, LEFT_COL_W, SECTION_H, "PLAYERS");
    const uint8_t counts[] = {2, 3, 4, 5, 6};
    for (int i = 0; i < 5; i++) {
        char label[4];
        snprintf(label, sizeof(label), "%d", counts[i]);
        drawSelectButton(gfx, _playerButtons[i], label, counts[i] == gameState().playerCount);
    }

    // Left column - Section 2: Starting Life
    drawSection(gfx, LEFT_COL_X, CONTENT_Y + SECTION_H + SECTION_GAP, LEFT_COL_W, SECTION_H,
                "STARTING LIFE");
    const int16_t lifeTotals[] = {20, 25, 30, 40};
    for (int i = 0; i < 4; i++) {
        char label[4];
        snprintf(label, sizeof(label), "%d", lifeTotals[i]);
        drawSelectButton(gfx, _lifeButtons[i], label, lifeTotals[i] == gameState().startingLife);
    }

    // Right column - Reset Options section
    const int16_t resetSectionH = SECTION_H * 2 + SECTION_GAP;
    drawSection(gfx, RIGHT_COL_X, CONTENT_Y, RIGHT_COL_W, resetSectionH, "RESET OPTIONS");

    // Reset Life button
    {
        const Rect& r = _resetLifeButton;
        gfx->fillRect(r.x, r.y, r.w, r.h, TFT_WHITE);
        gfx->drawRect(r.x, r.y, r.w, r.h, TFT_BLACK);
        gfx->setTextColor(TFT_BLACK);
        gfx->setTextDatum(ML_DATUM);
        gfx->setTextSize(2);
        gfx->drawString("Reset Life", r.x + 16, r.y + r.h / 2 - 12);
        gfx->drawString("Reset to starting life", r.x + 16, r.y + r.h / 2 + 12);
    }

    // New Game button (double border)
    {
        const Rect& r = _newGameButton;
        gfx->fillRect(r.x, r.y, r.w, r.h, TFT_WHITE);
        gfx->drawRect(r.x, r.y, r.w, r.h, TFT_BLACK);
        gfx->drawRect(r.x + 2, r.y + 2, r.w - 4, r.h - 4, TFT_BLACK);  // Double border
        gfx->setTextColor(TFT_BLACK);
        gfx->setTextDatum(ML_DATUM);
        gfx->setTextSize(2);
        gfx->drawString("New Game", r.x + 16, r.y + r.h / 2 - 12);
        gfx->drawString("Reset everything", r.x + 16, r.y + r.h / 2 + 12);
    }

    // Draw confirm dialog if showing
    if (_showingConfirm) {
        drawConfirmDialog(gfx);
    }
}

void MTGSettingsScreen::drawSection(M5GFX* gfx, int16_t x, int16_t y, int16_t w, int16_t h,
                                    const char* title) {
    // Section border
    gfx->drawRect(x, y, w, h, TFT_BLACK);

    // Section header - black background with white text for contrast
    gfx->fillRect(x + 1, y + 1, w - 2, SECTION_HEADER_H - 2, TFT_BLACK);
    gfx->setTextColor(TFT_WHITE);
    gfx->setTextDatum(ML_DATUM);
    gfx->setTextSize(1);
    gfx->drawString(title, x + 12, y + SECTION_HEADER_H / 2);
}

void MTGSettingsScreen::drawSelectButton(M5GFX* gfx, const Rect& r, const char* label,
                                         bool selected) {
    if (selected) {
        gfx->fillRect(r.x, r.y, r.w, r.h, TFT_BLACK);
        gfx->setTextColor(TFT_WHITE);
    } else {
        gfx->fillRect(r.x, r.y, r.w, r.h, TFT_WHITE);
        gfx->drawRect(r.x, r.y, r.w, r.h, TFT_BLACK);
        gfx->setTextColor(TFT_BLACK);
    }
    gfx->setTextDatum(MC_DATUM);
    gfx->setTextSize(2);
    gfx->drawString(label, r.x + r.w / 2, r.y + r.h / 2);
}

void MTGSettingsScreen::drawConfirmDialog(M5GFX* gfx) {
    // Dim background
    // Note: On e-ink we can't really dim, so we'll draw a border
    int16_t dialogX = 200;
    int16_t dialogY = 180;
    int16_t dialogW = 560;
    int16_t dialogH = 180;

    // Dialog background
    gfx->fillRect(dialogX, dialogY, dialogW, dialogH, TFT_WHITE);
    gfx->drawRect(dialogX, dialogY, dialogW, dialogH, TFT_BLACK);
    gfx->drawRect(dialogX + 2, dialogY + 2, dialogW - 4, dialogH - 4, TFT_BLACK);

    // Dialog title
    gfx->setTextColor(TFT_BLACK);
    gfx->setTextDatum(MC_DATUM);
    gfx->setTextSize(2);
    const char* title = _confirmIsNewGame ? "Start New Game?" : "Reset Life Totals?";
    gfx->drawString(title, dialogX + dialogW / 2, dialogY + 45);

    // Dialog message
    gfx->setTextSize(2);
    gfx->setTextDatum(MC_DATUM);
    const char* msg =
        _confirmIsNewGame ? "Reset names and life totals." : "Reset life and commander damage?";
    gfx->drawString(msg, dialogX + dialogW / 2, dialogY + 90);

    // Position dialog buttons
    _confirmCancelButton = Rect(dialogX + 60, dialogY + 110, 200, 50);
    _confirmOkButton = Rect(dialogX + 300, dialogY + 110, 200, 50);

    // Draw buttons
    const Rect& cancelR = _confirmCancelButton;
    gfx->fillRect(cancelR.x, cancelR.y, cancelR.w, cancelR.h, TFT_WHITE);
    gfx->drawRect(cancelR.x, cancelR.y, cancelR.w, cancelR.h, TFT_BLACK);
    gfx->setTextColor(TFT_BLACK);
    gfx->setTextDatum(MC_DATUM);
    gfx->setTextSize(2);
    gfx->drawString("Cancel", cancelR.x + cancelR.w / 2, cancelR.y + cancelR.h / 2);

    const Rect& okR = _confirmOkButton;
    gfx->fillRect(okR.x, okR.y, okR.w, okR.h, TFT_BLACK);
    gfx->setTextColor(TFT_WHITE);
    gfx->setTextDatum(MC_DATUM);
    gfx->setTextSize(2);
    gfx->drawString("Confirm", okR.x + okR.w / 2, okR.y + okR.h / 2);
}

bool MTGSettingsScreen::onTouch(int16_t x, int16_t y, bool pressed, bool released) {
    // Confirm dialog takes priority
    if (_showingConfirm) {
        if (released && _confirmCancelButton.contains(x, y)) {
            Sound::click();
            hideConfirmDialog();
            return true;
        }
        if (released && _confirmOkButton.contains(x, y)) {
            Sound::click();
            onConfirmAction();
            return true;
        }
        // Block other touches when dialog is showing
        return pressed || released;
    }

    // Player count buttons
    const uint8_t counts[] = {2, 3, 4, 5, 6};
    for (int i = 0; i < 5; i++) {
        if (_playerButtons[i].contains(x, y)) {
            if (released) {
                Sound::click();
                onPlayerCountSelect(counts[i]);
            }
            return true;
        }
    }

    // Starting life buttons
    const int16_t lifeTotals[] = {20, 25, 30, 40};
    for (int i = 0; i < 4; i++) {
        if (_lifeButtons[i].contains(x, y)) {
            if (released) {
                Sound::click();
                onStartingLifeSelect(lifeTotals[i]);
            }
            return true;
        }
    }

    // Reset buttons
    if (_resetLifeButton.contains(x, y)) {
        if (released) {
            Sound::click();
            onResetLifeTapped();
        }
        return true;
    }

    if (_newGameButton.contains(x, y)) {
        if (released) {
            Sound::click();
            onNewGameTapped();
        }
        return true;
    }

    return false;
}
