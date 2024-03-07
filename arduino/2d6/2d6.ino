#include <Arduboy2.h>
#include <ArduboyTones.h>
#include <Tinyfont.h>
#include "display.h"

Arduboy2 arduboy;
ArduboyTones sound(arduboy.audio.enabled);
Tinyfont tinyfont = Tinyfont(arduboy.sBuffer, WIDTH, HEIGHT);

uint8_t currentRollValues[2] = {0, 0};
int pastRollValues[11] = {0,0,0,0,0,0,0,0,0,0,0};

void update() {
  currentRollValues[0] = random(0, 6 + 1);
  currentRollValues[1] = random(0, 6 + 1);

  pastRollValues[getSum(currentRollValues) - 2] += 1;
}

void setup() {
  arduboy.beginDoFirst();
  arduboy.waitNoButtons();

  arduboy.setFrameRate(15);

  update();
}

void loop() {
  if (!arduboy.nextFrame()) {
    return;
  }

  arduboy.pollButtons();
  arduboy.clear();

  if (arduboy.pressed(A_BUTTON)) {
    arduboy.initRandomSeed();
  }

  if (arduboy.pressed(A_BUTTON) || arduboy.pressed(B_BUTTON)) {
    update();
  }

  tinyfont.setCursor(1, 1);
  tinyfont.print("NOW:" + getRollText(currentRollValues));

  printRolls(1, 10, pastRollValues, tinyfont);

  arduboy.display();
}
