#ifndef graph_h
#define graph_h

# define SIDEBAR_MIN_WIDTH  40

# define OUTER_FILLET       3
# define INNER_FILLET       1

# define FRAME              1
# define FRAME_GAP          2
# define GAP                1

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

String getRollText(uint8_t values[], uint8_t size) {
  String equation = String(values[0]);

  for (uint8_t i = 1; i < size; i++) {
    equation += "+" + String(values[i]);
  }

  return equation + "=" + String(getSum(values, size));
}

void drawSidebar(
  uint8_t x,
  uint8_t y,
  String text,
  uint8_t width,
  uint8_t height,
  Arduboy2 arduboy,
  Tinyfont tinyfont
) {
  arduboy.drawRoundRect(x, y, width, height, OUTER_FILLET);

  tinyfont.setCursor(x + FRAME + FRAME_GAP, x + FRAME + FRAME_GAP);
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