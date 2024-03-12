#ifndef graph_h
#define graph_h

# define SIDEBAR_WIDTH      45
# define GRAPH_WIDTH        (WIDTH - SIDEBAR_WIDTH - FRAME_GAP)

# define OUTER_FILLET       3
# define INNER_FILLET       1

# define FRAME              1
# define FRAME_GAP          2
# define GAP                1

# define DIE_SIZE           13
# define DIE_CHAR_WIDTH     5    // Default Arduboy font
# define DIE_CHAR_HEIGHT    7    // Default Arduboy font
# define DIE_CHAR_SMALL     4    // TinyFont 4x4

# define CARET_FRAMES       3
# define ROLL_FRAMES        5
# define UPDATE_FRAMES      2

# define CARET_SIZE         6

enum GameState {
  Title,
  Operation
};

enum Direction {
  None,
  Up,
  Down
};

enum MenuDie {
  DicePerRoll = 0,
  Dee,
  SidesPerDie
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

void drawCaret(
  uint8_t x,
  uint8_t y,

  Direction direction,
  bool fill,

  Arduboy2 arduboy
) {
  if (fill) {
    arduboy.fillTriangle(
      x, y,
      x + CARET_SIZE, y,
      x + CARET_SIZE / 2,
        y + CARET_SIZE / (direction == Direction::Up ? -2 : 2)
    );
  } else {
    arduboy.drawTriangle(
      x, y,
      x + CARET_SIZE, y,
      x + CARET_SIZE / 2,
        y + CARET_SIZE / (direction == Direction::Up ? -2 : 2)
    );
  }
}

void drawMenu(
  uint8_t dicePerRoll,
  uint8_t sidesPerDie,

  uint8_t selectedDieIndex,
  Direction activeCaret,

  Arduboy2 arduboy,
  Tinyfont tinyfont,

  uint8_t dieSize = DIE_CHAR_WIDTH * 2 + 8 // a little arbitrary
) {
  const uint8_t xOffset = (WIDTH - (dieSize * 3 + GAP * 2)) / 2;
  const uint8_t y = (HEIGHT - dieSize) / 2;

  String dieValues[3] = { String(dicePerRoll), "d", String(sidesPerDie) };

  for (uint8_t i = 0; i < 3; i++) {
    uint8_t x = xOffset + (dieSize + GAP) * i;

    if (i != 1) {
      drawCaret(
        x + (dieSize - CARET_SIZE) / 2, y - (FRAME + GAP),
        Direction::Up,
        selectedDieIndex == i && activeCaret == Direction::Up,
        arduboy
      );

      drawCaret(
        x + (dieSize - CARET_SIZE) / 2, y + dieSize + GAP,
        Direction::Down,
        selectedDieIndex == i && activeCaret == Direction::Down,
        arduboy
      );
    }

    drawDie(
      x, y,
      dieValues[i],
      arduboy, tinyfont,
      dieSize, false, OUTER_FILLET, selectedDieIndex == i
    );
  }
}

// TODO: title
void drawSidebar(
  uint8_t x,
  uint8_t y,

  uint8_t values[],
  uint8_t valuesCount,
  uint8_t maxValue,
  String text,

  bool inFlux,

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
    drawDie(
      x + FRAME + FRAME_GAP + xOffset + (i % diceColumns) * (DIE_SIZE + GAP),
      y + FRAME + FRAME_GAP + floor(i / diceColumns) * (DIE_SIZE + GAP),
      inFlux
        ? String(random(1, maxValue + 1))
        : values[i] == 0 ? "?" : String(values[i]),
      arduboy, tinyfont,
      DIE_SIZE, maxValue >= 10
    );
  }

  const uint8_t lineY = y + FRAME + FRAME_GAP
      + DIE_SIZE * diceRows + GAP * (diceRows - 1)
      + FRAME_GAP;

  arduboy.drawFastHLine(x, lineY, width);

  tinyfont.setCursor(
    x + FRAME + FRAME_GAP,
    lineY + FRAME + FRAME_GAP
  );
  tinyfont.print(text);
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