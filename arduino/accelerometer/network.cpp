
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

void handleRoot()
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
        <p><a href=\"/dlpretty\"><button>Download pretty</button></a></p>\
		<p><a href=\"/dlfast\"><button>Download fast</button></a></p>\
        <p></p>\
        <p>Uptime: %02d:%02d:%02d</p>\
	</body>\
	</html>",
	hr, min%60, sec%60
	);
	server.send(200, "text/html", temp);
}

void handleStart()
{
	g_website.setStartReq(1);
	server.sendHeader("Location", "/", true);  
	server.send(302, "text/plain", "");
}

void handleStop()
{
	g_website.setStopReq(1);
	server.sendHeader("Location", "/", true);  
	server.send(302, "text/plain", "");
}

void handleClear()
{
	g_website.setClearReq(1);
	server.sendHeader("Location", "/", true);  
	server.send(302, "text/plain", "");
}

void handleCalibrate()
{
	g_website.setCalibReq(1);
	server.sendHeader("Location", "/", true);  
	server.send(302, "text/plain", "");
}

void handleDlPretty()
{
	int32_t startidx;
	int32_t len;
	g_data.get_record(0, startidx, len);

	if( len <= 0 )
	{
		server.sendHeader("Location", "/", true);  
		server.send(302, "text/plain", "");
		return;
	}

	String firstline = "nr; x; y; z\n";

	server.setContentLength(CONTENT_LENGTH_UNKNOWN);
    server.send ( 200, "text/plain", firstline.c_str());

	int32_t i;

	// max. 100 char per line, d.h. 100 byte bei utf8
	// 5kB pro Paket sind ok, d.h. 50 lines

	while( i < len )
	{
		String packet = "";
		for(int32_t j=0; j<DATA_LINES_PER_SEND; j++)
		{
			if ( i+j >= len-1 )  break;
			uint64_t entry = g_data.get_entry(startidx + i + j);
			int32_t x = (int32_t)(entry & 0xFFFFF) - (1<<19);
			int32_t y = (int32_t)((entry>>20) & 0xFFFFF) - (1<<19);
			int32_t z = (int32_t)((entry>>40) & 0xFFFFF) - (1<<19);
			packet += String(i+j) + "; " + String(x) + "; " + String(y) + "; " + String(z) + "\n";
		}
		server.sendContent(packet.c_str());
		i += DATA_LINES_PER_SEND;
	}
}

void handleDlFast()
{
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
	server.on("/dlpretty", handleDlPretty);
	server.on("/dlfast", handleDlFast);
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
}