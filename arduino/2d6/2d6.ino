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

struct SettingsState {
  Stage stage = Stage::Title;

  uint8_t dicePerRoll = DEFAULT_DICE_PER_ROLL;
  uint8_t sidesPerDie = DEFAULT_SIDES_PER_DIE;
} settings;

struct DisplayState {
  uint8_t framesRemaining;
  MenuDie activeMenuDie = MenuDie::DicePerRoll;
  Direction activeCaret = Direction::None;
  uint8_t selectionIndex = 0;
} display;

struct OperationState {
  uint8_t currentRollValues[MAX_DICE_PER_ROLL];
  int sumCounts[MAX_DICE_PER_ROLL * MAX_SIDES_PER_DIE];
  int rollsCount;
  uint32_t totalSum;
} operation;

uint8_t getMinSum() {
  return settings.dicePerRoll * 1;
}

uint8_t getMaxSum() {
  return settings.dicePerRoll * settings.sidesPerDie;
}

uint8_t getUniqueSumsCount() {
  return getMaxSum() - getMinSum() + 1;
}

void roll(int count = 1) {
  while (count > 0) {
    for (uint8_t i = 0; i < settings.dicePerRoll; i++) {
      operation.currentRollValues[i] =
        random(1, settings.sidesPerDie + 1);
    }

    operation.sumCounts[
      getSum(operation.currentRollValues, settings.dicePerRoll)
      - getMinSum()
    ] += 1;
    operation.totalSum +=
      getSum(operation.currentRollValues, settings.dicePerRoll);

    count -= 1;
    operation.rollsCount += 1;
  }

  display.framesRemaining = ROLL_FRAMES;
}

void reset() {
  for (uint8_t i = 0; i < settings.dicePerRoll; i++) {
    operation.currentRollValues[i] = 0;
  }
  for (uint8_t i = 0; i < getUniqueSumsCount(); i++) {
    operation.sumCounts[i] = 0;
  }

  operation.rollsCount = 0;
  operation.totalSum = 0;
}

void setup() {
  arduboy.beginDoFirst();
  arduboy.waitNoButtons();

  arduboy.setFrameRate(15);

  reset();
}

uint8_t getDialogValueWithSideEffects(
  uint8_t initialValue,
  uint8_t minValue,
  uint8_t maxValue
) {
  Direction direction = Direction::None;

  if (arduboy.justPressed(UP_BUTTON)) {
    direction = Direction::Up;
  } else if (arduboy.justPressed(DOWN_BUTTON)) {
    direction = Direction::Down;
  } else {
    return initialValue;
  }

  // The side effects:
  display.activeCaret = direction;
  display.framesRemaining = CARET_FRAMES;

  return direction == Direction::Up
    ? min(initialValue + 1, maxValue)
    : max(minValue, initialValue - 1);
}

void handleMenuEvents() {
  if (arduboy.justPressed(LEFT_BUTTON)) {
    display.activeMenuDie = max(0, display.activeMenuDie - 1);
  } else if (arduboy.justPressed(RIGHT_BUTTON)) {
    display.activeMenuDie = min(2, display.activeMenuDie + 1);
  }

  if (arduboy.justPressed(B_BUTTON)) {
    settings.stage = Stage::Operation;
    reset();
  }

  if (display.activeMenuDie == MenuDie::DicePerRoll) {
    settings.dicePerRoll = getDialogValueWithSideEffects(
      settings.dicePerRoll,
      MIN_DICE_PER_ROLL,
      MAX_DICE_PER_ROLL
    );
  } else if (display.activeMenuDie == MenuDie::SidesPerDie) {
    settings.sidesPerDie = getDialogValueWithSideEffects(
      settings.sidesPerDie,
      MIN_SIDES_PER_DIE,
      MAX_SIDES_PER_DIE
    );
  }
}

void handleOperationEvents() {
  if (arduboy.justPressed(A_BUTTON)) {
    settings.stage = Stage::Title;
    display.framesRemaining = ROLL_FRAMES;
  }

  if (arduboy.justPressed(B_BUTTON)) {
    if (operation.rollsCount == 0) {
        arduboy.initRandomSeed();
    }
    roll();
  }

  if (
    arduboy.pressed(B_BUTTON) &&
    operation.rollsCount > 0
  ) {
    roll(arduboy.pressed(UP_BUTTON) ? 10 : 1);
  }
}

void loop() {
  if (!arduboy.nextFrame()) {
    return;
  }

  arduboy.pollButtons();
  arduboy.clear();

  display.framesRemaining =
    max(0, display.framesRemaining - 1);

  if (settings.stage == Stage::Title) {
    handleMenuEvents();

    drawMenu(
      settings.dicePerRoll, settings.sidesPerDie,
      display.activeMenuDie, display.activeCaret,
      arduboy, tinyfont
    );

    if (display.framesRemaining == 0) {
      display.activeCaret = Direction::None;
    }
  } else {
    handleOperationEvents();

    drawSidebar(
      0, 0,

      operation.currentRollValues,
      settings.dicePerRoll,
      settings.sidesPerDie,

      "NOW:" + String(getSum(operation.currentRollValues,
        settings.dicePerRoll))
      + "\nAVG:" + (operation.rollsCount > 0 ?
        getPrettyAverage(operation.totalSum, operation.rollsCount)
        : "?")
      + "\n\nCOUNT:\n" + String(operation.rollsCount),

      display.framesRemaining > 0,

      arduboy, tinyfont
    );

    drawGraph(
      WIDTH - GRAPH_WIDTH, 0,
      operation.sumCounts,
      getMinSum(),
      getMaxSum(),
      getUniqueSumsCount(),
      arduboy
    );
  }

  arduboy.display();
}
