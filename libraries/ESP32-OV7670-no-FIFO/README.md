ESP32-OV7670-no-FIFO (no-FIFO) driver for ESP32

This library provides a driver to interface OV7670 non-FIFO camera modules with the ESP32 using I2S parallel capture and DMA.

Features:
- XCLK generation using RMT (reliable across ESP32 core versions)
- SCCB (I2C) camera configuration
- I2S parallel capture with DMA buffers
- BMP header generator for streaming over HTTP

Usage:
- Copy the `ESP32-OV7670-no-FIFO` folder into your Arduino `libraries/` folder or include it in a project workspace under `libraries/ESP32-OV7670-no-FIFO`.
- Include `OV7670.h` in your sketch and instantiate the camera as shown in the `examples/CameraWebServer` example.

Examples:
- `examples/CameraWebServer/CameraWebServer.ino` — A web server example that serves BMP frames captured from the OV7670 (QQVGA RGB565).

License: MIT
