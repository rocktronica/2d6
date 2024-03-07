#ifndef graph_h
#define graph_h

uint8_t getSum(uint8_t values[2]) {
  return values[0] + values[1];
}

// TODO: more idiomatic?
uint8_t getMaxSumCount(int sumCounts[11]) {
  uint8_t maxSum = 0;

  for (uint8_t i = 0; i < 11; i++) {
    maxSum = max(maxSum, sumCounts[i]);
  }

  return maxSum;
}

String getRollText(uint8_t values[2]) {
  return String(
    values[0]) + "+" + String(values[1]) +
    "=" + String(getSum(values)
  );
}

// TODO: arg types?
// TODO: smaller, fix overlap when 3+ digits
void printRolls(int x, int y, int sumCounts[11], Tinyfont tinyfont) {
  for (uint8_t i = 0; i < 11; i++) {
    tinyfont.setCursor(x + i * 11, y);
    tinyfont.print(String(i + 2));

    tinyfont.setCursor(x + i * 11, y + 5);
    tinyfont.print(String(sumCounts[i]));
  }
}

void drawGraph(
  int x,
  int y,

  int sumCounts[11],

  Arduboy2 arduboy,
  Tinyfont tinyfont,

  uint8_t width = 11 * 8,
  uint8_t height = HEIGHT,

  bool showFrame = true,
  bool showLegend = true
) {
  if (showFrame) {
    arduboy.drawRect(x, y, width, height);
  }

  uint8_t barWidth = width / 11;
  int sumCount = getMaxSumCount(sumCounts);

  for (uint8_t i = 0; i < 11; i++) {
    uint8_t barHeight = (float(sumCounts[i]) / sumCount) * height;

    if (barHeight > 0) {
      arduboy.fillRect(
        x + i * barWidth,
        y + (height - barHeight),
        barWidth,
        barHeight,
        1
      );

      if (showLegend) {
        tinyfont.setCursor(x + i * barWidth, y + height - 5);
        tinyfont.setTextColor(BLACK);
        tinyfont.print(i + 2);
      }
    }
  }
}

#endif