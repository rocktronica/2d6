#ifndef Utils_h
#define Utils_h

#include "Arduino.h"

struct Xy {
  int16_t x;
  int16_t y;
};

void drawLine(
  int16_t x0,
  int16_t y0,
  int16_t x1,
  int16_t y1,
  Arduboy2 arduboy
) {
  if (x0 == x1 && y0 == y1) {
    return;
  } else if (x0 == x1) {
    arduboy.drawFastVLine(x0, min(y0, y1), abs(y0 - y1) + 1);
  } else if (y0 == y1) {
    arduboy.drawFastHLine(min(x0, x1), y0, abs(x0 - x1) + 1);
  } else {
    arduboy.drawLine(x0, y0, x1, y1);
  }
};

void drawSegmentedLine(Xy points[], uint8_t size, Arduboy2 arduboy) {
  Xy lastPoint = {0,0};

  for (int i = 0; i < size; i++) {
    if (i > 0) {
      drawLine(points[i].x, points[i].y, lastPoint.x, lastPoint.y, arduboy);
    }

    lastPoint = points[i];
  }
};

void drawPolygon(Xy points[], uint8_t size, Arduboy2 arduboy) {
  drawSegmentedLine(points, size, arduboy);
  drawLine(points[0].x, points[0].y, points[size - 1].x, points[size - 1].y, arduboy);
};

void drawRotatedSquare(
  uint8_t x,
  uint8_t y,

  uint8_t size,

  int8_t rotation,

  Arduboy2 arduboy
) {
  // Keep size behavior consistent with drawRect
  size = size - 1;

  // TODO: perfect math
  uint8_t offset = (float((360 + rotation) % 90) / 90) * size;

  Xy points[4] = {
    {x + offset, y},
    {x + size, y + offset},
    {x + size - offset, y + size},
    {x, y + size - offset},
  };
  drawPolygon(points, 4, arduboy);
}

#endif