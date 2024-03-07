#include <Arduboy2.h>
#include <ArduboyTones.h>
#include <Tinyfont.h>
#include "display.h"

# define ATTEMPTS_PER_UPDATE 10

Arduboy2 arduboy;
ArduboyTones sound(arduboy.audio.enabled);
Tinyfont tinyfont = Tinyfont(arduboy.sBuffer, WIDTH, HEIGHT);

uint8_t currentRollValues[2] = {0, 0};
int sumCounts[11] = {0,0,0,0,0,0,0,0,0,0,0};
int rollsCount = 0;

void update(int count = 1) {
  while (count > 0) {
    currentRollValues[0] = random(0, 6 + 1);
    currentRollValues[1] = random(0, 6 + 1);

    sumCounts[getSum(currentRollValues) - 2] += 1;

    count -= 1;
    rollsCount += 1;
  }
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
    update(ATTEMPTS_PER_UPDATE);
  }

  tinyfont.setCursor(0, 6 * 0);
  tinyfont.print(getRollText(currentRollValues));

  tinyfont.setCursor(0, 5 * 2);
  tinyfont.print("MAX:\n" + String(getMaxSumCount(sumCounts)));

  tinyfont.setCursor(0, 5 * 4);
  tinyfont.print("ROLLS:\n" + String(rollsCount));

  drawGraph(
    WIDTH - (11 * 8), 0,
    sumCounts,
    arduboy, tinyfont
  );

  arduboy.display();
}
