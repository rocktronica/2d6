#include <Arduboy2.h>
#include <ArduboyTones.h>
#include <Tinyfont.h>
#include "display.h"

# define MAX_DICE_PER_ROLL  6

Arduboy2 arduboy;
ArduboyTones sound(arduboy.audio.enabled);
Tinyfont tinyfont = Tinyfont(arduboy.sBuffer, WIDTH, HEIGHT);

GameState gameState = GameState::Title;
uint8_t animationFramesRemaining = ANIMATION_FRAMES;

const uint8_t dicePerRoll = min(2, MAX_DICE_PER_ROLL);
const uint8_t sidesPerDie = 6;

const uint8_t minSum = dicePerRoll * 1;
const uint8_t maxSum = dicePerRoll * sidesPerDie;
const uint8_t uniqueSumsCount = maxSum - minSum + 1;

// TODO: prevent/error on overflow
uint8_t currentRollValues[dicePerRoll];
int sumCounts[uniqueSumsCount];
int rollsCount = 0;
uint32_t totalSum = 0;

void update(int count = 1) {
  while (count > 0) {
    for (uint8_t i = 0; i < dicePerRoll; i++) {
      currentRollValues[i] = random(1, sidesPerDie + 1);
    }

    sumCounts[getSum(currentRollValues, dicePerRoll) - minSum] += 1;
    totalSum += getSum(currentRollValues, dicePerRoll);

    count -= 1;
    rollsCount += 1;
  }

  animationFramesRemaining = ANIMATION_FRAMES;
}

void reset() {
  for (uint8_t i = 0; i < dicePerRoll; i++) {
    currentRollValues[i] = 0;
  }
  for (uint8_t i = 0; i < uniqueSumsCount; i++) {
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

  animationFramesRemaining = max(0, animationFramesRemaining - 1);

  if (gameState == GameState::Title) {
    drawTitle(
      dicePerRoll, sidesPerDie,
      animationFramesRemaining > 0,
      arduboy, tinyfont
    );

    if (arduboy.justPressed(B_BUTTON)) {
      gameState = GameState::Operation;
      reset();
    }
  } else {
    drawSidebar(
      0, 0,
      currentRollValues, dicePerRoll, sidesPerDie,
        "AVG:\n" + (rollsCount > 0 ? String(float(totalSum) / rollsCount) : "?")
        + "\n\nROLLS:\n" + String(rollsCount),
      animationFramesRemaining > 0,
      arduboy, tinyfont
    );

    drawGraph(
      WIDTH - GRAPH_WIDTH, 0,
      sumCounts, minSum, maxSum, uniqueSumsCount,
      arduboy
    );

    if (arduboy.justPressed(A_BUTTON)) {
      gameState = GameState::Title;
      animationFramesRemaining = ANIMATION_FRAMES;
    }

    // TODO: prevent unintentional initial updates after title
    if (arduboy.pressed(B_BUTTON)) {
      if (rollsCount == 0) {
          arduboy.initRandomSeed();
      }

      update(arduboy.pressed(UP_BUTTON) ? 10 : 1);
    }
  }

  arduboy.display();
}
