#ifndef graph_h
#define graph_h

# define OUTER_FILLET 3
# define INNER_FILLET 1

# define GRAPH_FRAME  1
# define GRAPH_GAP    1

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

// TODO: arg types?
// TODO: smaller, fix overlap when 3+ digits
// TODO: use size
void printRolls(int x, int y, int sumCounts[11], Tinyfont tinyfont) {
  for (uint8_t i = 0; i < 11; i++) {
    tinyfont.setCursor(x + i * 11, y);
    tinyfont.print(String(i + 2));

    tinyfont.setCursor(x + i * 11, y + 5);
    tinyfont.print(String(sumCounts[i]));
  }
}

uint8_t getIdealGraphBarWidth(
  uint8_t maxWidth,
  uint8_t barsCount
) {
  return (maxWidth - (GRAPH_GAP + GRAPH_FRAME) * 2 - GRAPH_GAP * (barsCount - 1)) / barsCount;
}

uint8_t getIdealGraphWidth(
  uint8_t maxWidth,
  uint8_t barsCount
) {
  return getIdealGraphBarWidth(maxWidth, barsCount) * barsCount
    + (GRAPH_GAP + GRAPH_FRAME) * 2 + GRAPH_GAP * (barsCount - 1);
}

void drawGraph(
  int x,
  int y,

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
    uint8_t barHeight = (float(sumCounts[i]) / sumCount) * (height - (GRAPH_GAP + GRAPH_FRAME) * 2);

    if (barHeight > 0) {
      arduboy.fillRoundRect(
        x + (GRAPH_GAP + GRAPH_FRAME) + i * (barWidth + GRAPH_GAP),
        y + height - barHeight - (GRAPH_GAP + GRAPH_FRAME),
        barWidth,
        barHeight,
        INNER_FILLET
      );
    }
  }
}

#endif