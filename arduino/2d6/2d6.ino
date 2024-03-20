#include <Arduboy2.h>
#include <ArduboyTones.h>
#include <Tinyfont.h>
#include "display.h"
#include "noise.h"

# define MAX_COUNT              1000

# define DEFAULT_DICE_PER_ROLL  2
# define MAX_DICE_PER_ROLL      12

# define DEFAULT_SIDES_PER_DIE  6
# define MAX_SIDES_PER_DIE      20

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

  int8_t titleDieIndex = -1;
  int8_t titleFramesRemaining = TITLE_FRAMES;

  int8_t rollFramesRemaining[MAX_DICE_PER_ROLL];
  bool rollClockwise[MAX_DICE_PER_ROLL];

  int8_t framesRollButtonHeld = 0;
} display;

struct OperationState {
  uint8_t currentRollValues[MAX_DICE_PER_ROLL];
  uint16_t sumCounts[MAX_DICE_PER_ROLL * MAX_SIDES_PER_DIE];
  uint16_t rollsCount;
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
  if (operation.rollsCount >= MAX_COUNT) {
    return;
  }

  while (count > 0 && operation.rollsCount < MAX_COUNT) {
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

  for (uint8_t i = 0; i < MAX_DICE_PER_ROLL; i++) {
    // Start new animations only if idle
    if (display.rollFramesRemaining[i] == 0) {
      display.rollFramesRemaining[i] = getRollFramesCount(ROLL_FRAMES);
      display.rollClockwise[i] = random(0, 2) == 0;
    }
  }

  if (
    display.framesRollButtonHeld == 0 ||
    arduboy.everyXFrames(ROLL_FRAMES)
  ) {
    makeNoise(arduboyTones, CHANGE_TONES, settings.volume);
  }
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

  arduboy.setFrameRate(FRAMES_PER_SECOND);

  reset();
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
  Dialog* dialog,
  Dialog maxDialog = Dialog::Operation
) {
  if (arduboy.justPressed(A_BUTTON) && *dialog != 0) {
    *dialog = *dialog - 1;
  } else if (arduboy.justPressed(B_BUTTON) && *dialog != maxDialog) {
    *dialog = *dialog + 1;
  } else {
    return;
  }

  if (*dialog == Dialog::Title) {
    display.titleDieIndex = -1;
    display.titleFramesRemaining = TITLE_FRAMES;
  } else {
    makeNoise(arduboyTones, JUMP_TONES, settings.volume);
  }
}

void handleOperationEvents() {
  if (arduboy.justPressed(B_BUTTON)) {
    if (operation.rollsCount == 0) {
        arduboy.initRandomSeed();
    }
    roll();
  }

  if (arduboy.pressed(B_BUTTON)) {
    display.framesRollButtonHeld =
      min(FRAMES_PER_SECOND, display.framesRollButtonHeld + 1);
  } else {
    display.framesRollButtonHeld = 0;
  }

  if (
    arduboy.pressed(B_BUTTON) &&
    display.framesRollButtonHeld >= FRAMES_PER_SECOND
  ) {
    roll(arduboy.pressed(UP_BUTTON) ? 10 : 1);
  }
}

void handleTitleDieIncrementing(uint8_t targetFrame, uint8_t index) {
  uint8_t framesElapsed = TITLE_FRAMES - display.titleFramesRemaining;

  if (framesElapsed == targetFrame) {
    display.rollFramesRemaining[index] = ROLL_FRAMES;
    display.rollClockwise[index] = random(0, 2) == 0;
    display.titleDieIndex = index;
    makeNoise(arduboyTones, CHANGE_TONES, settings.volume);
  }
}

void loop() {
  if (!arduboy.nextFrame()) {
    return;
  }

  arduboy.pollButtons();
  arduboy.clear();

  for (uint8_t i = 0; i < MAX_DICE_PER_ROLL; i++) {
    display.rollFramesRemaining[i] = max(0, display.rollFramesRemaining[i] - 1);
  }

  if (display.dialog == Dialog::Title) {
    drawTitle(
      settings.dicePerRoll,
      settings.sidesPerDie,
      display.titleDieIndex,
      display.rollFramesRemaining,
      display.rollClockwise,
      arduboy, tinyfont
    );

    display.titleFramesRemaining =
      max(0, display.titleFramesRemaining - 1);

    handleTitleDieIncrementing(TITLE_ROLL_0_FRAME, 0);
    handleTitleDieIncrementing(TITLE_ROLL_1_FRAME, 1);
    handleTitleDieIncrementing(TITLE_ROLL_2_FRAME, 2);

    if (display.titleFramesRemaining == 0) {
      display.dialog = display.dialog + 1;
    }
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

    if (arduboy.justPressed(A_BUTTON) || arduboy.justPressed(B_BUTTON)) {
      arduboyTones.volumeMode(
        settings.volume == Volume::High
          ? VOLUME_ALWAYS_HIGH
          : VOLUME_ALWAYS_NORMAL
      );
    }
  } else if (display.dialog == Dialog::SetDicePerRoll) {
    const uint8_t options[8] = { 1, 2, 3, 4, 5, 6, 9, MAX_DICE_PER_ROLL };

    drawDialog(
      "DICE",
      options, 8, getIndexOfValue(settings.dicePerRoll, options, 8),
      arduboy, tinyfont
    );

    settings.dicePerRoll = options[getDialogIndexWithSideEffects(
      getIndexOfValue(settings.dicePerRoll, options, 8),
      0, 8 - 1
    )];
  } else if (display.dialog == Dialog::SetSidesPerDie) {
    const uint8_t options[7] = { 2, 4, 6, 8, 10, 12, MAX_SIDES_PER_DIE };

    drawDialog(
      "SIDES",
      options, 7, getIndexOfValue(settings.sidesPerDie, options, 7),
      arduboy, tinyfont
    );

    settings.sidesPerDie = options[getDialogIndexWithSideEffects(
      getIndexOfValue(settings.sidesPerDie, options, 7),
      0, 7 - 1
    )];
  } else if (display.dialog == Operation) {
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

      display.rollFramesRemaining,
      display.rollClockwise,

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

    if (arduboy.justPressed(A_BUTTON)) {
      reset(); // TODO: hold to reset w/ dialog
    }

    if (operation.rollsCount >= MAX_COUNT) {
      drawLimitErrorPanel(arduboy, tinyfont);
    }
  }

  handleDialogNavigationEvents(&display.dialog);

  arduboy.display();
}
