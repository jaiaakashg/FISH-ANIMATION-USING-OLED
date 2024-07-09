#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128   
#define SCREEN_HEIGHT 64 
#define OLED_RESET    -1  

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Fish parameters
struct Fish {
  float x1, y1;   
  float x2, y2;  
  float cx1, cy1; 
  float cx2, cy2;
  int size;
  int color;
  float speed;     
  float direction; 
  float depth;     
};

const int NUM_FISH = 5;
Fish fish[NUM_FISH];

int animationState = 0;           
unsigned long lastUpdateTime = 0; 
const int animationInterval = 50; 
const unsigned char PROGMEM fish_1[] = {
  B00011110,
  B00111100,
  B01111110,
  B11111111,
  B11111111,
  B01111110,
  B00111100,
  B00011110
};

const unsigned char PROGMEM fish_2[] = {
  B00011110,
  B00111100,
  B01111110,
  B11111111,
  B11111111,
  B01111110,
  B00111100,
  B00011110
};

void setup() {
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  display.clearDisplay();
  display.display();

  for (int i = 0; i < NUM_FISH; i++) {
    fish[i].x1 = random(SCREEN_WIDTH);  
    fish[i].y1 = random(SCREEN_HEIGHT);
    fish[i].x2 = random(SCREEN_WIDTH);  
    fish[i].y2 = random(SCREEN_HEIGHT);
    fish[i].cx1 = random(SCREEN_WIDTH);
    fish[i].cy1 = random(SCREEN_HEIGHT);
    fish[i].cx2 = random(SCREEN_WIDTH);
    fish[i].cy2 = random(SCREEN_HEIGHT);
    fish[i].size = random(6, 15);     
    fish[i].color = SSD1306_WHITE;     
    fish[i].speed = random(0.5, 2.0);  
    fish[i].direction = random(TWO_PI); 
    fish[i].depth = random(0.5, 1.0);  
  }
  lastUpdateTime = millis();
  animate();
}

void loop() {
}

void animate() {
  while (true) {
    unsigned long currentTime = millis();
    
    if (currentTime - lastUpdateTime >= animationInterval) {
      lastUpdateTime = currentTime;
      updateAnimation();
      drawFrame();
      display.display();
    }
  }
}

void updateAnimation() {
  for (int i = 0; i < NUM_FISH; i++) {
    float t = float(millis() % 5000) / 5000.0; 
    fish[i].x1 = bezierPoint(fish[i].x1, fish[i].cx1, fish[i].cx2, fish[i].x2, t);
    fish[i].y1 = bezierPoint(fish[i].y1, fish[i].cy1, fish[i].cy2, fish[i].y2, t);
  
    if (random(100) < 5) {
      fish[i].direction += random(-PI / 4, PI / 4);
    }
    fish[i].x1 += fish[i].speed * cos(fish[i].direction);
    fish[i].y1 += fish[i].speed * sin(fish[i].direction);
    fish[i].x1 = fmod(fish[i].x1 + SCREEN_WIDTH, SCREEN_WIDTH);
    fish[i].y1 = fmod(fish[i].y1 + SCREEN_HEIGHT, SCREEN_HEIGHT);
  }
  animationState = (animationState + 1) % 2;
}

void drawFrame() {
  display.clearDisplay();
  drawWater();
  drawFish();
}

void drawWater() {
  for (int y = SCREEN_HEIGHT / 2; y < SCREEN_HEIGHT; y++) {
    int lightIntensity = map(y, SCREEN_HEIGHT / 2, SCREEN_HEIGHT, 0, 255);
    display.drawFastHLine(0, y, SCREEN_WIDTH, SSD1306_WHITE); 
  }
}

void drawFish() {
  for (int i = 0; i < NUM_FISH; i++) {
    if (animationState == 0) {
      display.drawBitmap(fish[i].x1, fish[i].y1, fish_1, 8, 8, SSD1306_WHITE); 
    } else {
      display.drawBitmap(fish[i].x1, fish[i].y1, fish_2, 8, 8, SSD1306_WHITE); 
    }
  }
}

float bezierPoint(float a, float b, float c, float d, float t) {
  float mt = 1.0 - t;
  return mt * mt * mt * a + 3 * mt * mt * t * b + 3 * mt * t * t * c + t * t * t * d;
}

