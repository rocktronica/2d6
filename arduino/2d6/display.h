#ifndef graph_h
#define graph_h

uint8_t getSum(uint8_t values[], uint8_t size) {
  int sum = 0;

  for (uint8_t i = 0; i < size; i++) {
    sum += values[i];
  }

  return sum;
}

// TODO: more idiomatic?
int getMaxSumCount(int sumCounts[], uint8_t size) {
  int maxSum = 0;

  for (uint8_t i = 0; i < size; i++) {
    maxSum = max(maxSum, sumCounts[i]);
  }

  return maxSum;
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

void drawGraph(
  int x,
  int y,

  int sumCounts[],
  uint8_t minSum,
  uint8_t maxSum,
  uint8_t size,

  Arduboy2 arduboy,
  Tinyfont tinyfont,

  uint8_t width = 60,
  uint8_t height = 40,

  bool showFrame = true,
  bool showLegend = true
) {
  if (showFrame) {
    arduboy.drawRect(x, y, width, height);
  }

  uint8_t barWidth = width / size;
  int sumCount = getMaxSumCount(sumCounts, size);

  for (uint8_t i = 0; i < size; i++) {
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
        tinyfont.print(i + minSum);
      }
    }
  }
}

#endif