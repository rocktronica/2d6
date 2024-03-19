#ifndef graph_h
#define graph_h

#include "utils.h"

# define SIDEBAR_WIDTH      45
# define GRAPH_WIDTH        (WIDTH - SIDEBAR_WIDTH - FRAME_GAP)

# define OUTER_FILLET       3
# define INNER_FILLET       1

# define FRAME              1
# define FRAME_GAP          2
# define GAP                1
# define DIALOG_GAP         3

# define DIE_SIZE           13
# define DIE_CHAR_WIDTH     5    // Default Arduboy font
# define DIE_CHAR_HEIGHT    7    // Default Arduboy font
# define DIE_CHAR_SMALL     4    // TinyFont 4x4

# define FRAMES_PER_SECOND  15
# define ROLL_FRAMES        3
# define TITLE_FRAMES       30
# define TITLE_ROLL_0_FRAME 3
# define TITLE_ROLL_1_FRAME 4
# define TITLE_ROLL_2_FRAME 6

enum Dialog {
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

uint8_t getSum(uint8_t values[], uint8_t size) {
  int sum = 0;

  for (uint8_t i = 0; i < size; i++) {
    sum += values[i];
  }

  return sum;
}

String getPrettyAverage(uint32_t total, int size) {
  char response[4];
  dtostrf(float(total) / size, -4, 1, response);
  return response;
}

int getMaxValue(int values[], uint8_t size) {
  int maxValue = 0;

  for (uint8_t i = 0; i < size; i++) {
    maxValue = max(maxValue, values[i]);
  }

  return maxValue;
}

void drawPanel(
  uint8_t x,
  uint8_t y,

  uint8_t width,
  uint8_t height,

  String title,

  Arduboy2 arduboy,
  Tinyfont tinyfont
) {
  arduboy.fillRoundRect(x, y, width, height, OUTER_FILLET, BLACK);
  arduboy.drawRoundRect(x, y, width, height, OUTER_FILLET);

  arduboy.drawFastHLine(x, y + DIE_CHAR_SMALL + FRAME + GAP + GAP, width);

  tinyfont.setCursor(
    (WIDTH - (DIE_CHAR_SMALL * title.length() + (title.length() - 1))) / 2,
    y + (FRAME + GAP)
  );
  tinyfont.print(title);
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
    tinyfont.setCursor(
      (WIDTH - width) / 2 + (FRAME + DIALOG_GAP),
      (HEIGHT - height) / 2 + FRAME * 2 + GAP * 2
        + DIE_CHAR_SMALL + DIALOG_GAP
        + (DIE_CHAR_SMALL + GAP) * i
    );
    tinyfont.print(
      (selectedOptionIndex == i ? ">" : " ") + options[i]
    );
  }
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

  return (FRAME + DIALOG_GAP) * 2
    + maxChars * DIE_CHAR_SMALL + (maxChars - 1) * GAP;
}

uint8_t getMinDialogHeight(
  uint8_t optionsSize
) {
  return (FRAME + GAP) * 2 + DIE_CHAR_SMALL
    + DIALOG_GAP * 2 + FRAME
    + DIE_CHAR_SMALL * optionsSize
    + GAP * (optionsSize - 1);
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
    getMinDialogHeight(optionsSize),
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

// TODO: BIG version for tabletop play
void drawDie(
  uint8_t x,
  uint8_t y,

  String value,

  Arduboy2 arduboy,
  Tinyfont tinyfont,

  uint8_t size = DIE_SIZE,
  bool useTinyFont = false,
  uint8_t fillet = INNER_FILLET,

  bool accent = false
) {
  uint8_t textWidth = value.length() * DIE_CHAR_WIDTH
    + (value.length() - 1);

  if (useTinyFont) {
    tinyfont.setCursor(
      x + (float(size)
        - (value.length() < 2 ? DIE_CHAR_SMALL : DIE_CHAR_SMALL * 2 + 1)) / 2,
      y + (float(size) - DIE_CHAR_SMALL) / 2
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

  if (accent) {
    arduboy.drawRoundRect(
      x + FRAME,
      y + FRAME,
      size - FRAME * 2,
      size - FRAME * 2,
      max(0, fillet - FRAME)
    );
  }
}

const char ROLL_CHARS[] = {
  '!','@','#','$','%','^','&','*'
};

void drawRollingDie(
  int8_t x,
  int8_t y,

  int8_t framesRemaining,
  bool clockwise,

  Arduboy2 arduboy,
  Tinyfont tinyfont,

  uint8_t size = DIE_SIZE,
  int16_t coverage = 90
) {
  int16_t degrees = 45 + (float(framesRemaining) / (ROLL_FRAMES + 1)) * coverage;
  if (clockwise) { degrees = -degrees; }

  uint8_t hypotenuse = sqrt(2 * size * size);
  x = x - 1 - (hypotenuse - size) / 2;
  y = y - 1 - (hypotenuse - size) / 2;

  drawRotatedSquare(
    x, y,
    hypotenuse,
    degrees,
    arduboy
  );

  // TODO: move cursor around 2x2
  tinyfont.setCursor(
    x + (hypotenuse - DIE_CHAR_SMALL) / 2 + 1,
    y + (hypotenuse - DIE_CHAR_SMALL) / 2 + 1
  );
  tinyfont.print(ROLL_CHARS[random(0, sizeof(ROLL_CHARS) + 1)]);
}

void drawTitle(
  uint8_t dicePerRoll,
  uint8_t sidesPerDie,

  int8_t dieIndex, // TODO: obviate
  int8_t framesRemaining[],
  bool rollClockwise[],

  Arduboy2 arduboy,
  Tinyfont tinyfont
) {
  uint8_t dieSize = DIE_CHAR_WIDTH * 2 + 8; // a little arbitrary

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

  // TODO: tidy
  tinyfont.setCursor(
    (WIDTH - (DIE_CHAR_SMALL * 9 + (9 - 1))) / 2,
    36
  );
  tinyfont.print("DICE ROLL");

  tinyfont.setCursor(
    (WIDTH - (DIE_CHAR_SMALL * 12 + (12 - 1))) / 2,
    36 + (DIE_CHAR_SMALL + 1) * 1
  );
  tinyfont.print("DISTRIBUTION");

  tinyfont.setCursor(
    (WIDTH - (DIE_CHAR_SMALL * 10 + (10 - 1))) / 2,
    36 + (DIE_CHAR_SMALL + 1) * 2
  );
  tinyfont.print("VISUALIZER");
}

// TODO: title
void drawSidebar(
  uint8_t x,
  uint8_t y,

  uint8_t values[],
  uint8_t valuesCount,
  uint8_t maxValue,
  String text,

  int8_t framesRemaining[],
  bool rollClockwise[],

  Arduboy2 arduboy,
  Tinyfont tinyfont,

  uint8_t width = SIDEBAR_WIDTH,
  uint8_t height = HEIGHT
) {
  arduboy.drawRoundRect(x, y, width, height, OUTER_FILLET);

  const uint8_t diceColumns =
    floor((width - (FRAME_GAP + FRAME) * 2) / DIE_SIZE);
  const uint8_t diceRows = ceil(float(valuesCount) / diceColumns);

  const uint8_t xOffset = (
    (width - (FRAME_GAP + FRAME) * 2)
      - DIE_SIZE * min(valuesCount, diceColumns)
      - GAP * (diceColumns - 1)
    ) / 2;

  for (uint8_t i = 0; i < valuesCount; i++) {
    uint8_t diceX = x + FRAME + FRAME_GAP
      + xOffset + (i % diceColumns) * (DIE_SIZE + GAP);
    uint8_t diceY = y + FRAME + FRAME_GAP
      + floor(i / diceColumns) * (DIE_SIZE + GAP);

    if (framesRemaining[i] > 0) {
      drawRollingDie(
        diceX, diceY,
        framesRemaining[i],
        rollClockwise[i],
        arduboy, tinyfont
      );
    } else {
      drawDie(
        diceX, diceY,
        values[i] == 0 ? "?" : String(values[i]),
        arduboy, tinyfont,
        DIE_SIZE, maxValue >= 10
      );
    }
  }

  const uint8_t lineY = y + FRAME + FRAME_GAP
      + DIE_SIZE * diceRows + GAP * (diceRows - 1)
      + FRAME_GAP;

  // TinyFont breaks if asked to render off-screen
  // TODO: design away hack
  if (lineY <= HEIGHT - 10) {
    arduboy.drawFastHLine(x, lineY, width);

    tinyfont.setCursor(
      x + FRAME + FRAME_GAP,
      lineY + FRAME + FRAME_GAP
    );
    tinyfont.print(text);
  }
}

uint8_t getIdealGraphBarWidth(
  uint8_t maxWidth,
  uint8_t barsCount
) {
  return (maxWidth - (FRAME_GAP + FRAME) * 2 - GAP * (barsCount - 1)) / barsCount;
}

uint8_t getIdealGraphWidth(
  uint8_t maxWidth,
  uint8_t barsCount
) {
  return getIdealGraphBarWidth(maxWidth, barsCount) * barsCount
    + (FRAME_GAP + FRAME) * 2 + GAP * (barsCount - 1);
}

// TODO: fix for 6d20/etc
void drawGraph(
  uint8_t x,
  uint8_t y,

  int sumCounts[],
  uint8_t minSum,
  uint8_t maxSum,
  uint8_t barsCount,

  Arduboy2 arduboy,

  uint8_t width = GRAPH_WIDTH,
  uint8_t height = HEIGHT
) {
  arduboy.drawRoundRect(x, y, width, height, OUTER_FILLET);

  uint8_t barWidth = getIdealGraphBarWidth(width, barsCount);
  int maxCount = getMaxValue(sumCounts, barsCount);

  const uint8_t xOffset = (width - getIdealGraphWidth(width, barsCount)) / 2;

  for (uint8_t i = 0; i < barsCount; i++) {
    uint8_t barHeight = (float(sumCounts[i]) / maxCount) * (height - (FRAME_GAP + FRAME) * 2);

    if (barHeight > 0) {
      arduboy.fillRect(
        x + xOffset + (FRAME_GAP + FRAME) + i * (barWidth + GAP),
        y + height - barHeight - (FRAME_GAP + FRAME),
        max(1, barWidth),
        barHeight
      );
    }
  }

  if (maxCount <= (height - (FRAME_GAP + FRAME) * 2) / 2) {
    for (uint8_t i = 1; i < maxCount; i++) {
      arduboy.drawFastHLine(
        x + (FRAME_GAP + FRAME),
        y + (FRAME_GAP + FRAME) + (height - (FRAME_GAP + FRAME) * 2) * (float(i) / maxCount),
        width - (FRAME_GAP + FRAME) * 2,
        BLACK
      );
    }
  }
}

#endif