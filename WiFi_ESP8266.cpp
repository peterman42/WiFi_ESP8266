/*
 * WiFi_ESP8266.cpp
 *
 *  Created on: Mar 20, 2016
 *      Author: peterman
 */
#include "WiFi_ESP8266.h"

String WiFi_ESP8266::getIPAddress()
{
	String ip = sendMessage(AT_COMMAND_GET_IP_ADDRESS, TIMEOUT, debug);
	return ip;
}

String WiFi_ESP8266::getAPsList()
{
	String aps = "";

        aps = sendMessage(AT_COMMAND_LIST_APS, TIMEOUT, debug);

	return aps;
}

bool WiFi_ESP8266::setMode(WIFI_MODE_ENUM mode)
{
	String wmode = AT_COMMAND_WIFI_MODE;
	wmode.replace("<mode>", String(mode));
	String wifi_mode = sendMessage(wmode,TIMEOUT,debug);

	return (wifi_mode != "" && wifi_mode != NULL);
}

String WiFi_ESP8266::getMode(){

	return sendMessage(AT_COMMAND_WIFI_MODE_INQUIRY, TIMEOUT, debug);
}

bool WiFi_ESP8266::connectToAP(const char* ssid, const char* pass)
{

	String apInfo = AT_COMMAND_JOIN_THE_AP;
	apInfo.replace("<ssid>", String(ssid));
	apInfo.replace("<pwd>", String(pass));

	String join = sendMessage(apInfo,TIMEOUT,debug);
	return (join!="" && join!=NULL);
}

String WiFi_ESP8266::getConnectedAPinfo()
{
	return sendMessage(AT_COMMAND_JOIN_THE_AP_INQUIRY, TIMEOUT, debug);
}

bool WiFi_ESP8266::isConnectedToAP(String ssid)
{
	bool connected = false;
	String data = getConnectedAPinfo();
	int indexOfDelimeter;
	// Get the index of the delimiter, which is the ":"
	if((indexOfDelimeter = data.indexOf(":")) == -1) return connected;

	// Get the payload.
	String currentSSID = data.substring((indexOfDelimeter+2), (indexOfDelimeter+2+ssid.length()));
	
	connected = (ssid == currentSSID);
	return connected;
}

bool WiFi_ESP8266::disconnectFromAP()
{
	String discon = sendMessage(AT_COMMAND_QUIT_THE_AP, TIMEOUT, debug);
	return (discon!="" && discon != NULL);
}


String WiFi_ESP8266::setParametersofAP(const char* ssid, const char* pwd,
		WIFI_AP_CHANNELS_ENUM channel,
		WIFI_AP_ENCRYPTION_ENUM encryption)
{

	String paramsAP = AT_COMMAND_PARAMETERS_OF_AP;
	paramsAP.replace("<ssid>", String(ssid));
	paramsAP.replace("<pwd>", String(pwd));
	paramsAP.replace("<chl>", String(channel));
	paramsAP.replace("<enc>", String(encryption));

	return sendMessage(paramsAP,TIMEOUT,debug);
}


String WiFi_ESP8266::sendMessage(const String& message,
		const int timeout, bool debug)
{
        String response = "";
        wifi.write(message.c_str());
        long int time = millis();
        while((millis() - time) < timeout)
        {
                while(wifi.available())
                {
                        char c = wifi.read();
                        response += c;
                }
        }
        // If we are on debug mode, then print the response.
	if(debug)
	{
                Serial.println("DEBUG: " + String(response));
	}

        return response;
}

bool WiFi_ESP8266::enableConnections(WIFI_ENABLE_CONNECTIONS_MODE mode)
{
	String connections_mode = AT_COMMAND_ENABLE_CONNECTIONS;
	connections_mode.replace("<mode>", String(mode));
	String resp = sendMessage(connections_mode,TIMEOUT,debug);
	return (resp!="" && resp != NULL && resp!=RESPONSE_ERROR);

}

bool WiFi_ESP8266::openServer( int port)
{
	/*
	 *  Server can only be created when AT+CIPMUX=1
	 *
	 * */
	delay(5000); // It is needed because in either case the port does not open.
	bool connectionsEnabled = enableConnections(WIFI_ENABLE_CONNECTIONS_MODE::MULTIPLE_CONNECTIONS);
        delay(2000);
        return connectionsEnabled ? setAsServer(WIFI_SET_AS_SERVER::OPEN_SERVER, port) : false;
}

bool WiFi_ESP8266::closeServer()
{
	return setAsServer(WIFI_SET_AS_SERVER::CLOSE_SERVER, -1);
}

bool WiFi_ESP8266::reboot()
{// Get the index of the delimiter, which is the ":"
	String reset = AT_COMMAND_RESET;
	String resp = sendMessage(reset,TIMEOUT,debug);
	bool status = (resp!="" && resp != NULL && resp!=RESPONSE_ERROR);

	return status;
}

client_data WiFi_ESP8266::receive()
{// Get the index of the delimiter, which is the ":"
        client_data client_data;
        String data("");
	while(wifi.available())
	{
		char c = wifi.read();
                data += c;
                client_data.received = true;
	}

        if(client_data.received){
                // Get the index of the delimiter, which is the ":"
		int indexOfDelimeter = data.indexOf(":");
		// Get the payload.
                client_data.payload = data.substring((indexOfDelimeter+1));

                // Get the connection id and the length of data
                String substr = data.substring(0, indexOfDelimeter);
                String substr_array[3];
                char toCharArray[50];
                substr.toCharArray(toCharArray,50);
                char* token = strtok(toCharArray, ",");
                int i=0;
                while(token != NULL)
                {
                    substr_array[i++] = token;
                    token = strtok(NULL,",");
                }
                if(i==3)
                {
                    client_data.id = substr_array[1];
                    client_data.length = substr_array[2].toInt();
                }
	}

        return client_data;
}

bool WiFi_ESP8266::send(const String& id, const String& data)
{
    String send_command = AT_COMMAND_SEND_MUX_CONNECTIONS;
    send_command.replace("<id>", id);
    send_command.replace("<data_length>", String(sizeof (data)));
    String resp = sendMessage(send_command, TIMEOUT, debug);
    if(resp.endsWith(">"))
    {
        resp = sendMessage(data, TIMEOUT, debug);
    }

    return (resp!="" && resp != NULL && resp!=RESPONSE_ERROR);
}

bool WiFi_ESP8266::send(const String& data)
{
    String send_command = AT_COMMAND_SEND_SINGLE_CONNECTION;
    send_command.replace("<data_length>", String(sizeof (data)));
    String resp = sendMessage(send_command, TIMEOUT, debug);
    if(resp.endsWith(">"))
    {
        resp = sendMessage(data, TIMEOUT, debug);
    }
    return (resp!="" && resp != NULL && resp!=RESPONSE_ERROR);
}

bool WiFi_ESP8266::setAsServer(WIFI_SET_AS_SERVER mode, int port)
{
    bool status = false;
    String server_mode = AT_COMMAND_SET_AS_SERVER;
    server_mode.replace("<mode>", String(mode));
    if(port != -1)
            server_mode.replace("<port>", String(port));
    else // if the port equals to -1 then the default port (333) is opened.
            server_mode.replace(",<port>", String(""));

    String resp = sendMessage(server_mode,TIMEOUT,debug);
    status = (resp!="" && resp != NULL && resp!=RESPONSE_ERROR);

    return status;
}

bool WiFi_ESP8266::setupClient(WIFI_SETUP_CLIENT_TYPE type, const String& address, int port )
{
    String setup_client_command = AT_COMMAND_SETUP_CLIENT;
    setup_client_command.replace("<type>", (type == WIFI_SETUP_CLIENT_TYPE::TCP) ? "TCP" : "UDP");
    setup_client_command.replace("<addr>", address);
    setup_client_command.replace("<port>", String(port));

    String resp = sendMessage(setup_client_command, TIMEOUT, debug);

    return (resp!="" && resp != NULL && resp!=RESPONSE_ERROR);
}

bool WiFi_ESP8266::closeConnection(const String& id)
{
    String close_connection_command = AT_COMMAND_CLOSE_CONNECTION_BY_ID;
    close_connection_command.replace("<id>", id);

    String resp = sendMessage(close_connection_command, TIMEOUT, debug);

    return (resp!="" && resp != NULL && resp!=RESPONSE_ERROR);
}
