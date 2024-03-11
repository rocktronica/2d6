#include <Arduboy2.h>
#include <ArduboyTones.h>
#include <Tinyfont.h>
#include "display.h"

# define MIN_DICE_PER_ROLL  1
# define DEFAULT_DICE_PER_ROLL  2
# define MAX_DICE_PER_ROLL  6

# define MIN_SIDES_PER_DIE  2
# define DEFAULT_SIDES_PER_DIE  6
# define MAX_SIDES_PER_DIE  20

Arduboy2 arduboy;
ArduboyTones sound(arduboy.audio.enabled);
Tinyfont tinyfont = Tinyfont(arduboy.sBuffer, WIDTH, HEIGHT);

GameState gameState = GameState::Title;

uint8_t framesRemaining;
MenuDie activeMenuDie = MenuDie::DicePerRoll;
Direction activeCaret = Direction::None;

uint8_t dicePerRoll = DEFAULT_DICE_PER_ROLL;
uint8_t sidesPerDie = DEFAULT_SIDES_PER_DIE;

uint8_t minSum;
uint8_t maxSum;
uint8_t uniqueSumsCount;
uint8_t currentRollValues[MAX_DICE_PER_ROLL];
int sumCounts[MAX_DICE_PER_ROLL * MAX_SIDES_PER_DIE];
int rollsCount;
uint32_t totalSum;

void roll(int count = 1) {
  while (count > 0) {
    for (uint8_t i = 0; i < dicePerRoll; i++) {
      currentRollValues[i] = random(1, sidesPerDie + 1);
    }

    sumCounts[getSum(currentRollValues, dicePerRoll) - minSum] += 1;
    totalSum += getSum(currentRollValues, dicePerRoll);

    count -= 1;
    rollsCount += 1;
  }

  framesRemaining = ROLL_FRAMES;
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

  minSum = dicePerRoll * 1;
  maxSum = dicePerRoll * sidesPerDie;
  uniqueSumsCount = maxSum - minSum + 1;
}

void setup() {
  arduboy.beginDoFirst();
  arduboy.waitNoButtons();

  arduboy.setFrameRate(15);

  reset();
}

void handleMenuEvents() {
  if (arduboy.everyXFrames(UPDATE_FRAMES)) {
    if (arduboy.pressed(LEFT_BUTTON)) {
      activeMenuDie = max(0, activeMenuDie - 1);
    } else if (arduboy.pressed(RIGHT_BUTTON)) {
      activeMenuDie = min(2, activeMenuDie + 1);
    }

    if (arduboy.pressed(UP_BUTTON)) {
      activeCaret = Direction::Up;
      framesRemaining = CARET_FRAMES;

      if (activeMenuDie == MenuDie::DicePerRoll) {
        dicePerRoll = min(dicePerRoll + 1, MAX_DICE_PER_ROLL);
      } else if (activeMenuDie == MenuDie::SidesPerDie) {
        sidesPerDie = min(sidesPerDie + 1, MAX_SIDES_PER_DIE);
      }
    } else if (arduboy.pressed(DOWN_BUTTON)) {
      activeCaret = Direction::Down;
      framesRemaining = CARET_FRAMES;

      if (activeMenuDie == MenuDie::DicePerRoll) {
        dicePerRoll = max(MIN_DICE_PER_ROLL, dicePerRoll - 1);
      } else if (activeMenuDie == MenuDie::SidesPerDie) {
        sidesPerDie = max(MIN_SIDES_PER_DIE, sidesPerDie - 1);
      }
    }
  }

  if (arduboy.justPressed(B_BUTTON)) {
    gameState = GameState::Operation;
    reset();
  }
}

void handleOperationEvents() {
  if (arduboy.justPressed(A_BUTTON)) {
    gameState = GameState::Title;
    framesRemaining = ROLL_FRAMES;
  }

  // TODO: prevent unintentional initial rolls after title
  if (arduboy.pressed(B_BUTTON)) {
    if (rollsCount == 0) {
        arduboy.initRandomSeed();
    }

    roll(arduboy.pressed(UP_BUTTON) ? 10 : 1);
  }
}

void loop() {
  if (!arduboy.nextFrame()) {
    return;
  }

  arduboy.pollButtons();
  arduboy.clear();

  framesRemaining = max(0, framesRemaining - 1);

  if (gameState == GameState::Title) {
    handleMenuEvents();

    drawMenu(
      dicePerRoll, sidesPerDie,
      activeMenuDie, activeCaret,
      arduboy, tinyfont
    );

    if (framesRemaining == 0) {
      activeCaret = Direction::None;
    }
  } else {
    handleOperationEvents();

    drawSidebar(
      0, 0,
      currentRollValues, dicePerRoll, sidesPerDie,
        "AVG:\n" + (rollsCount > 0 ? String(float(totalSum) / rollsCount) : "?")
        + "\n\nROLLS:\n" + String(rollsCount),
      framesRemaining > 0,
      arduboy, tinyfont
    );

    drawGraph(
      WIDTH - GRAPH_WIDTH, 0,
      sumCounts, minSum, maxSum, uniqueSumsCount,
      arduboy
    );
  }

  arduboy.display();
}
