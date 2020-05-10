#include <ESP8266WiFi.h>

const char *ssid = "ION-303";
const char *password = "55638792";

WiFiServer server(80);

String header;

const int ledRed = 2;
const int ledYel = 16;
const int ledBlu = 0;
const int ledGre = 15;
const int piezo = 12;

int ledOrder[100];
int ledReset[100];
int ledPins[4] = {ledRed, ledYel, ledBlu, ledGre};

int highScore = 0;
int score = 0;
int currentCount = 0;
bool gameOn = false;
bool displaying = false;

unsigned long currentTime = millis();
unsigned long previousTime = 0;
unsigned long previousInput = 0;
const long timeoutTime = 2000;

void setup()
{
    Serial.begin(115200);

    pinMode(ledRed, OUTPUT);
    pinMode(ledYel, OUTPUT);
    pinMode(ledBlu, OUTPUT);
    pinMode(ledGre, OUTPUT);
    pinMode(piezo, OUTPUT);

    digitalWrite(ledRed, LOW);
    digitalWrite(ledYel, LOW);
    digitalWrite(ledBlu, LOW);
    digitalWrite(ledGre, LOW);
    digitalWrite(piezo, LOW);

    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    // Print local IP address and start web server
    Serial.println("");
    Serial.println("WiFi connected.");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    server.begin();
}

void loop()
{
    WiFiClient client = server.available();

    if (client)
    {
        Serial.println("New Client.");
        String currentLine = "";
        currentTime = millis();
        previousTime = currentTime;
        while (client.connected() && currentTime - previousTime <= timeoutTime)
        {
            currentTime = millis();

            if (currentTime - previousInput > 3000 && displaying == false && gameOn == true)
            {
                defeat(client);
                Serial.println("aaa");
            }

            if (client.available())
            {
                char c = client.read();
                Serial.write(c);
                header += c;
                if (c == '\n')
                {
                    if (currentLine.length() == 0)
                    {
                        client.println("HTTP/1.1 200 OK");
                        client.println("Content-type:text/html");
                        client.println("Connection: close");
                        client.println();

                        if (header.indexOf("GET /start") >= 0)
                        {
                            gameAdvance(4, client);
                        }
                        else if (header.indexOf("GET /red") >= 0)
                        {
                            gameAdvance(0, client);
                        }
                        else if (header.indexOf("GET /yel") >= 0)
                        {
                            gameAdvance(1, client);
                        }
                        else if (header.indexOf("GET /blu") >= 0)
                        {
                            gameAdvance(2, client);
                        }

                        else if (header.indexOf("GET /gre") >= 0)
                        {
                            gameAdvance(3, client);
                        }

                        client.println("<!DOCTYPE html><html>");
                        client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
                        client.println("<link rel=\"icon\" href=\"data:,\">");

                        client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
                        client.println(".buttonRed { background-color: #FF0000; border: none; color: white; padding: 16px 40px; text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
                        client.println(".buttonYel { background-color: #FFFF00; border: none; color: white; padding: 16px 40px; text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
                        client.println(".buttonBlu { background-color: #2200FF; border: none; color: white; padding: 16px 40px; text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
                        client.println(".buttonGre { background-color: #22FF00; border: none; color: white; padding: 16px 40px; text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
                        client.println(".buttonStart { background-color: #195B6A; border: none; color: white; padding: 16px 40px; text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
                        client.println(".buttonOff { background-color: #77878A; border: none; color: white; padding: 16px 40px; text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}</style></head>");

                        client.println("<body><h1>Simon game</h1>");

                        if (gameOn == false)
                        {
                            client.println("<p><a><button class=\"buttonOff\">Red</button></a></p>");
                            client.println("<p><a><button class=\"buttonOff\">Yellow</button></a></p>");
                            client.println("<p><a><button class=\"buttonOff\">Blue</button></a></p>");
                            client.println("<p><a><button class=\"buttonOff\">Green</button></a></p>");
                            client.println("<p><a href=\"/start\"><button class=\"buttonStart\">Start</button></a></p>");
                        }
                        else
                        {
                            client.println("<p><a href=\"/red\"><button class=\"buttonRed\">Red</button></a></p>");
                            client.println("<p><a href=\"/yel\"><button class=\"buttonYel\">Yellow</button></a></p>");
                            client.println("<p><a href=\"/blu\"><button class=\"buttonBlu\">Blue</button></a></p>");
                            client.println("<p><a href=\"/gre\"><button class=\"buttonGre\">Green</button></a></p>");
                            client.println("<p><a href=\"start\"><button class=\"buttonStart\">Restart</button></a></p>");
                        }

                        client.println("<p>Current score: " + String(score) + " / High score: " + String(highScore) + "</p>");

                        client.println("</body></html>");

                        client.println();

                        break;
                    }
                    else
                    {
                        currentLine = "";
                    }
                }
                else if (c != '\r')
                {
                    currentLine += c;
                }
            }
        }

        header = "";

        client.stop();
        Serial.println("Client disconnected.");
        Serial.println("");
    }
}

void gameAdvance(int button, WiFiClient client)
{

    displaying = true;

    if (button == 4)
    {
        gameOn = true;
        currentCount = 0;
        score = 0;

        tone(piezo, 200);
        delay(100);
        noTone(piezo);
        tone(piezo, 200);
        delay(100);
        noTone(piezo);

        digitalWrite(ledRed, HIGH);
        digitalWrite(ledYel, HIGH);
        digitalWrite(ledBlu, HIGH);
        digitalWrite(ledGre, HIGH);
        delay(500);
        digitalWrite(ledRed, LOW);
        digitalWrite(ledYel, LOW);
        digitalWrite(ledBlu, LOW);
        digitalWrite(ledGre, LOW);
        delay(500);

        ledOrder[0] = random(0, 4);
        digitalWrite(ledPins[ledOrder[0]], HIGH);
        delay(500);
        digitalWrite(ledPins[ledOrder[0]], LOW);
        delay(500);
    }
    else
    {
        digitalWrite(ledPins[button], HIGH);
        delay(500);
        digitalWrite(ledPins[button], LOW);
        delay(500);
        if (ledOrder[currentCount] == button)
        {
            currentCount++;

            if (currentCount > score)
            {

                tone(piezo, 900);
                delay(100);
                noTone(piezo);
                delay(100);
                tone(piezo, 500);
                delay(100);
                noTone(piezo);
                delay(100);
                tone(piezo, 500);
                delay(100);
                noTone(piezo);

                ledOrder[currentCount] = random(0, 4);

                for (int i = 0; i <= currentCount; i++)
                {

                    digitalWrite(ledPins[ledOrder[i]], HIGH);
                    delay(500);
                    digitalWrite(ledPins[ledOrder[i]], LOW);
                    delay(500);
                }

                currentCount = 0;
                score++;

                if (score > highScore)
                {
                    highScore++;
                }
            }
        }
        else
        {
            defeat(client);
        }

        previousInput = millis();
        displaying = false;
    }
}

void defeat(WiFiClient client)
{
    gameOn = false;
    score = 0;
    for (int i = 0; i <= 1; i++)
    {
        tone(piezo, 900);
        delay(100);
        noTone(piezo);
        tone(piezo, 900);
        delay(100);
        noTone(piezo);
        digitalWrite(ledRed, HIGH);
        delay(100);
        digitalWrite(ledRed, LOW);
        digitalWrite(ledYel, HIGH);
        delay(100);
        digitalWrite(ledYel, LOW);
        digitalWrite(ledBlu, HIGH);
        delay(100);
        digitalWrite(ledBlu, LOW);
        digitalWrite(ledGre, HIGH);
        delay(100);
        digitalWrite(ledGre, LOW);
    }

    client.println();
}