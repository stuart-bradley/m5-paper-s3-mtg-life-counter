#pragma once

namespace Sound {

void init();
void setEnabled(bool enabled);
bool isEnabled();

void click();     // Short click for button press
void lifeUp();    // Ascending tone for +life
void lifeDown();  // Descending tone for -life
void alert();     // Attention-getting tone

}  // namespace Sound
