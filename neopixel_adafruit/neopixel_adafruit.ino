#include <Adafruit_NeoPixel.h>

#define LED_PIN     6        
#define NUM_LEDS    24      
#define BRIGHTNESS  80       

#define EFFECT_DURATION 6000

Adafruit_NeoPixel strip(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

uint32_t colorHSV(uint16_t h, uint8_t s, uint8_t v) {
  return strip.ColorHSV(h * 256, s * 255, v);
}

uint32_t scaleColor(uint32_t color, uint8_t scale) {
  uint8_t r = (uint8_t)(color >> 16);
  uint8_t g = (uint8_t)(color >> 8);
  uint8_t b = (uint8_t)(color);
  r = (uint16_t)r * scale / 255;
  g = (uint16_t)g * scale / 255;
  b = (uint16_t)b * scale / 255;
  return strip.Color(r, g, b);
}

void fadeToBlack(int idx, uint8_t fadeAmount) {
  uint32_t color = strip.getPixelColor(idx);
  uint8_t r = (uint8_t)(color >> 16);
  uint8_t g = (uint8_t)(color >> 8);
  uint8_t b = (uint8_t)(color);
  r = (r > fadeAmount) ? r - fadeAmount : 0;
  g = (g > fadeAmount) ? g - fadeAmount : 0;
  b = (b > fadeAmount) ? b - fadeAmount : 0;
  strip.setPixelColor(idx, strip.Color(r, g, b));
}

void clearAll() {
  strip.clear();
  strip.show();
}


void rainbowWheel(unsigned long duration) {
  unsigned long start = millis();
  uint16_t hue = 0;
  while (millis() - start < duration) {
    for (int i = 0; i < NUM_LEDS; i++) {
      uint16_t pixelHue = hue + (i * 65536L / NUM_LEDS);
      strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(pixelHue)));
    }
    strip.show();
    hue += 512;
    delay(20);
  }
}


void colorChase(unsigned long duration) {
  uint32_t colors[] = {
    strip.Color(255, 0, 0),    // Red
    strip.Color(255, 100, 0),  // Orange
    strip.Color(255, 255, 0),  // Yellow
    strip.Color(0, 255, 0),    // Green
    strip.Color(0, 255, 255),  // Cyan
    strip.Color(0, 0, 255),    // Blue
    strip.Color(255, 0, 255),  // Magenta
    strip.Color(255, 255, 255) // White
  };
  uint8_t numColors = 8;
  unsigned long start = millis();
  uint8_t ci = 0;
  int pos = 0;

  while (millis() - start < duration) {
    // Fade all pixels
    for (int i = 0; i < NUM_LEDS; i++) fadeToBlack(i, 80);
    strip.setPixelColor(pos, colors[ci]);
    strip.show();
    pos = (pos + 1) % NUM_LEDS;
    if (pos == 0) ci = (ci + 1) % numColors;
    delay(40);
  }
}

void meteorRain(unsigned long duration) {
  unsigned long start = millis();
  int meteorSize = 4;

  while (millis() - start < duration) {
    for (int i = 0; i < NUM_LEDS + NUM_LEDS; i++) {
      // Fade existing pixels
      for (int j = 0; j < NUM_LEDS; j++) fadeToBlack(j, 64);

      // Draw meteor head and tail
      for (int j = 0; j < meteorSize; j++) {
        int idx = i - j;
        if (idx >= 0 && idx < NUM_LEDS) {
          uint8_t bright = 255 - (j * (255 / meteorSize));
          uint8_t hue = 160 + j * 10; // blue-purple range
          strip.setPixelColor(idx, strip.gamma32(
            strip.ColorHSV((uint16_t)hue * 256, 255, bright)
          ));
        }
      }
      strip.show();
      delay(30);
      if (millis() - start >= duration) return;
    }
  }
}

void breathingGlow(unsigned long duration) {
  unsigned long start = millis();
  uint16_t hue = 0;

  while (millis() - start < duration) {
    // Fade in
    for (int b = 0; b <= 255; b += 3) {
      uint32_t c = strip.gamma32(strip.ColorHSV(hue, 255 * 255, b));
      for (int i = 0; i < NUM_LEDS; i++) strip.setPixelColor(i, c);
      strip.show();
      delay(10);
      if (millis() - start >= duration) return;
    }
    // Fade out
    for (int b = 255; b >= 0; b -= 3) {
      uint32_t c = strip.gamma32(strip.ColorHSV(hue, 255 * 255, b));
      for (int i = 0; i < NUM_LEDS; i++) strip.setPixelColor(i, c);
      strip.show();
      delay(10);
      if (millis() - start >= duration) return;
    }
    hue += 8192; // shift hue each breath cycle
  }
}

void sparkle(unsigned long duration) {
  unsigned long start = millis();
  while (millis() - start < duration) {
    int pixel = random(NUM_LEDS);
    uint16_t hue = random(65536);
    strip.setPixelColor(pixel, strip.gamma32(strip.ColorHSV(hue, 200 * 255, 255)));
    strip.show();
    delay(30);
    
    for (int i = 0; i < NUM_LEDS; i++) fadeToBlack(i, 20);
  }
}

void theaterChaseRainbow(unsigned long duration) {
  unsigned long start = millis();
  uint16_t hue = 0;

  while (millis() - start < duration) {
    for (int q = 0; q < 3; q++) {
      strip.clear();
      for (int i = q; i < NUM_LEDS; i += 3) {
        strip.setPixelColor(i, strip.gamma32(
          strip.ColorHSV(hue + (uint16_t)i * 2730, 255 * 255, 255)
        ));
      }
      strip.show();
      delay(60);
      hue += 512;
      if (millis() - start >= duration) return;
    }
  }
}


#define COOLING   55
#define SPARKING  120
byte heat[NUM_LEDS];


uint32_t heatColor(byte t) {
  byte r, g, b;
  if (t < 85) {
    r = t * 3; g = 0; b = 0;
  } else if (t < 170) {
    t -= 85;
    r = 255; g = t * 3; b = 0;
  } else {
    t -= 170;
    r = 255; g = 255; b = t * 3;
  }
  return strip.Color(r, g, b);
}

void fire(unsigned long duration) {
  unsigned long start = millis();
  while (millis() - start < duration) {
    // Cool down each cell
    for (int i = 0; i < NUM_LEDS; i++) {
      uint8_t cool = random(0, ((COOLING * 10) / NUM_LEDS) + 2);
      heat[i] = (heat[i] > cool) ? heat[i] - cool : 0;
    }
    // Drift heat up
    for (int k = NUM_LEDS - 1; k >= 2; k--) {
      heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2]) / 3;
    }
    // Randomly ignite sparks at base
    if (random(255) < SPARKING) {
      int y = random(7);
      heat[y] = min(255, heat[y] + random(160, 255));
    }
    // Map heat to LED colors
    for (int j = 0; j < NUM_LEDS; j++) {
      strip.setPixelColor(j, heatColor(heat[j]));
    }
    strip.show();
    delay(15);
  }
}

void colorWipe(unsigned long duration) {
  uint32_t cols[] = {
    strip.Color(255, 0, 0),   // Red
    strip.Color(0, 255, 0),   // Green
    strip.Color(0, 0, 255),   // Blue
    strip.Color(255, 255, 0), // Yellow
    strip.Color(0, 255, 255), // Cyan
    strip.Color(255, 0, 255)  // Magenta
  };
  uint8_t n = 6;
  unsigned long start = millis();
  uint8_t ci = 0;

  while (millis() - start < duration) {
    for (int i = 0; i < NUM_LEDS; i++) {
      strip.setPixelColor(i, cols[ci]);
      strip.show();
      delay(30);
      if (millis() - start >= duration) return;
    }
    ci = (ci + 1) % n;
  }
}


void setup() {
  strip.begin();
  strip.setBrightness(BRIGHTNESS);
  strip.show(); 
  delay(500);
}

void loop() {
  rainbowWheel(EFFECT_DURATION);
  colorChase(EFFECT_DURATION);
  breathingGlow(EFFECT_DURATION);
  sparkle(EFFECT_DURATION);
  theaterChaseRainbow(EFFECT_DURATION);
  meteorRain(EFFECT_DURATION);
  fire(EFFECT_DURATION);
  colorWipe(EFFECT_DURATION);
}
