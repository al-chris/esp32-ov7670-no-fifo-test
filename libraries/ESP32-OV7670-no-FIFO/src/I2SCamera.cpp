#include "I2SCamera.h"
#include "Log.h"

int I2SCamera::blocksReceived = 0;
int I2SCamera::framesReceived = 0;
int I2SCamera::xres = 640;
int I2SCamera::yres = 480;
gpio_num_t I2SCamera::vSyncPin = (gpio_num_t)0;
intr_handle_t I2SCamera::i2sInterruptHandle = 0;
intr_handle_t I2SCamera::vSyncInterruptHandle = 0;
int I2SCamera::dmaBufferCount = 0;
int I2SCamera::dmaBufferActive = 0;
DMABuffer **I2SCamera::dmaBuffer = 0;
unsigned char* I2SCamera::frame = 0;
int I2SCamera::framePointer = 0;
int I2SCamera::frameBytes = 0;
volatile bool I2SCamera::stopSignal = false;

void IRAM_ATTR I2SCamera::i2sInterrupt(void* arg)
{
    I2S0.int_clr.val = I2S0.int_raw.val;
    blocksReceived++;
    unsigned char* buf = dmaBuffer[dmaBufferActive]->buffer;
    dmaBufferActive = (dmaBufferActive + 1) % dmaBufferCount;
    if(framePointer < frameBytes)
      for(int i = 0; i < xres * 4; i += 4)
      {
        frame[framePointer++] = buf[i + 2];
        frame[framePointer++] = buf[i];
      }
    if (blocksReceived == yres)
    {
      framePointer = 0;
      blocksReceived = 0;
      framesReceived++;
      if(stopSignal)
      {
        i2sStop();
        stopSignal = false;
      }
    }
}

void IRAM_ATTR I2SCamera::vSyncInterrupt(void* arg)
{
    gpio_intr_disable(vSyncPin);
    if (gpio_get_level(vSyncPin)) {
    }
    gpio_intr_enable(vSyncPin);
}

void I2SCamera::i2sStop()
{
    esp_intr_disable(i2sInterruptHandle);
    esp_intr_disable(vSyncInterruptHandle);
    i2sConfReset();
    I2S0.conf.rx_start = 0;
}

void I2SCamera::i2sRun()
{
    DEBUG_PRINTLN("I2S Run");
    while (gpio_get_level(vSyncPin) == 0);
    while (gpio_get_level(vSyncPin) != 0);

    esp_intr_disable(i2sInterruptHandle);
    i2sConfReset();
    blocksReceived = 0;
    dmaBufferActive = 0;
    framePointer = 0;
    DEBUG_PRINT("Sample count ");
    DEBUG_PRINTLN(dmaBuffer[0]->sampleCount());
    I2S0.rx_eof_num = dmaBuffer[0]->sampleCount();
    I2S0.in_link.addr = (uint32_t)&(dmaBuffer[0]->descriptor);
    I2S0.in_link.start = 1;
    I2S0.int_clr.val = I2S0.int_raw.val;
    I2S0.int_ena.val = 0;
    I2S0.int_ena.in_done = 1;
    esp_intr_enable(i2sInterruptHandle);
    esp_intr_enable(vSyncInterruptHandle);
    I2S0.conf.rx_start = 1;
}

bool I2SCamera::initVSync(int pin)
{
  DEBUG_PRINT("Initializing VSYNC... ");
  vSyncPin = (gpio_num_t)pin;
  gpio_set_intr_type(vSyncPin, GPIO_INTR_POSEDGE);
  gpio_intr_enable(vSyncPin);
  if(gpio_isr_register(&I2SCamera::vSyncInterrupt, (void*)"vSyncInterrupt", ESP_INTR_FLAG_INTRDISABLED | ESP_INTR_FLAG_IRAM, &vSyncInterruptHandle) != ESP_OK) 
  {
    DEBUG_PRINTLN("failed!");
    return false;
  }
  DEBUG_PRINTLN("done.");
  return true;
}

void I2SCamera::deinitVSync()
{
  esp_intr_disable(vSyncInterruptHandle);
}

bool I2SCamera::init(const int XRES, const int YRES, const int VSYNC, const int HREF, const int XCLK, const int PCLK, const int D0, const int D1, const int D2, const int D3, const int D4, const int D5, const int D6, const int D7)
{
  xres = XRES;
  yres = YRES;
  frameBytes = XRES * YRES * 2;
  frame = (unsigned char*)malloc(frameBytes);
  if(!frame)
  {
    DEBUG_PRINTLN("Not enough memory for frame buffer!");
    return false;
  }
  i2sInit(VSYNC, HREF, PCLK, D0, D1, D2, D3, D4, D5, D6, D7);
  dmaBufferInit(xres * 2 * 2);  //two bytes per dword packing, two bytes per pixel
  initVSync(VSYNC);
  return true;
}

