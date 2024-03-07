#include <Arduboy2.h>
#include <ArduboyTones.h>
#include <Tinyfont.h>

Arduboy2 arduboy;
ArduboyTones sound(arduboy.audio.enabled);
Tinyfont tinyfont = Tinyfont(arduboy.sBuffer, WIDTH, HEIGHT);

void setup() {
  arduboy.beginDoFirst();
  arduboy.waitNoButtons();

  arduboy.setFrameRate(15);
}

void loop() {
  if (!arduboy.nextFrame()) {
    return;
  }

  arduboy.clear();

  tinyfont.setCursor(1, 1);
  tinyfont.print("sup");

  arduboy.display();
}
