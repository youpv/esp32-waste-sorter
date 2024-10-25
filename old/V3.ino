#include "esp_camera.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Define CAMERA_MODEL and include camera_pins.h
#define CAMERA_MODEL_WROVER_KIT
#include "camera_pins.h"

// Define the boundary as a macro for string concatenation
#define BOUNDARY "----WebKitFormBoundary7MA4YWxkTrZu0gW"

// WiFi credentials and API endpoint
const char* ssid_Router     = "IT hertz when IP";
const char* password_Router = "wachtwoord";
const char* apiEndpoint     = "https://j7mp36xg-3000.euw.devtunnels.ms/api/upload";

const int buttonPin = 13;  // Button connected to pin 13
bool buttonPressed = false;
bool imageReady = false;

camera_config_t config;
bool wifiConnected = false;

HTTPClient http;

// Define head and tail for multipart/form-data
const char* head = "--" BOUNDARY "\r\n"
                   "Content-Disposition: form-data; name=\"image\"; filename=\"image.jpg\"\r\n"
                   "Content-Type: image/jpeg\r\n\r\n";

const char* tail = "\r\n--" BOUNDARY "--\r\n";

camera_fb_t *fb = NULL;  // Declare fb globally

// LCD setup
LiquidCrystal_I2C lcd(0x27, 20, 4);  // Set the LCD address to 0x27 for a 20 chars and 4 line display


void camera_init() {
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.frame_size = FRAMESIZE_VGA; // Adjust as needed
  config.pixel_format = PIXFORMAT_JPEG;
  config.grab_mode = CAMERA_GRAB_WHEN_EMPTY;
  config.fb_location = CAMERA_FB_IN_PSRAM;
  config.jpeg_quality = 15; // Lower quality for smaller size
  config.fb_count = 1; // Single framebuffer
}

void connectToWiFi() {
  Serial.printf("Connecting to WiFi: %s\n", ssid_Router);
  WiFi.begin(ssid_Router, password_Router);
  int attempts = 0;

  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi connected");
    Serial.printf("IP address: %s\n", WiFi.localIP().toString().c_str());
    wifiConnected = true;
  } else {
    Serial.println("\nWiFi connection failed. Retrying...");
    wifiConnected = false;
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("Starting setup...");

  pinMode(buttonPin, INPUT_PULLUP);  // Set button pin as input with internal pull-up resistor

  camera_init();
  
  unsigned long startTime = millis();
  esp_err_t err = esp_camera_init(&config);
  unsigned long endTime = millis();
  Serial.printf("Camera initialization time: %lu ms\n", endTime - startTime);
  
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x\n", err);
    while (1) {
      delay(1000); // Halt execution
    }
  }
  
  Serial.println("Camera initialized successfully");
}

void loop() {
  if (!wifiConnected) {
    connectToWiFi();
  }

  if (wifiConnected) {
    // Check button state
    if (digitalRead(buttonPin) == LOW && !buttonPressed) {
      buttonPressed = true;
      captureImage();
    } else if (digitalRead(buttonPin) == HIGH) {
      buttonPressed = false;
    }

    // If an image is ready, upload it
    if (imageReady) {
      uploadImage();
    }

    // Check if WiFi is still connected
    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("WiFi disconnected. Attempting to reconnect...");
      wifiConnected = false;
    }
  }

  delay(100); // Prevent watchdog resets
}

void captureImage() {
  unsigned long startTime, endTime;

  Serial.println("Capturing image...");
  startTime = millis();
  
  if (fb) {
    esp_camera_fb_return(fb);  // Return the previous frame buffer if it exists
  }
  
  fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("Camera capture failed");
    return;
  }
  endTime = millis();
  Serial.printf("Image capture time: %lu ms\n", endTime - startTime);
  
  imageReady = true;
}

void uploadImage() {
  if (!fb) {
    Serial.println("No image to upload");
    return;
  }

  unsigned long startTime, endTime;
  Serial.println("Uploading image...");
  startTime = millis();

  // Assemble the multipart/form-data body
  size_t headLen = strlen(head);
  size_t tailLen = strlen(tail);
  size_t totalLen = headLen + fb->len + tailLen;

  // Allocate buffer for the entire body
  uint8_t* body = (uint8_t*) malloc(totalLen);
  if (!body) {
    Serial.println("Failed to allocate memory for HTTP body");
    return;
  }

  // Copy head, image data, and tail into the buffer
  memcpy(body, head, headLen);
  memcpy(body + headLen, fb->buf, fb->len);
  memcpy(body + headLen + fb->len, tail, tailLen);

  // Initialize HTTPClient
  if (http.begin(apiEndpoint)) {
    // Add necessary headers
    http.addHeader("Content-Type", "multipart/form-data; boundary=" BOUNDARY);
    http.addHeader("Content-Length", String(totalLen));

    // Send POST request
    int httpCode = http.POST(body, totalLen);

    // Free the allocated memory
    free(body);

    endTime = millis();
    Serial.printf("Image upload time: %lu ms\n", endTime - startTime);

    if (httpCode > 0) {
      Serial.printf("HTTP Response code: %d\n", httpCode);
      String payload = http.getString();
      Serial.println(payload);

      unsigned long parseStart = millis();
      // Use StaticJsonDocument for efficiency
      StaticJsonDocument<1024> doc;
      DeserializationError error = deserializeJson(doc, payload);
      unsigned long parseEnd = millis();
      Serial.printf("JSON parsing time: %lu ms\n", parseEnd - parseStart);

      if (error) {
        Serial.print("deserializeJson() failed: ");
        Serial.println(error.c_str());
        http.end();
        return;
      }

      if (doc.containsKey("object")) {
        String object = doc["object"].as<String>();
        Serial.print("Detected Object: ");
        Serial.println(object);
      }

      const char* keys[] = {"plastic", "paper", "cardboard", "bio_waste", "mixed_waste", "metal", "glass", "return_to_store"};
      for (const char* key : keys) {
        if (doc.containsKey(key) && doc[key].as<bool>()) {
          Serial.printf("Category: %s\n", key);
          break;
        }
      }
    } else {
      Serial.printf("HTTP POST failed, error: %s\n", http.errorToString(httpCode).c_str());
    }

    // End the HTTP connection
    http.end();
  } else {
    Serial.println("Unable to connect to the server");
  }

  imageReady = false;  // Reset the flag after uploading
}