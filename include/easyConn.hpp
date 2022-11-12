#ifndef EASY_CONN_H
#define EASY_CONN_H
#include <BluetoothSerial.h>
#include <ESPmDNS.h>
#include <Preferences.h>
#include <WiFi.h>

BluetoothSerial SerialBT;
Preferences preferences;

int count = 0;

void easySetup()
{
    if (!MDNS.begin("darkflow-cam"))
    {
        Serial.println("Error starting mDNS");
    }

    preferences.begin("my-app", false);
    String ssid = preferences.getString("ssid", "null");
    String password = preferences.getString("pass", "null");
    Serial.println(ssid + ":" + password);
    if (ssid != "null" && password != "null")
    {
        WiFi.mode(WIFI_STA);
        WiFi.begin(ssid.c_str(), password.c_str());
        delay(5000);
        while (WiFi.status() != WL_CONNECTED)
        {
            Serial.println("First try failed.!");
            for (int i = 0; i < 5; i++)
            {
                WiFi.begin(ssid.c_str(), password.c_str());
                Serial.println("Trying to reconnect...");
                delay(2000);
                if (WiFi.status() == WL_CONNECTED)
                {
                    break;
                }
            }
            WiFi.setSleep(true);
            SerialBT.begin("Darkflow-Cam");
            Serial.println("Bluedaemon started...");
            while (count != 2)
            {
                if (SerialBT.available())
                {
                    if (count == 0)
                    {
                        String data0 = SerialBT.readString();
                        data0.trim();
                        if (data0 != "")
                        {
                            preferences.putString("ssid", data0.c_str());
                            count++;
                            Serial.println(data0);
                            SerialBT.println("correct ssid stablished");
                        }
                    }
                    else if (count == 1)
                    {
                        String data1 = SerialBT.readString();
                        data1.trim();
                        if (data1 != "")
                        {
                            preferences.putString("pass", data1.c_str());
                            Serial.println(data1);
                            SerialBT.println("correct pass stablished");
                            SerialBT.println("...rebooting...");
                            count++;
                        }
                    }
                }
            }
            ESP.restart();
        }

        delay(1000);

        if (WiFi.status() == WL_CONNECTED)
        {
            Serial.println("");
            Serial.println("WiFi connected");

            Serial.print("Camera Ready! Use 'http://");
            Serial.print(WiFi.localIP());
            Serial.println("' to connect");
        }
    }
}

#endif