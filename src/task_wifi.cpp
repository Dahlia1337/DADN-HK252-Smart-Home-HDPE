#include "task_wifi.h"

void startAP()
{
    WiFi.mode(WIFI_AP);
    WiFi.softAP(String(SSID_AP), String(PASS_AP));
    Serial.print("AP IP: ");
    Serial.println(WiFi.softAPIP());
}

void startSTA()
{
    
    if (WIFI_SSID.isEmpty())
    {
        vTaskDelete(NULL);
    }

    WiFi.mode(WIFI_STA);

    if (WIFI_PASS.isEmpty())
    {
        WiFi.begin(WIFI_SSID.c_str());
    }
    else
    {
        WiFi.begin(WIFI_SSID.c_str(), WIFI_PASS.c_str());
    }

    int timeout = 0;
    while (WiFi.status() != WL_CONNECTED && timeout < 200)
    {
        vTaskDelay(100 / portTICK_PERIOD_MS);
        timeout++;
    }
    // Give a semaphore here
    if (WiFi.status() == WL_CONNECTED)
    {
        Serial.print("✅ STA Đã kết nối. Local IP: ");
        Serial.println(WiFi.localIP());
        // Give a semaphore here
        if (MDNS.begin("grouphdpe"))
        {
            Serial.println("✅ mDNS updated for STA: http://grouphdpe.local");
            MDNS.addService("http", "tcp", 80);
        }
    }
    else
    {
        Serial.println("❌ STA Kết nối Thất bại/Timeout.");
    }
}

bool Wifi_reconnect()
{

    const wl_status_t status = WiFi.status();
    if (status == WL_CONNECTED)
    {
        return true;
    }
    startSTA();
    return false;
}
