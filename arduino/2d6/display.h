#ifndef graph_h
#define graph_h

# define SIDEBAR_MIN_WIDTH  40

# define OUTER_FILLET       3
# define INNER_FILLET       1

# define FRAME              1
# define FRAME_GAP          2
# define GAP                1

# define DIE_SIZE           11
# define DIE_CHAR_WIDTH     5
# define DIE_CHAR_HEIGHT    7

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

void drawDie(
  uint8_t x,
  uint8_t y,

  uint8_t value,

  Arduboy2 arduboy,

  uint8_t width = DIE_SIZE,
  uint8_t height = DIE_SIZE
) {
  arduboy.setCursor(
    x + (float(width) - DIE_CHAR_WIDTH) / 2,
    y + (float(height) - DIE_CHAR_HEIGHT) / 2
  );
  arduboy.print((value > 0) ? String(value) : "?");

  // Intentionally draw container after text to ensure visibility
  arduboy.drawRoundRect(x, y, width, height, INNER_FILLET);
}

void drawSidebar(
  uint8_t x,
  uint8_t y,

  uint8_t values[],
  uint8_t valuesCount,
  String text,

  uint8_t width,
  uint8_t height,

  Arduboy2 arduboy,
  Tinyfont tinyfont
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
      values[i],
      arduboy
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

  uint8_t width = 60,
  uint8_t height = 40
) {
  arduboy.drawRoundRect(x, y, width, height, OUTER_FILLET);

  uint8_t barWidth = getIdealGraphBarWidth(width, barsCount);
  int sumCount = getMaxValue(sumCounts, barsCount);

  for (uint8_t i = 0; i < barsCount; i++) {
    uint8_t barHeight = (float(sumCounts[i]) / sumCount) * (height - (FRAME_GAP + FRAME) * 2);

    if (barHeight > 0) {
      arduboy.fillRoundRect(
        x + (FRAME_GAP + FRAME) + i * (barWidth + GAP),
        y + height - barHeight - (FRAME_GAP + FRAME),
        barWidth,
        barHeight,
        barHeight > INNER_FILLET ? INNER_FILLET : 0
      );
    }
  }
}

#endif