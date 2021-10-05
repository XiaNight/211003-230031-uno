
#include <Arduino.h>
#include <SoftwareSerial.h>
SoftwareSerial sim808(2, 3); // RX, TX

#define bir 1
#define onn 10
#define yuz 100
#define ikiyuz 200

#define COMMAND_TIME_OUT 30000

unsigned long currentTime = 0;
unsigned long timeBuffer = 0;

String sim808Echo;
String serialInput;

char ctrl_z = 26;

void SendCommand(String cmd, bool changeLine = true);

void setup()
{
    Serial.begin(9600);
    delay(bir);
    sim808.begin(9600);
    delay(bir);
}

void loop()
{
    readsimPort();
    readSerialPort();

    if (sim808Echo != "")
    {
        Serial.println("SIM808 output:  ");
        Serial.println(sim808Echo);
        sim808Echo = "";
    }
    if (serialInput != "")
    {
        Serial.print("User input: ");
        Serial.println(serialInput);
        if (serialInput.indexOf("SendSMS") > -1)
        {
            SendCommand("AT+CMGF=1");
            sim808.println("AT+CMGS=\"+8860987750217\"\r");
            delay(50);
            sim808.print("HELLO_WORLD\x1A");
        }
        else if(serialInput.indexOf("/Reboot") > -1)
        {
            SendCommand("AT+CFUN=0");
            SendCommand("AT+CFUN=1");
        }
        else if (serialInput.indexOf("/SMSTest") > -1)
        {
            SendCommand("AT");
            SendCommand("AT+CFUN?");
            SendCommand("AT+CREG?");
            SendCommand("AT+COPS?");
        }
        else if (serialInput.indexOf("/Info") > -1)
        {
            SendCommand("AT");
            SendCommand("ATI");
            SendCommand("AT+CGMI");
            SendCommand("AT+CGMM");
            
            SendCommand("AT+GMM");
            SendCommand("AT+CPIN?");
            SendCommand("AT+CCID");
            SendCommand("AT+CFUN?");
            SendCommand("AT+CSQ");
            SendCommand("AT+CREG?");
            SendCommand("AT+CGREG?");
            SendCommand("AT+CEREG?");
            SendCommand("AT+CGDCONT?");
            SendCommand("AT+CGACT?");
            SendCommand("AT+COPS?");
        }
        else if(serialInput.indexOf("/Test") > -1)
        {
            SendCommand("AT");
        }
        else
        {
            sim808.print(serialInput);
        }

        serialInput = "";
    }
}

void SendCommand(String cmd, bool changeLine = true)
{
    unsigned long sendTime = millis();
    if(changeLine)
    {
        sim808.println(cmd);
    }
    else
    {
        sim808.print(cmd);
    }

    sim808Echo = "";

    while ((millis() - sendTime) < COMMAND_TIME_OUT)
    {
        if(sim808.available() > 0)
        {
            timeBuffer = millis();
            while(millis() - timeBuffer < 50)
            {
                while (sim808.available())
                {
                    char c = sim808.read(); //gets one byte from serial buffer
                    sim808Echo += c;          //makes the string readString
                }
            }
            break;
        }
    }
    sim808.flush();

    if(sim808Echo == "")
    {
        Serial.println("Operation Timed Out!");
    }
    else
    {
        Serial.println(sim808Echo);
        sim808Echo = "";
    }
}

void setsim808()
{
    Serial.println("at komutlar manuel gonderildi");
    delay(onn);

    sim808.print("AT+CGPSPWR=1\r\n");
    delay(ikiyuz); // gps on
    sim808.print("AT+CGNSSEQ=RMC\r\n");
    delay(ikiyuz); // gps on
    //sim808.print("AT+CGPSRST=0\r\n");             delay(yuz); // cold restart

    sim808.print("AT+CMGF=1\r\n");
    delay(ikiyuz); // set SMS mode to text
    sim808.print("AT+CNMi=2,2,0,0,0\r\n");
    delay(ikiyuz); //Gelen mesajı okuma
    sim808.print("AT+CLIP=1\r\n");
    delay(ikiyuz);
    sim808.print("AT+DDET=1\r\n");
    delay(ikiyuz); //dtmf
    sim808.print("AT+CNUM\r\n");
    delay(ikiyuz);
    sim808.print("AT+CLCC=1\r\n");
    delay(ikiyuz); //caller info
    sim808.print("ATS0=1\r\n");
    delay(ikiyuz); //ring counter // 2 aramadan sonra cevap verilecek
    //sim808.print("AT+CMGDA=DEL ALL\r\n");         delay(yuz); // Delete ALL SMS
    sim808.print("AT&W\r\n");
    delay(ikiyuz); // STORE active profile
}

void readsimPort()
{
    timeBuffer = currentTime;
    delay(bir);
    while ((currentTime - timeBuffer) < 200)
    {
        currentTime = millis();
        while (sim808.available())
        {
            currentTime = millis();
            timeBuffer = currentTime;
            char c = sim808.read(); //gets one byte from serial buffer
            sim808Echo += c;          //makes the string readString
        }
    }
    sim808.flush();
}

void readSerialPort()
{
    while (Serial.available())
    {
        delay(onn);
        if (Serial.available() > 0)
        {
            char d = Serial.read(); //gets one byte from serial buffer
            serialInput += d;          //makes the string readString
        }
    }
    Serial.flush();
}