#include <Adafruit_NeoPixel.h>
#include <OneButton.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#define BUTTON_PIN 0   // GPIO0，按键引脚
#define LED_PIN 5      // GPIO5，灯带控制引脚
#define NUM_LEDS 60    // 灯带上LED的数量
#define BRIGHTNESS_STEP 10 // 亮度调整步长

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);
OneButton button(BUTTON_PIN, true);

const uint8_t colors[][3] = {
  {255, 255, 255}, // 白光
  {255, 235, 52}, // 淡黄光
  {52, 84, 255},   // 淡蓝光
};
const uint8_t colorsCount = sizeof(colors) / sizeof(colors[0]);

int brightness = 60; // 默认亮度，0-100的值
int colorR = 255;    // 默认红色分量，0-255的值
int colorG = 255;    // 默认绿色分量，0-255的值
int colorB = 255;    // 默认蓝色分量，0-255的值

// OTA参数
const char* ssid = "LED_Controller"; // 热点名称
const char* password = "33445566"; // 热点密码

void setup() {
  Serial.begin(115200);
  Serial.println("Booting");

  strip.begin();
  strip.show(); // 初始化灯带，全部关闭
  button.attachClick(toggleLED); // 单击按键切换灯带开关
  button.attachDoubleClick(changeColor); // 双击按键切换颜色
  button.attachDuringLongPress(adjustBrightness); // 长按按键调整亮度

  // 开启热点
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);
  
  // Port defaults to 8266
  // ArduinoOTA.setPort(8266);

  // Hostname defaults to esp8266-[ChipID]
  // ArduinoOTA.setHostname("myesp8266");

  // No authentication by default
  // ArduinoOTA.setPassword("admin");

  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else {  // U_FS
      type = "filesystem";
    }

    // NOTE: if updating FS this would be the place to unmount FS using FS.end()
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });
  ArduinoOTA.begin();
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.softAPIP());
}

void loop() {
  button.tick();
  ArduinoOTA.handle();
}

void toggleLED() {
  static bool isOn = false;
  isOn = !isOn;
  
  if (isOn) {
    strip.fill(strip.Color(colorR * brightness / 100, colorG * brightness / 100, colorB * brightness / 100), 0, NUM_LEDS); // 打开灯带，设置亮度
  } else {
    strip.fill(strip.Color(0, 0, 0), 0, NUM_LEDS); // 关闭灯带
  }
  
  strip.show();
}

void changeColor() {
  static uint8_t colorIndex = 0;
  colorIndex = (colorIndex + 1) % colorsCount;
  colorR = colors[colorIndex][0];
  colorG = colors[colorIndex][1];
  colorB = colors[colorIndex][2];
  strip.fill(strip.Color(colorR * brightness / 100, colorG * brightness / 100, colorB * brightness / 100), 0, NUM_LEDS);
  strip.show();
}

void adjustBrightness() {
  static unsigned long lastPressTime = 0;
  unsigned long currentTime = millis();
  
  if (currentTime - lastPressTime > 500) {
    // 长按超过0.5秒时开始调整亮度
    brightness += BRIGHTNESS_STEP;
    if (brightness > 100) {
      brightness = 0; // 亮度循环，从0开始
    }
    
    strip.fill(strip.Color(colorR * brightness / 100, colorG * brightness / 100, colorB * brightness / 100), 0, NUM_LEDS);
    strip.show();
    
    lastPressTime = currentTime;
  }
}
