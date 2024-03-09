#include <Arduboy2.h>
#include <ArduboyTones.h>
#include <Tinyfont.h>
#include "display.h"

# define DICE_PER_ROLL      2
# define SIDES_PER_DIE      6
# define ANIMATION_FRAMES   5

const uint8_t MIN_SUM = DICE_PER_ROLL * 1;
const uint8_t MAX_SUM = DICE_PER_ROLL * SIDES_PER_DIE;
const uint8_t UNIQUE_SUMS_COUNT = MAX_SUM - MIN_SUM + 1;
const uint8_t GRAPH_MAX_WIDTH = WIDTH - SIDEBAR_MIN_WIDTH;

Arduboy2 arduboy;
ArduboyTones sound(arduboy.audio.enabled);
Tinyfont tinyfont = Tinyfont(arduboy.sBuffer, WIDTH, HEIGHT);

// TODO: prevent/error on overflow
uint8_t currentRollValues[DICE_PER_ROLL];
int sumCounts[UNIQUE_SUMS_COUNT];
int rollsCount = 0;
uint32_t totalSum = 0;

uint8_t animationFramesRemaining = 0;

void update(int count = 1) {
  while (count > 0) {
    for (uint8_t i = 0; i < DICE_PER_ROLL; i++) {
      currentRollValues[i] = random(1, SIDES_PER_DIE + 1);
    }

    sumCounts[getSum(currentRollValues, DICE_PER_ROLL) - MIN_SUM] += 1;
    totalSum += getSum(currentRollValues, DICE_PER_ROLL);

    count -= 1;
    rollsCount += 1;
  }
}

void reset() {
  for (uint8_t i = 0; i < DICE_PER_ROLL; i++) {
    currentRollValues[i] = 0;
  }
  for (uint8_t i = 0; i < UNIQUE_SUMS_COUNT; i++) {
    sumCounts[i] = 0;
  }

  rollsCount = 0;
  totalSum = 0;
}

void setup() {
  arduboy.beginDoFirst();
  arduboy.waitNoButtons();

  arduboy.setFrameRate(15);

  reset();
}

void loop() {
  if (!arduboy.nextFrame()) {
    return;
  }

  arduboy.pollButtons();
  arduboy.clear();

  if (arduboy.pressed(A_BUTTON)) {
    reset();
  }

  if (arduboy.pressed(B_BUTTON)) {
    if (rollsCount == 0) {
        arduboy.initRandomSeed();
    }

    update(arduboy.pressed(UP_BUTTON) ? 10 : 1);
    animationFramesRemaining = ANIMATION_FRAMES;
  }

  // TODO: fix arbitrarily. fluid width isn't worth baggage
  const uint8_t graphWidth = getIdealGraphWidth(GRAPH_MAX_WIDTH, UNIQUE_SUMS_COUNT);
  const uint8_t sidebarWidth = WIDTH - graphWidth - FRAME_GAP;

  animationFramesRemaining = max(0, animationFramesRemaining - 1);
  drawSidebar(
    0, 0,
    currentRollValues, DICE_PER_ROLL, SIDES_PER_DIE,
      "AVG:\n" + (rollsCount > 0 ? String(float(totalSum) / rollsCount) : "?")
      + "\n\nROLLS:\n" + String(rollsCount),
    animationFramesRemaining > 0,
    sidebarWidth, HEIGHT,
    arduboy, tinyfont
  );

  drawGraph(
    WIDTH - graphWidth, 0,
    sumCounts, MIN_SUM, MAX_SUM, UNIQUE_SUMS_COUNT,
    arduboy,
    graphWidth, HEIGHT
  );

  arduboy.display();
}
