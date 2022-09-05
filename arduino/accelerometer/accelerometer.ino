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

#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>

// ---------------------------------------------------------

const char *ssid = "netzwerk";
const char *password = "Muckendorf1";

// ---------------------------------------------------------

adxl357 adxl;
storage data;
WebServer server(80);

int32_t t_readout   = 0;
int32_t t_webserver = 0;

void setup()
{
	delay(100); 
	Serial.begin(1000000);
	delay(100);    // Give peripherials time to start

	Serial.printf("Starting up\n");

	Serial.printf("Total heap: %d\n", ESP.getHeapSize());
	Serial.printf("Free heap: %d\n", ESP.getFreeHeap());
	Serial.printf("Total PSRAM: %d\n", ESP.getPsramSize());
	Serial.printf("Free PSRAM: %d\n", ESP.getFreePsram());

	Serial.printf("Allocating Buffer\n");
	data.init(2*1024*1024);   // 2MB ... approx. 524000 int32 values

	Serial.printf("Total PSRAM: %d\n", ESP.getPsramSize());
	Serial.printf("Free PSRAM: %d\n", ESP.getFreePsram());

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

	// setup wifi
	WiFi.mode(WIFI_STA);
  	WiFi.begin(ssid, password);
	Serial.println("Connecting to wifi ");
	while (WiFi.status() != WL_CONNECTED)
	{
    	delay(500);
    	Serial.print(".");
  	}
	Serial.println("");
  	Serial.print("Connected to ");
  	Serial.println(ssid);
  	Serial.print("IP address: ");
  	Serial.println(WiFi.localIP());
	Serial.println("");
  	
	if (MDNS.begin("esp32"))
    	Serial.println("MDNS responder started");
	Serial.println("");
	
	// setup web server
	server.on("/", handleRoot);
	server.onNotFound(handleNotFound);	
	server.begin();
	Serial.println("HTTP server started");

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

	if( tnow >= t_webserver + READOUT_INTERVAL_US )
	{
		t_webserver = tnow;
		server.handleClient();
	}
}

void handleNotFound()
{
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }

  server.send(404, "text/plain", message);
}

void handleRoot()
{
	char temp[400];
	int sec = millis() / 1000;
	int min = sec / 60;
	int hr = min / 60;

	snprintf(temp, 400,
    "<html>\
	<head>\
    	<meta http-equiv='refresh' content='5'/>\
    	<title>ESP32 Demo</title>\
    	<style>\
     		body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
    	</style>\
  	</head>\
	<body>\
    	<h1>Hello from ESP32!</h1>\
    	<p>Uptime: %02d:%02d:%02d</p>\
	</body>\
	</html>",
	hr, min % 60, sec % 60
	);
	server.send(200, "text/html", temp);
}