//
// ESP32 OV7670 (No-FIFO) Camera Streaming Web Server
//
// This code is based on the work by bitluni (ESP32CameraI2S) and has been
// adapted using the community-validated pinout for reliable operation.
// It requires the bitluni/ESP32CameraI2S library and its dependencies.
//

#include "OV7670.h"       // Core camera library from bitluni
#include <WiFi.h>         // ESP32 WiFi functionality
#include <WiFiClient.h>   // WiFi client/server functionality
#include <WiFiServer.h>   //
#include "BMP.h"          // Helper for creating BMP image headers

// --- Pinout Configuration (Definitive Mapping) ---
// This pinout has been validated to work reliably with ESP32-WROOM modules.
// See Section 2 of the accompanying report for a detailed rationale.
const int SIOD = 21;      // SCCB Data (SDA)
const int SIOC = 22;      // SCCB Clock (SCL)
const int VSYNC = 34;     // Vertical Sync (Input-Only Pin)
const int HREF = 35;      // Horizontal Reference (Input-Only Pin)
const int XCLK = 32;      // System Master Clock (Output)
const int PCLK = 33;      // Pixel Clock (Input)
const int D0 = 27;        // Data Bus Bit 0
const int D1 = 19;        // Data Bus Bit 1
const int D2 = 18;        // Data Bus Bit 2
const int D3 = 15;        // Data Bus Bit 3
const int D4 = 14;        // Data Bus Bit 4
const int D5 = 13;        // Data Bus Bit 5
const int D6 = 12;        // Data Bus Bit 6
const int D7 = 4;         // Data Bus Bit 7

// --- Wi-Fi Credentials ---
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// --- Global Objects ---
OV7670 *camera;                       // Pointer to the camera driver object
WiFiServer server(80);                // Web server object on port 80
unsigned char bmpHeader; // Buffer for the BMP file header

// Function to handle incoming web client requests
void handleClient() {
  WiFiClient client = server.available();
  if (!client) {
    return;
  }

  Serial.println("New Client.");
  String currentLine = "";
  while (client.connected()) {
    if (client.available()) {
      char c = client.read();
      if (c == '\n') {
        // If we've got a blank line, we have reached the end of the client HTTP request,
        // so we can send a response.
        if (currentLine.length() == 0) {
          // The main page request serves an HTML page with auto-refreshing images.
          client.println("HTTP/1.1 200 OK");
          client.println("Content-type:text/html");
          client.println("Connection: close");
          client.println();
          client.print(
            "<!DOCTYPE html><html><head><title>ESP32-OV7670 Camera</title>"
            "<meta http-equiv='refresh' content='0.1'></head>" // Refresh every 100ms for video effect
            "<body><h1 style='font-family: sans-serif;'>ESP32-OV7670 Stream</h1>"
            "<img src='/camera' width='320' height='240'>" // Display image scaled to QVGA
            "</body></html>"
          );
          client.println();
          break;
        } else {
          // Clear the line for the next part of the request
          currentLine = "";
        }
      } else if (c!= '\r') {
        currentLine += c;
      }

      // Check if the request is for the camera image stream
      if (currentLine.endsWith("GET /camera")) {
        // Capture one frame from the camera. The oneFrame() function blocks until
        // the I2S/DMA engine has delivered a complete frame to the buffer.
        camera->oneFrame();

        // Send the HTTP response headers for a BMP image
        client.println("HTTP/1.1 200 OK");
        client.println("Content-Type: image/bmp");
        client.println("Content-Length: " + String(BMP::headerSize + camera->xres * camera->yres * 2));
        client.println("Connection: close");
        client.println();

        // Send the BMP header first, then the raw pixel data
        client.write(bmpHeader, BMP::headerSize);
        client.write(camera->frame, camera->xres * camera->yres * 2);
      }
    }
  }
  // Close the connection
  client.stop();
  Serial.println("Client Disconnected.");
}

void setup() {
  Serial.begin(115200);
  Serial.println("\n--- ESP32 OV7670 (No-FIFO) Camera Test ---");

  // Instantiate the camera driver object. This performs all the complex
  // hardware initialization: XCLK generation, SCCB configuration, and
  // setting up the I2S/DMA data capture pipeline.
  camera = new OV7670(OV7670::Mode::QQVGA_RGB565,
                      SIOD, SIOC, VSYNC, HREF, XCLK, PCLK,
                      D0, D1, D2, D3, D4, D5, D6, D7);

  // Construct the BMP header for the specified resolution (QQVGA, 160x120)
  // and pixel format (16-bit RGB565).
  BMP::construct16BitHeader(bmpHeader, camera->xres, camera->yres);

  // --- Connect to Wi-Fi ---
  Serial.printf("Connecting to %s ", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status()!= WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected!");

  // --- Start Web Server ---
  server.begin();
  Serial.print("Web Server Started. Go to: http://");
  Serial.println(WiFi.localIP());
}

void loop() {
  // The main loop's only job is to listen for and handle incoming web clients.
  // The camera frame capture is triggered on-demand within the handleClient() function
  // when a request for '/camera' is received.
  handleClient();
}