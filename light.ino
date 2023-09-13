#include <Adafruit_NeoPixel.h>
#include <OneButton.h>

#define BUTTON_PIN 0   // GPIO0，按键引脚
#define LED_PIN 5      // GPIO5，灯带控制引脚
#define NUM_LEDS 60    // 灯带上LED的数量
#define BRIGHTNESS_STEP 10 // 亮度调整步长

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);
OneButton button(BUTTON_PIN, true);

int brightness = 153; // 默认亮度，60%亮度的白光

void setup() {
  strip.begin();
  strip.show(); // 初始化灯带，全部关闭
  button.attachClick(toggleLED); // 单击按键切换灯带开关
  button.attachDoubleClick(changeColor); // 双击按键切换颜色
  button.attachDuringLongPress(adjustBrightness); // 长按按键调整亮度
}

void loop() {
  button.tick();
}

void toggleLED() {
  static bool isOn = false;
  isOn = !isOn;
  
  if (isOn) {
    strip.fill(strip.Color(brightness, brightness, brightness), 0, NUM_LEDS); // 打开灯带，设置亮度
  } else {
    strip.fill(strip.Color(0, 0, 0), 0, NUM_LEDS); // 关闭灯带
  }
  
  strip.show();
}

void changeColor() {
  static uint8_t colorIndex = 0;
  uint8_t colors[][3] = {
    {255, 255, 255}, // 白光
    {255, 235, 52}, // 淡黄光
    {52, 84, 255},   // 淡蓝光
    {255, 255, 0},
    {0, 255, 255}
  };
  colorIndex = (colorIndex + 1) % 2;
  strip.fill(strip.Color(colors[colorIndex][0], colors[colorIndex][1], colors[colorIndex][2]), 0, NUM_LEDS);
  strip.show();
}

void adjustBrightness() {
  static unsigned long lastPressTime = 0;
  unsigned long currentTime = millis();
  
  if (currentTime - lastPressTime > 500) {
    // 长按超过0.5秒时开始调整亮度
    brightness += BRIGHTNESS_STEP;
    if (brightness > 255) {
      brightness = 0; // 亮度循环，从0开始
    }
    
    strip.fill(strip.Color(brightness, brightness, brightness), 0, NUM_LEDS);
    strip.show();
    
    lastPressTime = currentTime;
  }
}
