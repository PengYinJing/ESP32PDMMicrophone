
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <Preferences.h>
#include <Wire.h>
#include <ArduinoJson.h>
#include <TimeLib.h>
#include "Globals.h"
#include "PubSubClient.h"

#define BUFFER_SIZE     (1024 * 2) /* Channel = 1, Format = 16bit */

char ssid[] = "CMI-TECH3"; //  your network SSID (name)
char password[] = "19790222";    // your network password (use for WPA, or use as key for WEP)
bool g_WifiConnected = false;

#if BLUEMIX_EN
char *server = NULL;                                //IBM Watson server
char authMethod[] = "use-token-auth";               //Authentication method used to connect to IBM
char *token = NULL;                                 //Token used to connect to IBM
char *clientId = NULL;                              //ClientID used to connect to IBM

#if SSL_EN
const char* root_ca_cert = 
"-----BEGIN CERTIFICATE-----\n"
"MIIElDCCA3ygAwIBAgIQAf2j627KdciIQ4tyS8+8kTANBgkqhkiG9w0BAQsFADBh\n"
"MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\n"
"d3cuZGlnaWNlcnQuY29tMSAwHgYDVQQDExdEaWdpQ2VydCBHbG9iYWwgUm9vdCBD\n"
"QTAeFw0xMzAzMDgxMjAwMDBaFw0yMzAzMDgxMjAwMDBaME0xCzAJBgNVBAYTAlVT\n"
"MRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxJzAlBgNVBAMTHkRpZ2lDZXJ0IFNIQTIg\n"
"U2VjdXJlIFNlcnZlciBDQTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEB\n"
"ANyuWJBNwcQwFZA1W248ghX1LFy949v/cUP6ZCWA1O4Yok3wZtAKc24RmDYXZK83\n"
"nf36QYSvx6+M/hpzTc8zl5CilodTgyu5pnVILR1WN3vaMTIa16yrBvSqXUu3R0bd\n"
"KpPDkC55gIDvEwRqFDu1m5K+wgdlTvza/P96rtxcflUxDOg5B6TXvi/TC2rSsd9f\n"
"/ld0Uzs1gN2ujkSYs58O09rg1/RrKatEp0tYhG2SS4HD2nOLEpdIkARFdRrdNzGX\n"
"kujNVA075ME/OV4uuPNcfhCOhkEAjUVmR7ChZc6gqikJTvOX6+guqw9ypzAO+sf0\n"
"/RR3w6RbKFfCs/mC/bdFWJsCAwEAAaOCAVowggFWMBIGA1UdEwEB/wQIMAYBAf8C\n"
"AQAwDgYDVR0PAQH/BAQDAgGGMDQGCCsGAQUFBwEBBCgwJjAkBggrBgEFBQcwAYYY\n"
"aHR0cDovL29jc3AuZGlnaWNlcnQuY29tMHsGA1UdHwR0MHIwN6A1oDOGMWh0dHA6\n"
"Ly9jcmwzLmRpZ2ljZXJ0LmNvbS9EaWdpQ2VydEdsb2JhbFJvb3RDQS5jcmwwN6A1\n"
"oDOGMWh0dHA6Ly9jcmw0LmRpZ2ljZXJ0LmNvbS9EaWdpQ2VydEdsb2JhbFJvb3RD\n"
"QS5jcmwwPQYDVR0gBDYwNDAyBgRVHSAAMCowKAYIKwYBBQUHAgEWHGh0dHBzOi8v\n"
"d3d3LmRpZ2ljZXJ0LmNvbS9DUFMwHQYDVR0OBBYEFA+AYRyCMWHVLyjnjUY4tCzh\n"
"xtniMB8GA1UdIwQYMBaAFAPeUDVW0Uy7ZvCj4hsbw5eyPdFVMA0GCSqGSIb3DQEB\n"
"CwUAA4IBAQAjPt9L0jFCpbZ+QlwaRMxp0Wi0XUvgBCFsS+JtzLHgl4+mUwnNqipl\n"
"5TlPHoOlblyYoiQm5vuh7ZPHLgLGTUq/sELfeNqzqPlt/yGFUzZgTHbO7Djc1lGA\n"
"8MXW5dRNJ2Srm8c+cftIl7gzbckTB+6WohsYFfZcTEDts8Ls/3HB40f/1LkAtDdC\n"
"2iDJ6m6K7hQGrn2iWZiIqBtvLfTyyRRfJs8sjX7tN8Cp1Tm5gr8ZDOo0rwAhaPit\n"
"c+LJMto4JQtV05od8GiG7S5BNO98pVAdvzr508EIDObtHopYJeS4d60tbvVS3bR0\n"
"j6tJLp07kzQoH3jOlOrHvdPJbRzeXDLz\n"
"-----END CERTIFICATE-----\n";
#endif

Preferences g_Preferences;
PubSubClient* g_PubSubClient = NULL;

#if SSL_EN
WiFiClientSecure g_WifiClient;
#else
WiFiClient g_WifiClient;
#endif /* SSL_EN */
#elif 0
const char* udpAddress = "192.168.1.129";
int udpPort = 8000;
WiFiUDP g_WifiUDP;
#else
const char* host = "192.168.1.129";
int port = 8000;
WiFiClient g_WifiClient;
#endif /* BLUEMIX_EN */

char g_SampleData[BUFFER_SIZE];

#if BLUEMIX_EN
void subscriptionReceiveCallback(char* topic, byte* payload, unsigned int payloadLength)
{
  
}

boolean checkSubscriptionTopics() {
  if (!g_PubSubClient->loop())  // Ping IoTWatson
    return mqttConnect();      // If unable to ping, check for valid connection
  return true;
}

boolean mqttConnect() {
   int timeout = 60;
   if (!!!g_PubSubClient->connected()) {  
      Serial.print(F("Reconnecting MQTT client to ")); Serial.println(server);
      while (!!!g_PubSubClient->connect(clientId, authMethod, token) && timeout != 0) {
         Serial.print(".");
         delay(1000);
         timeout --;
      }
      Serial.println();
   }
   if (timeout)
      return true;
   else
      return false;
}

boolean setUpBluemix()
{
    //Collect credentials from Preference
    String org, device_type, device_id, tken;
#if 0
    g_Preferences.begin(PREF_SECTION_WIFI, false);
    org = g_Preferences.getString(PREF_KEY_ORG);
    device_type = g_Preferences.getString(PREF_KEY_DEV_TYPE);
    device_id = g_Preferences.getString(PREF_KEY_DEV_ID);
    tken = g_Preferences.getString(PREF_KEY_TOKEN);
    g_Preferences.end();
#else
    org = "jwphba";
    device_type = "ESP32";
    device_id = "esp32_device_001";
    tken = "N8QtvY-YrnXF4o)*Bf";
#endif
    
    //Configure credentials for connection to bluemix
    if (server)
      free(server);
    if (token)
      free(token);
    if (clientId)
      free(clientId);
    if (g_PubSubClient)
      delete g_PubSubClient;

    server = (char *)calloc(1, strlen(org.c_str()) + strlen(".messaging.internetofthings.ibmcloud.com") + 1);
    strcpy(server, org.c_str());
    strcat(server, ".messaging.internetofthings.ibmcloud.com");
    token = (char *)calloc(1, strlen(tken.c_str()) + 1);
    strcpy(token, tken.c_str());
    clientId = (char *)calloc(1, strlen("d:::") + strlen(org.c_str()) + strlen(device_type.c_str()) + strlen(device_id.c_str()) + 1);
    strcpy(clientId, "d:");
    strcat(clientId, org.c_str());
    strcat(clientId, ":");
    strcat(clientId, device_type.c_str());
    strcat(clientId, ":");
    strcat(clientId, device_id.c_str());
#if SSL_EN
    g_WifiClient.setCACert(root_ca_cert);
#endif
    g_PubSubClient = new PubSubClient(server, 1883, subscriptionReceiveCallback, g_WifiClient);

    return mqttConnect();
}

#endif /* BLUEMIX_EN */

//wifi event handler
void WiFiEvent(WiFiEvent_t event){
    switch(event) {
      case SYSTEM_EVENT_STA_GOT_IP:
          //When connected set 
          Serial.print("WiFi connected! IP address: ");
          Serial.println(WiFi.localIP());
          g_WifiConnected = true;
          //initializes the UDP state
          //This initializes the transfer buffer
#if !BLUEMIX_EN && 0
          g_WifiUDP.begin(WiFi.localIP(), udpPort);
#endif
          break;
      case SYSTEM_EVENT_STA_DISCONNECTED:
          Serial.println("WiFi lost connection");
          g_WifiConnected = false;
          break;
    }
}

void setup()
{
  Serial.begin(115200);
  
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.onEvent(WiFiEvent);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
#if BLUEMIX_EN
  setUpBluemix();
#else
  Serial.printf("Connecting to server (%s:%d)\n", host, port);
  while (!g_WifiClient.connected())
  {
    Serial.print(".");
    g_WifiClient.connect(host, port);
    delay(500);
  }

  Serial.println("Connected to server\n");
  g_WifiClient.print("!**********************!");
#endif
  
  init_i2s_pdm();
}

void loop()
{
  //if (Serial.available() > 0)
  {
    //Serial.println("Capturing audio");
    capture_i2s_pdm();
  }
}

