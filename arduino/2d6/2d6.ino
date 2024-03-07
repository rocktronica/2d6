#include <Arduboy2.h>
#include <ArduboyTones.h>
#include <Tinyfont.h>
#include "display.h"

Arduboy2 arduboy;
ArduboyTones sound(arduboy.audio.enabled);
Tinyfont tinyfont = Tinyfont(arduboy.sBuffer, WIDTH, HEIGHT);

uint8_t currentRollValues[2] = {0, 0};
int sumCounts[11] = {0,0,0,0,0,0,0,0,0,0,0};

void update() {
  currentRollValues[0] = random(0, 6 + 1);
  currentRollValues[1] = random(0, 6 + 1);

  sumCounts[getSum(currentRollValues) - 2] += 1;
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

  tinyfont.setCursor(0, 0);
  tinyfont.print(getRollText(currentRollValues));

  tinyfont.setCursor(0, 6);
  tinyfont.print(getMaxSumCount(sumCounts));

  drawGraph(
    WIDTH - (11 * 8), 0,
    sumCounts,
    arduboy, tinyfont
  );

  arduboy.display();
}
