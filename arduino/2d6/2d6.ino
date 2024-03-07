#include <Arduboy2.h>
#include <ArduboyTones.h>
#include <Tinyfont.h>

Arduboy2 arduboy;
ArduboyTones sound(arduboy.audio.enabled);
Tinyfont tinyfont = Tinyfont(arduboy.sBuffer, WIDTH, HEIGHT);

uint8_t currentRollValues[2] = {0, 0};
int pastRollValues[11] = {0,0,0,0,0,0,0,0,0,0,0};

void update() {
  currentRollValues[0] = random(0, 6 + 1);
  currentRollValues[1] = random(0, 6 + 1);

  pastRollValues[getSum(currentRollValues) - 2] += 1;
}

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
void printRolls(int x, int y) {
  for (uint8_t i = 0; i < 11; i++) {
    tinyfont.setCursor(x + i * 11, y);
    tinyfont.print(String(i + 2));

    tinyfont.setCursor(x + i * 11, y + 5);
    tinyfont.print(String(pastRollValues[i]));
  }
}

void setup() {
  arduboy.beginDoFirst();
  arduboy.waitNoButtons();

  arduboy.setFrameRate(15);

  update();
}

void loop() {
  if (!arduboy.nextFrame()) {
    return;
  }

  arduboy.pollButtons();
  arduboy.clear();

  if (arduboy.pressed(A_BUTTON)) {
    arduboy.initRandomSeed();
  }

  if (arduboy.pressed(A_BUTTON) || arduboy.pressed(B_BUTTON)) {
    update();
  }

  tinyfont.setCursor(1, 1);
  tinyfont.print("NOW:" + getRollText(currentRollValues));

  printRolls(1, 10);

  arduboy.display();
}
