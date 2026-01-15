#include "MTGSettingsScreen.hpp"
#include <Arduino.h>
#include "../../app/ScreenManager.hpp"
#include "../../utils/Sound.hpp"

// Layout constants
static constexpr int16_t SCREEN_W = 960;
static constexpr int16_t HEADER_Y = Toolbar::HEIGHT;
static constexpr int16_t HEADER_H = 44;
static constexpr int16_t CONTENT_Y = HEADER_Y + HEADER_H + 12;
static constexpr int16_t MARGIN = 16;
// Two-column layout
static constexpr int16_t LEFT_COL_X = MARGIN;
static constexpr int16_t LEFT_COL_W = 460;
static constexpr int16_t RIGHT_COL_X = LEFT_COL_X + LEFT_COL_W + MARGIN;
static constexpr int16_t RIGHT_COL_W = SCREEN_W - RIGHT_COL_X - MARGIN;
// Section dimensions
static constexpr int16_t SECTION_H = 100;  // Extra padding at bottom
static constexpr int16_t SECTION_GAP = 12;
static constexpr int16_t SECTION_HEADER_H = 28;
// Button dimensions
static constexpr int16_t SELECT_BTN_W = 64;
static constexpr int16_t SELECT_BTN_H = 50;
static constexpr int16_t RESET_BTN_H = 70;

MTGSettingsScreen::MTGSettingsScreen(ScreenManager* manager) : _manager(manager) {
    _headerBar.setTitle("GAME SETTINGS");
    _headerBar.setLeftButton("< BACK", [this]() { _manager->goBack(); });
}

MTGSettingsScreen::~MTGSettingsScreen() {
    destroyButtons();
}

void MTGSettingsScreen::onEnter() {
    loadState();
    createButtons();
    setNeedsFullRedraw(true);
}

void MTGSettingsScreen::onExit() {
    saveState();
    destroyButtons();
}

void MTGSettingsScreen::loadState() {
    Preferences prefs;
    _gameState.load(prefs);
}

void MTGSettingsScreen::saveState() {
    Preferences prefs;
    _gameState.save(prefs);
}

void MTGSettingsScreen::createButtons() {
    destroyButtons();

    // Left column, Section 1: Player count buttons (2-6)
    const int16_t playerSectionY = CONTENT_Y;
    const int16_t playerBtnY = playerSectionY + SECTION_HEADER_H + 12;
    const int16_t playerBtnGap = 8;
    const int16_t playerBtnsWidth = 5 * SELECT_BTN_W + 4 * playerBtnGap;
    const int16_t playerStartX = LEFT_COL_X + (LEFT_COL_W - playerBtnsWidth) / 2;
    const uint8_t playerCounts[] = {2, 3, 4, 5, 6};
    for (int i = 0; i < 5; i++) {
        char label[4];
        snprintf(label, sizeof(label), "%d", playerCounts[i]);
        uint8_t count = playerCounts[i];
        _playerButtons[i] = new Button(Rect(playerStartX + i * (SELECT_BTN_W + playerBtnGap),
                                            playerBtnY, SELECT_BTN_W, SELECT_BTN_H),
                                       label, [this, count]() {
                                           Sound::click();
                                           onPlayerCountSelect(count);
                                       });
    }

    // Left column, Section 2: Starting life buttons (20, 25, 30, 40)
    const int16_t lifeSectionY = CONTENT_Y + SECTION_H + SECTION_GAP;
    const int16_t lifeBtnY = lifeSectionY + SECTION_HEADER_H + 12;
    const int16_t lifeBtnW = 72;
    const int16_t lifeBtnGap = 12;
    const int16_t lifeBtnsWidth = 4 * lifeBtnW + 3 * lifeBtnGap;
    const int16_t lifeStartX = LEFT_COL_X + (LEFT_COL_W - lifeBtnsWidth) / 2;
    const int16_t lifeTotals[] = {20, 25, 30, 40};
    for (int i = 0; i < 4; i++) {
        char label[4];
        snprintf(label, sizeof(label), "%d", lifeTotals[i]);
        int16_t life = lifeTotals[i];
        _lifeButtons[i] = new Button(
            Rect(lifeStartX + i * (lifeBtnW + lifeBtnGap), lifeBtnY, lifeBtnW, SELECT_BTN_H), label,
            [this, life]() {
                Sound::click();
                onStartingLifeSelect(life);
            });
    }

    // Right column: Reset buttons stacked
    const int16_t resetSectionY = CONTENT_Y;
    const int16_t resetBtnY = resetSectionY + SECTION_HEADER_H + 12;
    const int16_t resetBtnW = RIGHT_COL_W - 24;  // Padding inside section
    const int16_t resetBtnX = RIGHT_COL_X + 12;

    _resetLifeButton =
        new Button(Rect(resetBtnX, resetBtnY, resetBtnW, RESET_BTN_H), "Reset Life", [this]() {
            Sound::click();
            onResetLifeTapped();
        });

    _newGameButton =
        new Button(Rect(resetBtnX, resetBtnY + RESET_BTN_H + 12, resetBtnW, RESET_BTN_H),
                   "New Game", [this]() {
                       Sound::click();
                       onNewGameTapped();
                   });

    // Confirm dialog buttons (initially hidden, created when needed)
    _confirmCancelButton = new Button(Rect(280, 300, 160, 50), "Cancel", [this]() {
        Sound::click();
        hideConfirmDialog();
    });

    _confirmOkButton = new Button(Rect(520, 300, 160, 50), "Confirm", [this]() {
        Sound::click();
        onConfirmAction();
    });

    updatePlayerButtonStates();
    updateLifeButtonStates();
}

void MTGSettingsScreen::destroyButtons() {
    for (int i = 0; i < 5; i++) {
        delete _playerButtons[i];
        _playerButtons[i] = nullptr;
    }
    for (int i = 0; i < 4; i++) {
        delete _lifeButtons[i];
        _lifeButtons[i] = nullptr;
    }
    delete _resetLifeButton;
    _resetLifeButton = nullptr;
    delete _newGameButton;
    _newGameButton = nullptr;
    delete _confirmCancelButton;
    _confirmCancelButton = nullptr;
    delete _confirmOkButton;
    _confirmOkButton = nullptr;
}

void MTGSettingsScreen::updatePlayerButtonStates() {
    const uint8_t counts[] = {2, 3, 4, 5, 6};
    for (int i = 0; i < 5; i++) {
        if (_playerButtons[i]) {
            _playerButtons[i]->setDirty(true);
        }
    }
}

void MTGSettingsScreen::updateLifeButtonStates() {
    for (int i = 0; i < 4; i++) {
        if (_lifeButtons[i]) {
            _lifeButtons[i]->setDirty(true);
        }
    }
}

void MTGSettingsScreen::onPlayerCountSelect(uint8_t count) {
    if (_gameState.playerCount != count) {
        _gameState.playerCount = count;
        updatePlayerButtonStates();
        saveState();
        setNeedsFullRedraw(true);
    }
}

void MTGSettingsScreen::onStartingLifeSelect(int16_t life) {
    if (_gameState.startingLife != life) {
        _gameState.startingLife = life;
        updateLifeButtonStates();
        saveState();
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
        _gameState.reset();
    } else {
        // Just reset life totals
        _gameState.resetLifeTotals();
    }
    saveState();
    hideConfirmDialog();
}

void MTGSettingsScreen::update() {
    _toolbar.update();
}

void MTGSettingsScreen::draw(M5GFX* gfx) {
    bool needsDisplay = false;

    if (needsFullRedraw()) {
        gfx->fillScreen(TFT_WHITE);
        setNeedsFullRedraw(false);

        // Draw toolbar
        _toolbar.setDirty(true);
        _toolbar.draw(gfx);

        // Draw header bar
        _headerBar.draw(gfx);

        // Left column - Section 1: Players
        drawSection(gfx, LEFT_COL_X, CONTENT_Y, LEFT_COL_W, SECTION_H, "PLAYERS");

        // Draw player count buttons
        const uint8_t counts[] = {2, 3, 4, 5, 6};
        for (int i = 0; i < 5; i++) {
            if (_playerButtons[i]) {
                Rect r = _playerButtons[i]->getBounds();
                bool selected = (counts[i] == _gameState.playerCount);
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
                char label[4];
                snprintf(label, sizeof(label), "%d", counts[i]);
                gfx->drawString(label, r.x + r.w / 2, r.y + r.h / 2);
            }
        }

        // Left column - Section 2: Starting Life
        drawSection(gfx, LEFT_COL_X, CONTENT_Y + SECTION_H + SECTION_GAP, LEFT_COL_W, SECTION_H,
                    "STARTING LIFE");

        // Draw starting life buttons
        const int16_t lifeTotals[] = {20, 25, 30, 40};
        for (int i = 0; i < 4; i++) {
            if (_lifeButtons[i]) {
                Rect r = _lifeButtons[i]->getBounds();
                bool selected = (lifeTotals[i] == _gameState.startingLife);
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
                char label[4];
                snprintf(label, sizeof(label), "%d", lifeTotals[i]);
                gfx->drawString(label, r.x + r.w / 2, r.y + r.h / 2);
            }
        }

        // Right column - Reset Options section
        const int16_t resetSectionH = SECTION_H * 2 + SECTION_GAP;
        drawSection(gfx, RIGHT_COL_X, CONTENT_Y, RIGHT_COL_W, resetSectionH, "RESET OPTIONS");

        // Draw reset buttons
        if (_resetLifeButton) {
            Rect r = _resetLifeButton->getBounds();
            gfx->fillRect(r.x, r.y, r.w, r.h, TFT_WHITE);
            gfx->drawRect(r.x, r.y, r.w, r.h, TFT_BLACK);
            gfx->setTextColor(TFT_BLACK);
            gfx->setTextDatum(ML_DATUM);
            gfx->setTextSize(2);
            gfx->drawString("Reset Life", r.x + 16, r.y + r.h / 2 - 12);
            gfx->drawString("Reset to starting life", r.x + 16, r.y + r.h / 2 + 12);
        }

        if (_newGameButton) {
            Rect r = _newGameButton->getBounds();
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

        needsDisplay = true;
    }

    if (needsDisplay) {
        gfx->display();
    }

    _dirty = false;
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
        _confirmIsNewGame ? "Reset names and life totals." : "Reset all players to starting life.";
    gfx->drawString(msg, dialogX + dialogW / 2, dialogY + 90);

    // Update button positions for dialog
    _confirmCancelButton->setBounds(Rect(dialogX + 60, dialogY + 110, 200, 50));
    _confirmOkButton->setBounds(Rect(dialogX + 300, dialogY + 110, 200, 50));

    // Draw buttons
    Rect cancelR = _confirmCancelButton->getBounds();
    gfx->fillRect(cancelR.x, cancelR.y, cancelR.w, cancelR.h, TFT_WHITE);
    gfx->drawRect(cancelR.x, cancelR.y, cancelR.w, cancelR.h, TFT_BLACK);
    gfx->setTextColor(TFT_BLACK);
    gfx->setTextDatum(MC_DATUM);
    gfx->setTextSize(2);
    gfx->drawString("Cancel", cancelR.x + cancelR.w / 2, cancelR.y + cancelR.h / 2);

    Rect okR = _confirmOkButton->getBounds();
    gfx->fillRect(okR.x, okR.y, okR.w, okR.h, TFT_BLACK);
    gfx->setTextColor(TFT_WHITE);
    gfx->setTextDatum(MC_DATUM);
    gfx->setTextSize(2);
    gfx->drawString("Confirm", okR.x + okR.w / 2, okR.y + okR.h / 2);
}

bool MTGSettingsScreen::handleTouch(int16_t x, int16_t y, bool pressed, bool released) {
    // Confirm dialog takes priority
    if (_showingConfirm) {
        if (_confirmCancelButton && _confirmCancelButton->handleTouch(x, y, pressed, released)) {
            return true;
        }
        if (_confirmOkButton && _confirmOkButton->handleTouch(x, y, pressed, released)) {
            return true;
        }
        // Block other touches when dialog is showing
        return pressed || released;
    }

    // Header bar (back button)
    if (_headerBar.handleTouch(x, y, pressed, released)) {
        return true;
    }

    // Player count buttons
    const uint8_t counts[] = {2, 3, 4, 5, 6};
    for (int i = 0; i < 5; i++) {
        if (_playerButtons[i] && _playerButtons[i]->getBounds().contains(x, y)) {
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
        if (_lifeButtons[i] && _lifeButtons[i]->getBounds().contains(x, y)) {
            if (released) {
                Sound::click();
                onStartingLifeSelect(lifeTotals[i]);
            }
            return true;
        }
    }

    // Reset buttons
    if (_resetLifeButton && _resetLifeButton->getBounds().contains(x, y)) {
        if (released) {
            Sound::click();
            onResetLifeTapped();
        }
        return true;
    }

    if (_newGameButton && _newGameButton->getBounds().contains(x, y)) {
        if (released) {
            Sound::click();
            onNewGameTapped();
        }
        return true;
    }

    return false;
}
