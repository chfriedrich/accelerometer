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

int32_t time_readout   = 0;
int32_t time_webserver = 0;

int8_t state = STATE_IDLE;

void setup()
{ 
	Serial.begin(1000000);
	delay(100);    // Give peripherials time to start

	Serial.println("Starting up");

	// allocate space in PSRAM
	g_data.init(4*1024*1024);   // 4MB 
	Serial.printf("Free PSRAM after Buffer allocation: %d\n", ESP.getFreePsram());

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

	g_website.init(WIFI_SSID, WIFI_PASSWORD);

	adxl.measureOffset();

	time_readout   = micros() - READOUT_INTERVAL_US;
	time_webserver = micros() - WEBSERVER_INTERVAL_US;
}

int32_t maxi = 0;
int32_t mini = 0;

void loop()
{
	uint32_t tnow = micros();
	if( tnow >= time_readout + READOUT_INTERVAL_US )
	{
		time_readout = tnow;

		switch(state)
		{
			case STATE_IDLE:
				if(g_website.isCalibReq())
				{	
					g_website.clear_requests();
					adxl.measureOffset();      // ist momentan noch blocking
				}
				if(g_website.isClearReq())
				{
					g_website.clear_requests();
					g_data.clear();
				}
				if(g_website.isStartReq())
				{
					g_website.clear_requests();
					g_data.start_record();
					state = STATE_RECORDING;
				}  
				break;

			case STATE_RECORDING:
				int32_t xarr[BUFFER_SIZE_PER_AXIS];
				int32_t yarr[BUFFER_SIZE_PER_AXIS];
				int32_t zarr[BUFFER_SIZE_PER_AXIS];
				uint8_t len;
				adxl.readAllFromFifo(xarr, yarr, zarr, len);
				g_data.addMultiple(xarr, yarr, zarr, len);

				for(int i=0; i<len; i++)
				{
					if(yarr[i]>maxi) maxi=yarr[i];
					if(yarr[i]<mini) mini=yarr[i];
				}

				if(g_website.isStopReq())
				{
					Serial.println("Maximum: " + String(maxi));
					Serial.println("Minimum: " + String(mini));

					g_website.clear_requests();
					g_data.end_record();
					state = STATE_IDLE;
				}
				break;
		}

	}

	if( tnow >= time_webserver + WEBSERVER_INTERVAL_US )
	{
		time_webserver = tnow;
		g_website.run();
	}
}

