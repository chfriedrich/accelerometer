/*
TTGO T7 Settings:
ESP32 Dev Module
Flash mode QIO
Flash size 4MB
CPU Frequency 240MHz
Flash Frequency 80MHz
*/

#include <SPI.h>
#include <cf_adxl357.h>

// ---------------------------------------------------------

// SPI Pin Numbers
#define PIN_SCK  18
#define PIN_MISO 19
#define PIN_MOSI 23
#define PIN_SS   26

#define READOUT_INTERVAL_MS 2

// ---------------------------------------------------------

SPISettings spiSettings(10000000, MSBFIRST , SPI_MODE0);
adxl357 adxl;
uint32_t tlast = 0;

void setup()
{
	// Give peripherials time to start
	delay(100);

	Serial.begin(1000000);

	// Configure adxl
	adxl.setPins(PIN_MISO, PIN_MOSI, PIN_SCK, PIN_SS);
	adxl.init();
	delay(10);
	adxl.writeRange(TEN_G);
	adxl.enableSensor();
	delay(10);

	Serial.print("Device ID:      ");
	Serial.println(adxl.readDeviceID());
	Serial.print("Device Version: ");
	Serial.println(adxl.readDeviceVersion());
}

void loop()
{
	uint32_t tnow = millis();
	if( tnow >= tlast + READOUT_INTERVAL_MS )
	{
		tlast = tnow;
		adxl.readAllFromFifo();
	}
}
