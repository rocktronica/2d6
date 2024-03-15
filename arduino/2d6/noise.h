#pragma once

enum Volume {
  High,
  Low,
  Mute
};

// Tones borrowed from any-kind-of-car-and-any-kind-of-truck
// Variable names kept in homage!

const uint16_t CHANGE_TONES[] PROGMEM = {
 NOTE_G3,34,
 NOTE_C4,68, NOTE_E4,68, NOTE_C5,136,
 TONES_END
};

const uint16_t JUMP_TONES[] PROGMEM = {
 NOTE_E4,17, NOTE_D4,17, NOTE_C4,17, NOTE_E3,17,
 TONES_END
};

void makeNoise(
  ArduboyTones arduboyTones,
  uint16_t sequence[],
  Volume volume
) {
  if (volume == Volume::Mute) {
    return;
  }

  arduboyTones.tones(sequence);
}