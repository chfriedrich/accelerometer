
#include "network.h"

WebServer server(80);

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
	char temp[BUF_LEN_BYTE];
	int sec = millis() / 1000;
	int min = sec / 60;
	int hr = min / 60;

	snprintf(temp, BUF_LEN_BYTE,
    "<html>\
	<head>\
    	<meta http-equiv='refresh' content='5'/>\
    	<title>ESP32 Demo</title>\
    	<style>\
     		body { background-color: #FFFFFF; font-family: Arial, Helvetica, Sans-Serif; Color: #000000; }\
    	</style>\
  	</head>\
	<body>\
    	<h1>Accelerometer</h1>\
        <p><a href=\"/start\"><button>Start</button></a></p>\
        <p><a href=\"/stop\"><button>Stop</button></a></p>\
        <p><a href=\"/clear\"><button>Clear</button></a></p>\
        <p><a href=\"/calib\"><button>Calibrate</button></a></p>\
        <p><a href=\"/download\"><button>Download</button></a></p>\
        <p></p>\
        <p>Uptime: %02d:%02d:%02d</p>\
	</body>\
	</html>",
	hr, min % 60, sec % 60
	);
	server.send(200, "text/html", temp);
}

// ###################################################################################################

void network::init(const char* ssid, const char* password)
{
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
  	
	if (MDNS.begin("esp32"))
    	Serial.println("MDNS responder started");
	
	// setup web server
	server.on("/", handleRoot);
	server.onNotFound(handleNotFound);	
	server.begin();
	Serial.println("HTTP server started");	
}

void network::run()
{
    server.handleClient();
}
