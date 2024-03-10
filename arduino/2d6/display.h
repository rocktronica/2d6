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

# define ANIMATION_FRAMES   5

enum GameState {
  Title,
  Operation
};

uint8_t getSum(uint8_t values[], uint8_t size) {
  int sum = 0;

  for (uint8_t i = 0; i < size; i++) {
    sum += values[i];
  }

  return sum;
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

  bool inFlux = false,
  uint8_t maxValue = 9,

  uint8_t width = DIE_SIZE,
  uint8_t height = DIE_SIZE
) {
  String displayValue = inFlux
    ? String(random(1, maxValue + 1))
    : value != "0" ? value : "?";

  if (maxValue < 10) {
    arduboy.setCursor(
      x + (float(width) - DIE_CHAR_WIDTH) / 2,
      y + (float(height) - DIE_CHAR_HEIGHT) / 2
    );
    arduboy.print(displayValue);
  } else {
    tinyfont.setCursor(
      x + (float(width)
        - (value.length() < 2 ? DIE_CHAR_SMALL : DIE_CHAR_SMALL * 2 + 1)) / 2,
      y + (float(height) - DIE_CHAR_SMALL) / 2
    );
    tinyfont.print(displayValue);
  }

  // Intentionally draw container after text to ensure visibility
  arduboy.drawRoundRect(x, y, width, height, INNER_FILLET);
}

// TODO: drawMenu
void drawTitle(
  uint8_t dicePerRoll,
  uint8_t sidesPerDie,

  bool inFlux,

  Arduboy2 arduboy,
  Tinyfont tinyfont
) {
  const uint8_t xOffset = (WIDTH - (DIE_SIZE * 3 + GAP * 2)) / 2;
  const uint8_t y = (HEIGHT - DIE_SIZE) / 2;

  drawDie(xOffset + (DIE_SIZE + GAP) * 0, y, String(dicePerRoll), arduboy, tinyfont, inFlux);
  drawDie(xOffset + (DIE_SIZE + GAP) * 1, y, "d", arduboy, tinyfont, inFlux);
  drawDie(xOffset + (DIE_SIZE + GAP) * 2, y, String(sidesPerDie), arduboy, tinyfont, inFlux);
}

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
      String(values[i]),
      arduboy, tinyfont,
      inFlux, maxValue
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
  int sumCount = getMaxValue(sumCounts, barsCount);

  const uint8_t xOffset = (width - getIdealGraphWidth(width, barsCount)) / 2;

  for (uint8_t i = 0; i < barsCount; i++) {
    uint8_t barHeight = (float(sumCounts[i]) / sumCount) * (height - (FRAME_GAP + FRAME) * 2);

    if (barHeight > 0) {
      arduboy.fillRoundRect(
        x + xOffset + (FRAME_GAP + FRAME) + i * (barWidth + GAP),
        y + height - barHeight - (FRAME_GAP + FRAME),
        max(1, barWidth),
        barHeight,
        barWidth > INNER_FILLET * 2
          ? (barHeight > INNER_FILLET * 2 ? INNER_FILLET : 0)
          : 0
      );
    }
  }
}

#endif