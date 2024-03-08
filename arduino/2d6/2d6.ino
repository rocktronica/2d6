#include <Arduboy2.h>
#include <ArduboyTones.h>
#include <Tinyfont.h>
#include "display.h"

# define ATTEMPTS_PER_UPDATE 10
# define DICE_PER_ROLL      2
# define SIDES_PER_DIE      6

const uint8_t MIN_SUM = DICE_PER_ROLL * 1;
const uint8_t MAX_SUM = DICE_PER_ROLL * SIDES_PER_DIE;
const uint8_t UNIQUE_SUMS_COUNT = MAX_SUM - MIN_SUM + 1;
const uint8_t GRAPH_BAR_WIDTH = floor((WIDTH - 40) / UNIQUE_SUMS_COUNT);

Arduboy2 arduboy;
ArduboyTones sound(arduboy.audio.enabled);
Tinyfont tinyfont = Tinyfont(arduboy.sBuffer, WIDTH, HEIGHT);

uint8_t currentRollValues[DICE_PER_ROLL];
int sumCounts[UNIQUE_SUMS_COUNT];
int rollsCount = 0;

void update(int count = 1) {
  while (count > 0) {
    for (uint8_t i = 0; i < DICE_PER_ROLL; i++) {
      currentRollValues[i] = random(0, SIDES_PER_DIE + 1);
    }
    sumCounts[getSum(currentRollValues, DICE_PER_ROLL) - MIN_SUM] += 1;

    count -= 1;
    rollsCount += 1;
  }
}

void setup() {
  arduboy.beginDoFirst();
  arduboy.waitNoButtons();

  arduboy.setFrameRate(15);

  for (uint8_t i = 0; i < DICE_PER_ROLL; i++) {
    currentRollValues[i] = 0;
  }
  for (uint8_t i = 0; i < UNIQUE_SUMS_COUNT; i++) {
    sumCounts[i] = 0;
  }
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
  tinyfont.print(getRollText(currentRollValues, DICE_PER_ROLL));

  tinyfont.setCursor(0, 5 * 2);
  tinyfont.print("MAX:\n" + String(getMaxSumCount(sumCounts, UNIQUE_SUMS_COUNT)));

  tinyfont.setCursor(0, 5 * 4);
  tinyfont.print("ROLLS:\n" + String(rollsCount));

  drawGraph(
    WIDTH - UNIQUE_SUMS_COUNT * GRAPH_BAR_WIDTH, 0,
    sumCounts, MIN_SUM, MAX_SUM, UNIQUE_SUMS_COUNT,
    arduboy, tinyfont,
    UNIQUE_SUMS_COUNT * GRAPH_BAR_WIDTH, HEIGHT
  );

  arduboy.display();
}
