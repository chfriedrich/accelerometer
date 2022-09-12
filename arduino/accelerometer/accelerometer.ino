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
#include "accelerometer.h"
#include "storage.h"
#include "network.h"

// ---------------------------------------------------------
 
adxl357 adxl;
storage data;
network net;

int32_t t_readout   = 0;
int32_t t_webserver = 0;

void setup()
{ 
	Serial.begin(1000000);
	delay(100);    // Give peripherials time to start

	Serial.println("Starting up");

	// allocate space in PSRAM
	data.init(2*1024*1024);   // 2MB ... approx. 524000 int32 values
	Serial.printf("Free PSRAM after Buffer allocation: %d\n", ESP.getFreePsram());

	// Configure adxl
	adxl.setPins(PIN_MISO, PIN_MOSI, PIN_SCK, PIN_SS);
	adxl.init();
	delay(10);
	adxl.writeRange(TEN_G);
	adxl.enableSensor();
	delay(10);
	adxl.measureOffset();

	Serial.print("Device ID:      ");
	Serial.println(adxl.readDeviceID());
	Serial.print("Device Version: ");
	Serial.println(adxl.readDeviceVersion());

	net.init(WIFI_SSID, WIFI_PASSWORD);

	t_readout = micros() - READOUT_INTERVAL_US;
	t_webserver = micros() - WEBSERVER_INTERVAL_US;
}

void loop()
{
	uint32_t tnow = micros();
	if( tnow >= t_readout + READOUT_INTERVAL_US )
	{
		t_readout = tnow;

		int32_t xarr[BUFFER_SIZE_PER_AXIS];
		int32_t yarr[BUFFER_SIZE_PER_AXIS];
		int32_t zarr[BUFFER_SIZE_PER_AXIS];
		uint8_t len;
		adxl.readAllFromFifo(xarr, xarr, xarr, &len);
		data.addMultiple(xarr, yarr, zarr, len);
	}

	if( tnow >= t_webserver + WEBSERVER_INTERVAL_US )
	{
		t_webserver = tnow;
		net.run();
	}
}

