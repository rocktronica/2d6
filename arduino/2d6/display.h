#ifndef graph_h
#define graph_h

uint8_t getSum(uint8_t values[2]) {
  return values[0] + values[1];
}

String getRollText(uint8_t values[2]) {
  return String(
    values[0]) + "+" + String(values[1]) +
    "=" + String(getSum(values)
  );
}

// TODO: arg types?
// TODO: smaller, fix overlap when 3+ digits
void printRolls(int x, int y, int pastRollValues[11], Tinyfont tinyfont) {
  for (uint8_t i = 0; i < 11; i++) {
    tinyfont.setCursor(x + i * 11, y);
    tinyfont.print(String(i + 2));

    tinyfont.setCursor(x + i * 11, y + 5);
    tinyfont.print(String(pastRollValues[i]));
  }
}

#endif