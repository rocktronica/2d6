#include <Arduboy2.h>
#include <ArduboyTones.h>
#include <Tinyfont.h>
#include "display.h"
#include "noise.h"

# define DEFAULT_DICE_PER_ROLL  2
# define MAX_DICE_PER_ROLL  6

# define DEFAULT_SIDES_PER_DIE  6
# define MAX_SIDES_PER_DIE  20

Arduboy2 arduboy;
ArduboyTones arduboyTones(arduboy.audio.enabled);
Tinyfont tinyfont = Tinyfont(arduboy.sBuffer, WIDTH, HEIGHT);

struct SettingsState {
  uint8_t dicePerRoll = DEFAULT_DICE_PER_ROLL;
  uint8_t sidesPerDie = DEFAULT_SIDES_PER_DIE;

  Volume volume = Volume::Low;
} settings;

struct DisplayState {
  Dialog dialog = Dialog::Title;
  uint8_t framesRemaining = ROLL_FRAMES;
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

  // Start animation only if idle
  display.framesRemaining = display.framesRemaining > 0
    ? display.framesRemaining
    : ROLL_FRAMES;

  makeNoise(arduboyTones, CHANGE_TONES, settings.volume);
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
  makeNoise(arduboyTones, CHANGE_TONES, settings.volume);
}

uint8_t getDialogIndexWithSideEffects(
  uint8_t initialIndex,
  uint8_t minIndex,
  uint8_t maxIndex
) {
  bool up;

  if (arduboy.justPressed(UP_BUTTON)) {
    up = true;
  } else if (arduboy.justPressed(DOWN_BUTTON)) {
    up = false;
  } else {
    return initialIndex;
  }

  // The side effect(s):
  makeNoise(arduboyTones, JUMP_TONES, settings.volume);

  return up
    ? max(minIndex, initialIndex - 1)
    : min(initialIndex + 1, maxIndex);
}

void handleDialogNavigationEvents(
  Dialog maxDialog = Dialog::Operation
) {
  if (
    arduboy.justPressed(A_BUTTON) &&
    display.dialog != 0
  ) {
    display.dialog = display.dialog - 1;
  } else if (
    arduboy.justPressed(B_BUTTON) &&
    display.dialog != maxDialog
  ) {
    display.dialog = display.dialog + 1;
  } else {
    return;
  }

  makeNoise(arduboyTones, JUMP_TONES, settings.volume);
}

void handleOperationEvents() {
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

  if (display.dialog == Dialog::Title) {
    drawTitle(
      settings.dicePerRoll,
      settings.sidesPerDie,
      display.framesRemaining,
      arduboy, tinyfont
    );
  } else if (display.dialog == Dialog::SetSound) {
    const String options[3] = { "HIGH", "LOW", "NONE" };

    drawDialog(
      "SOUND",
      options, 3, settings.volume,
      arduboy, tinyfont
    );

    settings.volume = getDialogIndexWithSideEffects(
      settings.volume, Volume::High, Volume::Mute
    );

    if (arduboy.justPressed(B_BUTTON)) {
      arduboyTones.volumeMode(
        settings.volume == Volume::High
          ? VOLUME_ALWAYS_HIGH
          : VOLUME_ALWAYS_NORMAL
      );
    }
  } else if (display.dialog == Dialog::SetDicePerRoll) {
    const uint8_t options[6] = { 1, 2, 3, 4, 5, 6 };

    drawDialog(
      "DICE",
      options, 6, getIndexOfValue(settings.dicePerRoll, options, 6),
      arduboy, tinyfont
    );

    settings.dicePerRoll = options[getDialogIndexWithSideEffects(
      getIndexOfValue(settings.dicePerRoll, options, 6),
      0, 6 - 1
    )];
  } else if (display.dialog == Dialog::SetSidesPerDie) {
    const uint8_t options[6] = { 4, 6, 8, 10, 12, 20 };

    drawDialog(
      "SIDES",
      options, 6, getIndexOfValue(settings.sidesPerDie, options, 6),
      arduboy, tinyfont
    );

    settings.sidesPerDie = options[getDialogIndexWithSideEffects(
      getIndexOfValue(settings.sidesPerDie, options, 6),
      0, 6 - 1
    )];
  } else if (display.dialog == Operation) {
    if (arduboy.justPressed(A_BUTTON)) {
      reset(); // TODO: hold to reset w/ dialog
    }

    handleOperationEvents();

    drawSidebar(
      0, 0,

      operation.currentRollValues,
      settings.dicePerRoll,
      settings.sidesPerDie,

      + "NOW:" + String(getSum(operation.currentRollValues,
        settings.dicePerRoll))
      + "\nAVG:" + (operation.rollsCount > 0 ?
        getPrettyAverage(operation.totalSum, operation.rollsCount)
        : "?")
      + "\n\nCOUNT:\n" + String(operation.rollsCount),

      display.framesRemaining,

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

  handleDialogNavigationEvents();

  arduboy.display();
}
