#ifndef graph_h
#define graph_h

#include "FlashStringHelper.h"
#include "utils.h"

# define SIDEBAR_WIDTH      45
# define GRAPH_WIDTH        (WIDTH - SIDEBAR_WIDTH - GAP)

# define OUTER_FILLET       3
# define INNER_FILLET       1

# define FRAME              1
# define GAP                1
# define DIALOG_GAP         3

# define DIE_CHAR_WIDTH     5    // Default Arduboy font
# define DIE_CHAR_HEIGHT    7    // Default Arduboy font
# define TINY_TEXT_SIZE     4    // TinyFont 4x4

# define FRAMES_PER_SECOND  30

# define ROLL_FRAMES        6
# define DECONSTRUCT_FRAMES 15

# define TITLE_FRAMES       60
# define TITLE_ROLL_0_FRAME 6
# define TITLE_ROLL_1_FRAME 8
# define TITLE_ROLL_2_FRAME 12

# define SCROLL_FRAMES      6
# define GRAPH_PANEL_Y      0
# define DEBUG_PANEL_Y      -(HEIGHT + GAP)

# define DEBUG_MAX_ROWS     6
# define DEBUG_COLS         2

# define CARET_SIZE         2

const char ROLL_CHAR0[] PROGMEM = "!";
const char ROLL_CHAR1[] PROGMEM = "@";
const char ROLL_CHAR2[] PROGMEM = "#";
const char ROLL_CHAR3[] PROGMEM = "$";
const char ROLL_CHAR4[] PROGMEM = "%";
const char ROLL_CHAR5[] PROGMEM = "^";
const char ROLL_CHAR6[] PROGMEM = "&";
const char ROLL_CHAR7[] PROGMEM = "*";

const char * const ROLL_CHARS[] PROGMEM {
  ROLL_CHAR0,
  ROLL_CHAR1,
  ROLL_CHAR2,
  ROLL_CHAR3,
  ROLL_CHAR4,
  ROLL_CHAR5,
  ROLL_CHAR6,
  ROLL_CHAR7,
};

const char TITLE_STRING0[] PROGMEM = "DICE ROLL";
const char TITLE_STRING1[] PROGMEM = "DISTRIBUTION";
const char TITLE_STRING2[] PROGMEM = "VISUALIZER";

const char * const TITLE_STRINGS[] PROGMEM {
  TITLE_STRING0,
  TITLE_STRING1,
  TITLE_STRING2,
};

const char CREDITS_STRING0[] PROGMEM = "MADE BY";
const char CREDITS_STRING1[] PROGMEM = "@ROCKTRONICA";
const char CREDITS_STRING2[] PROGMEM = "who knows why";
const char CREDITS_STRING3[] PROGMEM = "2024";

const char * const CREDITS_STRINGS[] PROGMEM {
  CREDITS_STRING0,
  CREDITS_STRING1,
  CREDITS_STRING2,
  CREDITS_STRING3,
};

const char LIMIT_ERROR_STRING0[] PROGMEM = "ERROR";
const char LIMIT_ERROR_STRING1[] PROGMEM = "LIMIT";
const char LIMIT_ERROR_STRING2[] PROGMEM = "REACHED";

const char * const LIMIT_ERROR_STRINGS[] PROGMEM {
  LIMIT_ERROR_STRING0,
  LIMIT_ERROR_STRING1,
  LIMIT_ERROR_STRING2,
};

const char DEBUG_STRING0[] PROGMEM = "DEBUG";
const char DEBUG_STRING1[] PROGMEM = "AVERAGE:";
const char DEBUG_STRING2[] PROGMEM = "ROLLS:  ";
const char DEBUG_STRING3[] PROGMEM = "MIN:";
const char DEBUG_STRING4[] PROGMEM = "  MAX:";

const char * const DEBUG_STRINGS[] PROGMEM {
  DEBUG_STRING0,
  DEBUG_STRING1,
  DEBUG_STRING2,
  DEBUG_STRING3,
  DEBUG_STRING4,
};

enum Stage {
  Credits,
  Title,
  SetSound,
  SetDicePerRoll,
  SetSidesPerDie,
  Operation
};

enum Direction {
  None,
  Up,
  Down
};

Xy getPanelTextXy(
  int8_t x,
  int8_t y,

  uint8_t panelWidth,
  uint8_t panelHeight,

  uint8_t lineIndex
) {
  return {
    x: x + (FRAME + DIALOG_GAP),
    y: y + FRAME * 2 + GAP * 2
      + TINY_TEXT_SIZE + DIALOG_GAP
      + (TINY_TEXT_SIZE + GAP) * lineIndex
  };
}

Xy getPanelTextXy(
  uint8_t panelWidth,
  uint8_t panelHeight,
  uint8_t lineIndex = 0
) {
  return getPanelTextXy(
    (WIDTH - panelWidth) / 2, (HEIGHT - panelHeight) / 2,
    panelWidth, panelHeight,
    lineIndex
  );
}

void drawCaret(
  int8_t x,
  int8_t y,

  Direction direction,

  Arduboy2 arduboy
) {
  if (direction == Direction::Up) {
    y += CARET_SIZE / 2;
  }

  arduboy.fillTriangle(
    x, y,
    x + CARET_SIZE, y,
    x + CARET_SIZE / 2, y + CARET_SIZE / (direction == Direction::Up ? -2 : 2)
  );
}

void drawPanel(
  int8_t x,
  int8_t y,

  uint8_t width,
  uint8_t height,

  String title,

  Arduboy2 arduboy,
  Tinyfont tinyfont,

  bool shadow = false,
  Direction arrow = Direction::None
) {
  if (shadow) {
    arduboy.fillRoundRect(
      x - 1,
      y - 1,
      width + 2,
      height + 2,
      OUTER_FILLET,
      BLACK
    );
  }

  arduboy.drawRoundRect(x, y, width, height, OUTER_FILLET);

  arduboy.drawFastHLine(x, y + TINY_TEXT_SIZE + FRAME + GAP + GAP, width);
  tinyfont.setCursor(
    x + (width - (TINY_TEXT_SIZE * title.length() + (title.length() - 1))) / 2,
    y + (FRAME + GAP)
  );
  tinyfont.print(title);

  if (arrow != Direction::None) {
    drawCaret(
      x + width - CARET_SIZE - 5,   // eyeballed!
      y + 3,                        // eyeballed!
      arrow,
      arduboy
    );
  }
}

uint8_t getMinPanelWidth(uint8_t chars) {
  return (FRAME + DIALOG_GAP) * 2
      + chars * TINY_TEXT_SIZE + (chars - 1) * GAP;
}

uint8_t getMinPanelHeight(uint8_t linesCount) {
  return (FRAME + GAP) * 2 + TINY_TEXT_SIZE
    + DIALOG_GAP * 2 + FRAME
    + TINY_TEXT_SIZE * linesCount
    + GAP * (linesCount - 1);
}

uint8_t getMinDialogWidth(
  String title,
  String options[],
  uint8_t optionsSize
) {
  uint8_t maxChars = title.length() + 2;

  for (uint8_t i = 0; i < optionsSize; i++) {
    maxChars = max(maxChars, options[i].length() + 2);
  }

  return getMinPanelWidth(maxChars);
}

void drawLimitErrorPanel(
  Arduboy2 arduboy,
  Tinyfont tinyfont
) {
  uint8_t width = getMinPanelWidth(7);
  uint8_t height = getMinPanelHeight(2);

  drawPanel(
    (WIDTH - width) / 2, (HEIGHT - height) / 2,
    width, height,
    readFlashStringPointer(&LIMIT_ERROR_STRINGS[0]),
    arduboy, tinyfont,
    true
  );

  String lines[] = {
    readFlashStringPointer(&LIMIT_ERROR_STRINGS[1]),
    readFlashStringPointer(&LIMIT_ERROR_STRINGS[2]),
  };
  for (uint8_t i = 0; i < 2; i++) {
    Xy xy = getPanelTextXy(width, height, i);
    tinyfont.setCursor(xy.x, xy.y);
    tinyfont.print(lines[i]);
  }
}

void drawDialog(
  uint8_t width,
  uint8_t height,

  String title,

  String options[],
  uint8_t optionsSize,
  uint8_t selectedOptionIndex,

  Arduboy2 arduboy,
  Tinyfont tinyfont
) {
  drawPanel(
    (WIDTH - width) / 2, (HEIGHT - height) / 2,
    width, height,
    title,
    arduboy, tinyfont
  );

  for (uint8_t i = 0; i < optionsSize; i++) {
    Xy xy = getPanelTextXy(width, height, i);
    tinyfont.setCursor(xy.x, xy.y);
    tinyfont.print(
      (selectedOptionIndex == i ? ">" : " ") + options[i]
    );
  }
}

void drawDialog(
  String title,

  String options[],
  uint8_t optionsSize,
  uint8_t selectedOptionIndex,

  Arduboy2 arduboy,
  Tinyfont tinyfont
) {
  drawDialog(
    getMinDialogWidth(title, options, optionsSize),
    getMinPanelHeight(optionsSize),
    title,
    options, optionsSize, selectedOptionIndex,
    arduboy, tinyfont
  );
}

void drawDialog(
  String title,

  uint8_t options[],
  uint8_t optionsSize,
  uint8_t selectedOptionIndex,

  Arduboy2 arduboy,
  Tinyfont tinyfont
) {
  String optionsAsStrings[optionsSize];
  for (uint8_t i = 0; i < optionsSize; i++) {
    optionsAsStrings[i] = String(options[i]);
  }

  drawDialog(
    title,
    optionsAsStrings, optionsSize, selectedOptionIndex,
    arduboy, tinyfont
  );
}

void drawDie(
  int8_t x,
  int8_t y,

  String value,

  Arduboy2 arduboy,
  Tinyfont tinyfont,

  uint8_t size,

  bool useTinyFont = false,
  uint8_t fillet = INNER_FILLET
) {
  uint8_t textWidth = value.length() * DIE_CHAR_WIDTH
    + (value.length() - 1);

  if (useTinyFont) {
    tinyfont.setCursor(
      x + (float(size)
        - (value.length() < 2 ? TINY_TEXT_SIZE : TINY_TEXT_SIZE * 2 + 1)) / 2,
      y + (float(size) - TINY_TEXT_SIZE) / 2
    );
    tinyfont.print(value);
  } else {
    arduboy.setCursor(
      x + (float(size) - textWidth) / 2,
      y + (float(size) - DIE_CHAR_HEIGHT) / 2
    );
    arduboy.print(value);
  }

  // Intentionally draw container after text to ensure visibility
  arduboy.drawRoundRect(x, y, size, size, fillet);
}

void drawRollingDie(
  int8_t x,
  int8_t y,

  int8_t framesRemaining,
  bool clockwise,

  Arduboy2 arduboy,
  Tinyfont tinyfont,

  uint8_t size,
  int16_t coverage = 90
) {
  int16_t degrees = 45 + (float(framesRemaining) / (ROLL_FRAMES + 1)) * coverage;
  if (clockwise) { degrees = -degrees; }

  uint8_t hypotenuse = sqrt(2 * size * size);
  x = x - 1 - (hypotenuse - size) / 2;
  y = y - 1 - (hypotenuse - size) / 2;

  drawEquilateralPolygon(
    x, y,
    hypotenuse, degrees, 4,
    arduboy
  );

  tinyfont.setCursor(
    x + (hypotenuse - TINY_TEXT_SIZE) / 2 + 1,
    y + (hypotenuse - TINY_TEXT_SIZE) / 2 + 1
  );
  tinyfont.print(
    readFlashStringPointer(&ROLL_CHARS[random(0, 7 + 1)])
  );
}

void drawCenteredText(
  String text,
  uint8_t size,
  int8_t y,
  uint8_t lineIndex,
  Tinyfont tinyfont
) {
  tinyfont.setCursor(
    (WIDTH - (TINY_TEXT_SIZE * size + (size - 1))) / 2,
    y + (TINY_TEXT_SIZE + 1) * lineIndex
  );
  tinyfont.print(text);
}

void drawCredits(
  Arduboy2 arduboy,
  Tinyfont tinyfont
) {
  drawCenteredText(readFlashStringPointer(&CREDITS_STRINGS[0]), 7, 18, 0, tinyfont);
  drawCenteredText(readFlashStringPointer(&CREDITS_STRINGS[1]), 12, 18, 1, tinyfont);
  drawCenteredText(readFlashStringPointer(&CREDITS_STRINGS[2]), 13, 18, 3, tinyfont);
  drawCenteredText(readFlashStringPointer(&CREDITS_STRINGS[3]), 4, 18, 5, tinyfont);
}

void drawTitle(
  uint8_t dicePerRoll,
  uint8_t sidesPerDie,

  int8_t dieIndex,
  int8_t framesRemaining[],
  bool rollClockwise[],

  Arduboy2 arduboy,
  Tinyfont tinyfont
) {
  uint8_t dieSize = 19; // eyeballed!

  const uint8_t xOffset = (WIDTH - (dieSize * 3 + GAP * 2)) / 2;
  const uint8_t y = 15;

  String dieValues[3] = { String(dicePerRoll), "d", String(sidesPerDie) };

  for (uint8_t i = 0; i < 3; i++) {
    uint8_t x = xOffset + (dieSize + GAP) * i;

    if (dieIndex >= i && framesRemaining[i] > 0) {
      drawRollingDie(
        x, y,
        framesRemaining[i],
        rollClockwise[i],
        arduboy, tinyfont,
        dieSize, 180
      );
    } else {
      drawDie(
        x, y,
        dieIndex >= i ? dieValues[i] : "",
        arduboy, tinyfont,
        dieSize, false, OUTER_FILLET
      );
    }
  }

  drawCenteredText(readFlashStringPointer(&TITLE_STRINGS[0]), 9, 36, 0, tinyfont);
  drawCenteredText(readFlashStringPointer(&TITLE_STRINGS[1]), 12, 36, 1, tinyfont);
  drawCenteredText(readFlashStringPointer(&TITLE_STRINGS[2]), 10, 36, 2, tinyfont);
}

uint8_t getDieSize(
  uint8_t diceCount,
  uint8_t availableWidth,
  uint8_t availableHeight
) {
  switch(diceCount) {
    case 1:
      return availableWidth;
    case 2:
      return (availableHeight - GAP * 1) / 2;
    case 3: case 4:
      return (availableWidth - GAP * 1) / 2;
    case 5: case 6:
      return (availableHeight - GAP * 2) / 3;
  }

  return (availableHeight - GAP * 3) / 4;
}

void drawRollPanel(
  uint8_t values[],
  uint8_t valuesCount,
  uint8_t sidesPerDie,

  int8_t framesRemaining[],
  bool rollClockwise[],

  Arduboy2 arduboy,
  Tinyfont tinyfont,

  int8_t x = 0,
  int8_t y = 0,

  uint8_t width = SIDEBAR_WIDTH,
  uint8_t height = HEIGHT
) {
  drawPanel(
    x, y,
    width, height,
    String(valuesCount) + "D" + String(sidesPerDie),
    arduboy, tinyfont
  );

  uint8_t yOffset = FRAME * 2 + GAP * 3 + TINY_TEXT_SIZE;
  uint8_t availableHeight = height - yOffset - (GAP + FRAME);

  uint8_t dieSize =
    getDieSize(valuesCount, width - (GAP + FRAME) * 2, availableHeight);
  uint8_t diceColumns =
    floor((width - (GAP + FRAME) * 2) / dieSize);
  uint8_t diceRows = ceil(float(valuesCount) / diceColumns);

  uint8_t xOffset = (
    width - dieSize * min(valuesCount, diceColumns)
      - GAP * (diceColumns - 1)
    ) / 2;

  for (uint8_t i = 0; i < valuesCount; i++) {
    uint8_t diceX = x + xOffset
      + (i % diceColumns) * (dieSize + GAP);
    uint8_t diceY = y + yOffset
      + floor(i / diceColumns) * (dieSize + GAP);

    if (framesRemaining[i] > 0) {
      drawRollingDie(
        diceX, diceY,
        framesRemaining[i],
        rollClockwise[i],
        arduboy, tinyfont,
        dieSize
      );
    } else {
      drawDie(
        diceX, diceY,
        values[i] == 0 ? "?" : String(values[i]),
        arduboy, tinyfont,
        dieSize, (sidesPerDie >= 10 && valuesCount >= 7)
      );
    }
  }
}

uint8_t getIdealGraphBarWidth(
  uint8_t maxWidth,
  uint8_t barsCount
) {
  return (maxWidth - (GAP + FRAME) * 2 - GAP * (barsCount - 1)) / barsCount;
}

uint8_t getIdealGraphWidth(
  uint8_t maxWidth,
  uint8_t barsCount
) {
  return getIdealGraphBarWidth(maxWidth, barsCount) * barsCount
    + (GAP + FRAME) * 2 + GAP * (barsCount - 1);
}

// TODO: ~9d6 off center?
void drawGraphPanel(
  int8_t x,
  int8_t y,

  uint16_t sumCounts[],
  uint8_t sumsCount,

  Arduboy2 arduboy,
  Tinyfont tinyfont,

  uint8_t width = GRAPH_WIDTH,
  uint8_t height = HEIGHT
) {
  drawPanel(
    x, y,
    width, height,
    readFlashStringPointer(&TITLE_STRINGS[1]),
    arduboy, tinyfont,
    false, Direction::Down
  );

  uint8_t idealBarWidth = getIdealGraphBarWidth(width, sumsCount);
  uint16_t maxCount = getMaxValue(sumCounts, sumsCount);

  uint8_t xOffset = max(0, (width - getIdealGraphWidth(width, sumsCount)) / 2)
    + (GAP + FRAME);
  float plotWidth = idealBarWidth == 0
    ? float(width) / sumsCount
    : (idealBarWidth + GAP);

  for (uint8_t i = 0; i < sumsCount; i++) {
    uint8_t barHeight = (float(sumCounts[i]) / maxCount) *
      (height - (FRAME * 3 + GAP * 4 + TINY_TEXT_SIZE));

    if (barHeight > 0) {
      arduboy.fillRect(
        x + xOffset + i * plotWidth,
        y + height - barHeight - (GAP + FRAME),
        max(1, idealBarWidth),
        barHeight
      );
    }
  }
}

void drawDebugPanel(
  int8_t x,
  int8_t y,

  // OperationState
  uint16_t sumCounts[],
  uint8_t sumsCount,
  uint8_t minSum,
  uint8_t maxSum,
  uint16_t rollsCount,
  uint32_t totalSum,

  Arduboy2 arduboy,
  Tinyfont tinyfont,

  uint8_t width = GRAPH_WIDTH,
  uint8_t height = HEIGHT
) {
  drawPanel(
    x, y,
    width, height,
    readFlashStringPointer(&DEBUG_STRINGS[0]),
    arduboy, tinyfont,
    false, Direction::Up
  );

  Xy bodyXy = getPanelTextXy(x, y, width, height, 0);
  int8_t startingIndex = max(0, (sumsCount - (DEBUG_MAX_ROWS * 3)) / 2);

  tinyfont.setCursor(bodyXy.x, bodyXy.y);
  tinyfont.print(readFlashStringPointer(&DEBUG_STRINGS[1]));
  tinyfont.print(getPrettyAverage(totalSum, rollsCount) + "\n");
  tinyfont.print(readFlashStringPointer(&DEBUG_STRINGS[2]));
  tinyfont.print(String(rollsCount) + "\n");
  tinyfont.print(readFlashStringPointer(&DEBUG_STRINGS[3]));
  tinyfont.print(minSum);
  tinyfont.print(readFlashStringPointer(&DEBUG_STRINGS[4]));
  tinyfont.print(maxSum);

  for (
    uint8_t i = startingIndex;
    i < min(sumsCount, startingIndex + DEBUG_MAX_ROWS * DEBUG_COLS);
    i++
  ) {
    uint8_t likelihood = (float(sumCounts[i]) / rollsCount * 100);

    Xy xy = {
      bodyXy.x + ((i - startingIndex) / (DEBUG_MAX_ROWS))
        * ((width - (bodyXy.x - x) * 2) / DEBUG_COLS),
      bodyXy.y + (((i - startingIndex) % DEBUG_MAX_ROWS) + 3.5) * (TINY_TEXT_SIZE + GAP)
    };

    tinyfont.setCursor(xy.x, xy.y);
    tinyfont.print(((i + minSum < 10) ? " " : ""));
    tinyfont.print(((i + minSum < 100) ? " " : ""));
    tinyfont.print(String(i + minSum));

    // Squeezing pixels around : for max real estate
    tinyfont.setCursor(xy.x + (TINY_TEXT_SIZE + GAP) * 3 - 1, xy.y);
    tinyfont.print(":");
    tinyfont.setCursor(xy.x + (TINY_TEXT_SIZE + GAP) * 3 + 2, xy.y);
    tinyfont.print(String(likelihood));
    tinyfont.print("%");
  }
}

#endif