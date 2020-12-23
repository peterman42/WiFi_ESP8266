/*
 * Server.h
 *
 *  Created on: Mar 20, 2016
 *      Author: peterman
 */
#ifndef WIFI_ESP8266_H_
#define WIFI_ESP8266_H_

#include "Arduino.h"
#include "SoftwareSerial.h"

#define AT_COMMAND_GET_IP_ADDRESS String("AT+CIFSR\r\n")
#define AT_COMMAND_LIST_APS String("AT+CWLAP\r\n")
#define AT_COMMAND_WIFI_MODE String("AT+CWMODE=<mode>\r\n")
#define AT_COMMAND_WIFI_MODE_INQUIRY String("AT+CWMODE?\r\n")
#define AT_COMMAND_JOIN_THE_AP String("AT+CWJAP=\"<ssid>\",\"<pwd>\"\r\n")
#define AT_COMMAND_JOIN_THE_AP_INQUIRY String("AT+CWJAP?\r\n")
#define AT_COMMAND_QUIT_THE_AP String("AT+CWQAP\r\n")
#define AT_COMMAND_PARAMETERS_OF_AP String("AT+CWSAP=\"<ssid>\",\"<pwd>\",\"<chl>\",\"<enc>\"\r\n")
#define AT_COMMAND_SET_AS_SERVER String("AT+CIPSERVER=<mode>,<port>\r\n")
#define AT_COMMAND_ENABLE_CONNECTIONS String("AT+CIPMUX=<mode>\r\n")
#define AT_COMMAND_RESET String("AT+RST\r\n")
#define AT_COMMAND_RECEIVE String("+IPD\r\n")
#define AT_COMMAND_SEND String("AT+CIPSEND=<id>,<data_length>")
#define TIMEOUT 2000
#define RESPONSE_ERROR String("ERROR")

enum WIFI_MODE_ENUM
{
	STATION = 1,
	AP=2,
	BOTH=3
};

enum WIFI_AP_ENCRYPTION_ENUM
{
	OPEN = 0,
	WPA_PSK = 2,
	WPA2_PSK = 3,
	WPA_WPA2_PSKreceive = 4
};

enum WIFI_SET_AS_SERVER
{
	CLOSE_SERVER = 0,
	OPEN_SERVER = 1
};

enum WIFI_ENABLE_CONNECTIONS_MODE
{
	SINGLE_CONNECTION=0,
	MULTIPLE_CONNECTIONS=1
};

enum WIFI_AP_CHANNELS_ENUM
{
	CHANNEL1 = 1,
	CHANNEL2,
	CHANNEL3,
	CHANNEL4,
	CHANNEL5,
	CHANNEL6,
	CHANNEL7,
	CHANNEL8,
	CHANNEL9,
	CHANNEL10,
	CHANNEL11,
	CHANNEL12,
	CHANNEL13
};

struct client_data {
    bool received;
    String id;
    String payload;
    int length;

    client_data()
        : received(false)
        , id(-1)
        , payload("")
        , length(0)
    {}
};

class WiFi_ESP8266
{

public:
	SoftwareSerial wifi;
	bool debug;

	WiFi_ESP8266():wifi(0,1),debug(true)
	{
		Serial.begin(115200);
		wifi.begin(115200);
		while (!wifi)
		{
			; // wait for serial port to connect. Needed for native USB port only
		}
	}

	WiFi_ESP8266(int rx_pin, int tx_pin, bool debug=true):
		wifi(rx_pin,tx_pin),
		debug(debug)
	{
		Serial.begin(115200);
		wifi.begin(115200);
		while (!wifi)
		{
			; // wait for serial port to connect. Needed for native USB port only
		}

	}
	~WiFi_ESP8266(){}

	String getIPAddress();
        String getAPsList();
	bool setMode(WIFI_MODE_ENUM mode);
	String getMode();
	bool connectToAP(const char* ssid, const char* pass);
	String getConnectedAPinfo();
	bool isConnectedToAP(String ssid);
	bool disconnectFromAP();
	String setParametersofAP(const char* ssid, const char* pwd, WIFI_AP_CHANNELS_ENUM channel, WIFI_AP_ENCRYPTION_ENUM encryption);
	String sendMessage(const String& message, const int timeout, bool debug);
	bool enableConnections(WIFI_ENABLE_CONNECTIONS_MODE mode);
        bool openServer(int port);
	bool closeServer();
	bool reboot();
        client_data receive();
        bool send(String id, String data);
private:
     bool setAsServer(WIFI_SET_AS_SERVER mode, int port);
};
#endif /* WIFI_ESP8266_H_ */
