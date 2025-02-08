
#include "UiHelper.h"

// #include "httpUpdater.h"

// #include <WiFiClient.h>
// #include "context.h"

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

UiHelper::UiHelper(uint8_t pinLed, ConfigHelper *smManager) : BaseHelper(pinLed)
{
#ifdef ESP8266
    _server = new ESP8266WebServer(80);
#endif
#ifdef ESP32
    _server = new WebServer(80);
#endif
    m_configuration = smManager;
}
wl_status_t UiHelper::begin()
{
    _server->on("/", std::bind(&UiHelper::help, this));
    _server->onNotFound(std::bind(&UiHelper::help, this));

    _server->on("/restart", std::bind(&UiHelper::restartESP, this));
    _server->on("/setSetup", std::bind(&UiHelper::setSetup, this));
    _server->on("/setup", std::bind(&UiHelper::displaySetup, this));
    _server->on("/style.css", std::bind(&UiHelper::cssStyle, this));
    _server->on("/reset", std::bind(&UiHelper::wifiReset, this));
    _server->on("/jsonSetup", std::bind(&UiHelper::jsonSetup, this));
    _server->on("/fileSetup", std::bind(&UiHelper::fileSetup, this));
    _server->on("/dir", std::bind(&UiHelper::dir, this));
    _server->on("/open", std::bind(&UiHelper::open, this));
    _server->on("/help", std::bind(&UiHelper::help, this));
    _server->on("/jquery.min.js", std::bind(&UiHelper::jqueryminjs, this));
#ifdef ESP8266
    _httpUpdater.setup(_server, ((const char *)"/firmware"), MODULE_UPDATE_LOGIN, MODULE_UPDATE_PASS);
#else
    initHttpUpdater();
#endif
#ifdef OTA_FOR_ATOM
    setOTAForAtom();
#endif

    _server->begin();
    return WiFi.status();
}

void UiHelper::restartESP()
{
    ESP.restart();
}

void UiHelper::cssStyle()
{
    // digitalWrite ( pinLed, LOW );
    loadFromSpiffs("/style.css");
}

void UiHelper::jqueryminjs()
{
    // digitalWrite ( pinLed, LOW );
    loadFromSpiffs("/jquery.min.js");
}

void UiHelper::displaySetup()
{
    // digitalWrite ( pinLed, LOW );
    loadFromSpiffs("/setup.html");
}

void UiHelper::dir()
{
    callback(FUNCTION_IN);
    File root = SPIFFS.open("/");
    File file = root.openNextFile();
    String ss = "\"file\":[";
    while (file)
    {
        ss += "\"" + String(file.name()) + "\"";
        file = root.openNextFile();
        if (file)
        {
            ss += ",";
        }
    }
    ss += "]";

    _server->send(200, "text/html", ss);
    callback(FUNCTION_OUT);
}

void UiHelper::open()
{
    String fileName;
    if ((fileName = _server->arg("file")) != NULL)
    {
        loadFromSpiffs("/" + fileName);
    }
}

void UiHelper::help()
{
    // load from spiff and replace string by real ip address
    File file = SPIFFS.open("/help.html", "r");
    if (!file)
    {
        Serial.println("Failed to open file");
        return;
    }

    String content = file.readString();
    file.close();

    // Replace the placeholder with the actual IP address of the router
    String ipStr;

    if (WiFi.status() == WL_CONNECTED)
    {
        ipStr = WiFi.localIP().toString();
    }
    else
    {
        ipStr = WiFi.softAPIP().toString();
    }
    content.replace("%ip", ipStr);
    _server->send(200, "text/html", content);
    // loadFromSpiffs("/help.html");
}

#ifdef ESP32
void IRAM_ATTR UiHelper::setSetup()
{
#endif
#ifdef ESP8266
    void UiHelper::setSetup()
    {
#endif
#ifdef MCPOC_TEST
        for (uint8_t i = 0; i < _server->args(); i++)
        {
            DEBUGLOGF("[%s,%s]\n", _server->argName(i).c_str(), _server->arg(i).c_str());
        }
        DEBUGLOG("");
#endif

        String str;
        // DEBUGLOGF("[1,%s]\n",str.c_str());
        if ((str = _server->arg("ssid")) != NULL)
        {
            m_configuration->m_ssid = str;
        }

        if ((str = _server->arg("routerIP")) != NULL)
        {
            m_configuration->m_routerIP.fromString(str);
        }
        if ((str = _server->arg("IP")) != NULL)
        {
            m_configuration->m_IP.fromString(str);
        }

        m_configuration->m_dynamicIP = _server->arg("dynamicIP") != NULL;

        if ((str = _server->arg("pass")) != NULL)
        {
            if (str != HIDDEN_KEY)
            {
                m_configuration->m_password = str;
            }
        }

        if ((str = _server->arg("privateKey")) != NULL)
        {
            if (str != HIDDEN_KEY)
            {
                m_configuration->m_privateKey = str;
            }
        }

        if ((str = _server->arg("publicKey")) != NULL)
        {
            if (str != HIDDEN_KEY)
            {
                m_configuration->m_publicKey = str;
            }
        }

        _server->send(200, "text/html", "data recorded.restart board");
        m_configuration->writeData();
        delay(200);
        restartESP();
    }

    void UiHelper::wifiReset()
    {
        _server->send(200, "text/html", "reseting...");
        WiFi.disconnect(true, true);
        restartESP();
    }

    String UiHelper::getJSon()
    {
        return "{" + m_configuration->toJson() + "}";
    }

    void UiHelper::jsonSetup()
    {
        _server->send(200, "text/html", getJSon());
    }

    void UiHelper::fileSetup()
    {
        loadFromSpiffs("/config.json");
    }

    void UiHelper::handleServer()
    {
        _server->handleClient();

#ifdef OTA_FOR_ATOM
        ArduinoOTA.handle();
#endif
    }

    bool UiHelper::loadFromSpiffs(String path)
    {
        callback(FUNCTION_IN);
        DEBUGLOGF("[%s]\n", path.c_str());

        /*Serial.println("spiffs directory");
        String str = "";
        Dir dir = SPIFFS.openDir("/");
        while (dir.next()) {
            str += dir.fileName();
            str += " / ";
            str += dir.fileSize();
            str += "\r\n";
        }
        Serial.println(str);*/

        if (SPIFFS.exists(path.c_str()))
        {
            File dataFile = SPIFFS.open(path.c_str(), "r");
            if (_server->streamFile(dataFile, "text/html") != dataFile.size())
            {
                DEBUGLOGF("pb[%s][%d]\n", dataFile.name(), dataFile.size());
            }
            DEBUGLOGF("[%s][%d]\n", dataFile.name(), dataFile.size());
            dataFile.close();
        }
        else
        {
            DEBUGLOGF("[%s] does not exist\n", path.c_str());
        }

        callback(FUNCTION_OUT);
        return true;
    }

#ifdef ESP32

    //    const char HTML_FIRMWARE[] PROGMEM =
    //        "<style>#file-input,input{width:100%;height:44px;border-radius:4px;margin:10px auto;font-size:15px}input{background:#f1f1f1;border:0;padding:0 15px}body{background:#3498db;font-family:sans-serif;font-size:14px;color:#777}#file-input{padding:0;border:1px solid #ddd;line-height:44px;text-align:left;display:block;cursor:pointer}#bar,#prgbar{background-color:#f1f1f1;border-radius:10px}#bar{background-color:#3498db;width:0;height:10px}form{background:#fff;max-width:258px;margin:75px auto;padding:30px;border-radius:5px;text-align:center}.btn{background:#3498db;color:#fff;cursor:pointer}</style>\n<script src='https://ajax.googleapis.com/ajax/libs/jquery/3.2.1/jquery.min.js'></script>\n<form method='POST' action='#' enctype='multipart/form-data' id='upload_form'>\n<input type='file' name='update' id='file' onchange='sub(this)' style=display:none>\n<label id='file-input' for='file'> Choose file...</label>\n<input type='submit' class=btn value='Update'><br><br>\n<div id='prg'></div><br>\n<div id='prgbar'>\n<div id='bar'></div>\n</div><br>\n</form>\n<script>function sub(a){var b=a.value.split(\"\\\\\");document.getElementById(\"file-input\").innerHTML=\"   \"+b[b.length-1]}$(\"form\").submit(function(c){c.preventDefault();var a=$(\"#upload_form\")[0];var b=new FormData(a);$.ajax({url:\"/update\",type:\"POST\",data:b,contentType:false,processData:false,xhr:function(){var d=new window.XMLHttpRequest();d.upload.addEventListener(\"progress\",function(e){if(e.lengthComputable){var f=e.loaded/e.total;$(\"#prg\").html(\"progress: \"+Math.round(f*100)+\"%\");$(\"#bar\").css(\"width\",Math.round(f*100)+\"%\")}},false);return d},success:function(f,e){console.log(\"success!\")},error:function(e,d,f){}})});</script>";

    /*const char HTML_FIRMWARE[] PROGMEM =
            "<style>#file-input,input{width:100%;height:44px;border-radius:4px;margin:10px auto;font-size:15px}input{background:#f1f1f1;border:0;padding:0 15px}body{background:#3498db;font-family:sans-serif;font-size:14px;color:#777}#file-input{padding:0;border:1px solid #ddd;line-height:44px;text-align:left;display:block;cursor:pointer}#bar,#prgbar{background-color:#f1f1f1;border-radius:10px}#bar{background-color:#3498db;width:0;height:10px}form{background:#fff;max-width:258px;margin:75px auto;padding:30px;border-radius:5px;text-align:center}.btn{background:#3498db;color:#fff;cursor:pointer}</style>\n<script src='jquery.min.js'></script>\n<form method='POST' action='#' enctype='multipart/form-data' id='upload_form'>\n<input type='file' name='update' id='file' onchange='sub(this)' style=display:none>\n<label id='file-input' for='file'> Choose file...</label>\n<input type='submit' class=btn value='Update'><br><br>\n<div id='prg'></div><br>\n<div id='prgbar'>\n<div id='bar'></div>\n</div><br>\n</form>\n<script>function sub(a){var b=a.value.split(\"\\\\\");document.getElementById(\"file-input\").innerHTML=\"   \"+b[b.length-1]}$(\"form\").submit(function(c){c.preventDefault();var a=$(\"#upload_form\")[0];var b=new FormData(a);$.ajax({url:\"/update\",type:\"POST\",data:b,contentType:false,processData:false,xhr:function(){var d=new window.XMLHttpRequest();d.upload.addEventListener(\"progress\",function(e){if(e.lengthComputable){var f=e.loaded/e.total;$(\"#prg\").html(\"progress: \"+Math.round(f*100)+\"%\");$(\"#bar\").css(\"width\",Math.round(f*100)+\"%\")}},false);return d},success:function(f,e){console.log(\"success!\")},error:function(e,d,f){}})});</script>";
    */

    void UiHelper::httpUpdaterPage()
    {
        DEBUGLOG("Load Firmware page");
        // Serial.println("Load Firmware page");
        loadFromSpiffs("/firmware.html");
        //_server->send_P(200, "text/html", HTML_FIRMWARE);
        //_server->send(200, "text/html", "");
    }

    void UiHelper::initHttpUpdater()
    {

        _server->on("/firmware", std::bind(&UiHelper::httpUpdaterPage, this));

        /*handling uploading firmware file */
        _server->on(
            "/update", HTTP_POST, [&]()
            {
                //Serial.printf("Update: %s\n", Update.errorString());
                _server->sendHeader("Connection", "close");
                _server->send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
                ESP.restart(); },
            [&]()
            {
                HTTPUpload &upload = _server->upload();
                /*Serial.printf("upload.status: %d\n", upload.status);
                Serial.printf("upload.status: %s\n", Update.errorString());*/
                if (upload.status == UPLOAD_FILE_START)
                {
                    // Serial.printf("Update: %s\n", upload.filename.c_str());
                    int command = U_FLASH;
                    if (upload.filename == "spiffs.bin")
                    {
                        command = U_SPIFFS;
                    }
                    DEBUGLOGF("Update [%s][%d]\r", upload.filename.c_str(), command);
                    if (!Update.begin(UPDATE_SIZE_UNKNOWN, command, LED_BUILTIN))
                    { // start with max available size
                        Update.printError(Serial);
                    }
                }
                else if (upload.status == UPLOAD_FILE_WRITE)
                {
                    // Serial.println("flashing firmware to ESP");
                    /* flashing firmware to ESP*/
                    if (Update.write(upload.buf, upload.currentSize) != upload.currentSize)
                    {
                        Update.printError(Serial);
                    }
                }
                else if (upload.status == UPLOAD_FILE_END)
                {
                    if (Update.end(true))
                    { // true to set the size to the current progress
                        DEBUGLOGF("Update Success: %u\nRebooting...\n", upload.totalSize);
                        // Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
                    }
                    else
                    {
                        Update.printError(Serial);
                    }
                }
            });
    }
#endif

#ifdef OTA_FOR_ATOM

    void UiHelper::OTAOnStart()
    {
        String type;
        if (ArduinoOTA.getCommand() == U_FLASH)
        {
            type = "sketch";
        }
        else
        { // U_SPIFFS
            type = "filesystem";
        }
        // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
        Serial.println("Start updating " + type);
    }

    void UiHelper::setOTAForAtom()
    {
        Serial.println(F("ArduinoOTA conf"));
        ArduinoOTA.onStart(std::bind(&UiHelper::OTAOnStart, this));
        ArduinoOTA.onEnd([]()
                         { Serial.println("\nEnd"); });
        ArduinoOTA.onProgress([](unsigned int progress, unsigned int total)
                              { Serial.printf("Progress: %u%%\r", (progress / (total / 100))); });
        ArduinoOTA.onError([](ota_error_t error)
                           {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) {
        Serial.println(F("Auth Failed"));
      } else if (error == OTA_BEGIN_ERROR) {
        Serial.println(F("Begin Failed"));
      } else if (error == OTA_CONNECT_ERROR) {
        Serial.println(F("Connect Failed"));
      } else if (error == OTA_RECEIVE_ERROR) {
        Serial.println(F("Receive Failed"));
      } else if (error == OTA_END_ERROR) {
        Serial.println(F("End Failed"));
      } });
        ArduinoOTA.begin();
        Serial.println(F("ArduinoOTA Ready"));
    }
#endif
