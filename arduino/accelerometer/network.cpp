
#include "network.h"
#include "storage.h"

WebServer server(80);

Twebsite g_website;

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

void send_rootpage()
{
	char temp[BUF_LEN_BYTE];
	int sec = millis() / 1000;
	int min = sec / 60;
	int hr = min / 60;

	snprintf(temp, BUF_LEN_BYTE,
    "<html>\
	<head>\
    	<title>Accelerometer</title>\
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
	hr, min%60, sec%60
	);
	server.send(200, "text/html", temp);
}

void handleRoot()
{
	send_rootpage();
}

void handleStart()
{
	g_website.setStartReq(1);
	send_rootpage();
}

void handleStop()
{
	g_website.setStopReq(1);
	send_rootpage();
}

void handleClear()
{
	g_website.setClearReq(1);
	send_rootpage();
}

void handleCalibrate()
{
	g_website.setCalibReq(1);
	send_rootpage();
}

void handleDownload()
{
	Serial.println("download button handler start");

	int32_t startidx;
	int32_t len;
	g_data.get_record(0, startidx, len);

	if( len <= 0 )
	{
		send_rootpage();
		return;
	}
	if( len > 10 )
	{
		len = 10;
	} 

	char temp[BUF_LEN_BYTE] = "";

	strcat(temp,
	"<html>\
	<head>\
    	<title>ESP32 Demo</title>\
    	<style>\
     		body { background-color: #FFFFFF; font-family: Arial, Helvetica, Sans-Serif; Color: #000000; }\
    	</style>\
  	</head>\
	<body>"
	);

	for(int i=0; i<len; i++)
	{
		uint64_t entry = g_data.get_entry(startidx + i);
		int32_t x = (entry & 0xFFFFF) - (1<<19);
		int32_t y = ((entry>>20) & 0xFFFFF) - (1<<19);
		int32_t z = ((entry>>40) & 0xFFFFF) - (1<<19);
		
		char line[LINE_LEN_CHAR];
		snprintf(line, LINE_LEN_CHAR, "<p>%20d%20d%20d</p>", x, y, z);
		strcat(temp, line);
	}

	strcat(temp,	
	"</body>\
	</html>"
	);

	server.send(200, "text/html", temp);
}


// ###################################################################################################

void Twebsite::init(const char* ssid, const char* password)
{
	clear_requests();

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
	server.on("/start", handleStart);
	server.on("/stop", handleStop);
	server.on("/clear", handleClear);
	server.on("/calibrate", handleCalibrate);
	server.on("/download", handleDownload);
	server.onNotFound(handleNotFound);	
	server.begin();
	Serial.println("HTTP server started");	
}

void Twebsite::run()
{
    server.handleClient();
}

void Twebsite::clear_requests()
{
	start_requested = 0;
    stop_requested = 0;
    clear_requested = 0;
    calib_requested = 0;
    download_requested = 0;
}