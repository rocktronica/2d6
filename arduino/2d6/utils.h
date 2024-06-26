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

float getRadians(int16_t degrees) { return (float(degrees) * 71) / 4068; }

Xy getEquilateralPolygonCorner(
  int16_t x,
  int16_t y,

  uint8_t size,

  int16_t degrees
) {
  float angleInRadians = getRadians(degrees);
  uint8_t radius = size / 2;
  Xy center = {radius, radius};
  int8_t adjacent = radius * cos(angleInRadians);
  int8_t opposite = radius * sin(angleInRadians);

  return {
    x + center.x + adjacent,
    y + center.y + opposite
  };
}

void drawEquilateralPolygon(
  int16_t x,
  int16_t y,

  uint8_t size,
  int16_t degrees,
  uint32_t corners,

  Arduboy2 arduboy
) {
  Xy points[corners];
  for (int i = 0; i < corners; i++) {
    points[i] = getEquilateralPolygonCorner(x, y, size, degrees + i * (360 / corners));
  }

  drawPolygon(points, corners, arduboy);
}

int8_t getIndexOfValue(
  uint8_t needle,
  uint8_t haystack[],
  uint8_t haystackSize
) {
  for (int i = 0; i < haystackSize; i++) {
    if (haystack[i] == needle) {
      return i;
    }
  }

  return -1;
}

int8_t getRollFramesCount(int8_t defaultValue, int8_t variance = 2) {
  return random(defaultValue - variance, defaultValue + variance + 1);
}

uint8_t getSum(uint8_t values[], uint8_t size) {
  int sum = 0;

  for (uint8_t i = 0; i < size; i++) {
    sum += values[i];
  }

  return sum;
}

String getPrettyAverage(uint32_t total, int size) {
  if (size == 0) {
    return "?";
  }

  char response[4];
  dtostrf(float(total) / size, -4, 1, response);
  return response;
}

uint16_t getMaxValue(uint16_t values[], uint8_t size) {
  int maxValue = 0;

  for (uint8_t i = 0; i < size; i++) {
    maxValue = max(maxValue, values[i]);
  }

  return maxValue;
}

#endif