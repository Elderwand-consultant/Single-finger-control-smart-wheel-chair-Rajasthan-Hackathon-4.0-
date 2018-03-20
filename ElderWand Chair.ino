int val;
int tempPin = 1;
int leftMotorForward = 3;    
int rightMotorForward = 5;   
int leftMotorBackward = 4;   
int rightMotorBackward = 6; 
int buzzer = 13; 
int pulsePin = 2;   
volatile int BPM;                  
volatile int Signal;               
volatile int IBI = 600;             
volatile boolean Pulse = false;     
volatile boolean QS = false;       
static boolean serialVisual = true;   
volatile int rate[10];                      
volatile unsigned long sampleCounter = 0;          
volatile unsigned long lastBeatTime = 0;          
volatile int P = 512;                      
volatile int T = 512;                     
volatile int thresh = 525;                
volatile int amp = 100;                   
volatile boolean firstBeat = true;        
volatile boolean secondBeat = false;   

#include <SPI.h>
#include <Ethernet.h>
#include <String.h>

byte mac[]={0xDE,0xAB,0xBE,0xEF,0xAC,0xED};

//ip is that of your device
// you can find it through ipconfig in cmd
byte ip[] = {169,254,182,100};

// gateway and subnet is not required 

byte subnet[] = {255,255,248,0};

String data;
bool access;

int pin = 7;
int ground = 6;

EthernetServer server(80);

byte incomingByte =0;
int a = 10;
int b = 25;

int trig = 12;
int echo = 11;


void setup()

{

 Serial.begin(9600);
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT); 
          
Ethernet.begin(mac,ip);
  data = "";
  access = false;
  pinMode(pin,OUTPUT);
  pinMode(ground,OUTPUT);
  digitalWrite(ground,LOW);
  pinMode(leftMotorForward, OUTPUT);
  pinMode(rightMotorForward, OUTPUT); 
  pinMode(leftMotorBackward, OUTPUT);  
  pinMode(rightMotorBackward, OUTPUT);
  Serial.begin(115200);
  interruptSetup();              
}
void loop()
{

  long t = 0, h = 0, hp = 0;
  
  // Transmitting pulse
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(trig, LOW);
  
  // Waiting for pulse
  t = pulseIn(echo, HIGH);
  
  // Calculating distance 
  h = t / 58;

  
  hp = 2 * h;  // distance in %, 0-100 %

  if(hp >= 10)
  {
    for(int i=0;i<1;i++)
    {
      beep();
      delay(1000);
    }
  }
  else if(hp >= 7 && hp < 10)
  {
    for(int i=0;i<3;i++)
    {
      beep();
      delay(1000);
    }
  }
  else if(hp >= 4 && hp < 7)
  {
    for(int i=0;i<5;i++)
    {
      beep();
      delay(1000);
    }
  }
  else
  {
    for(int i=0;i<7;i++)
    {
      beep();
      delay(1000);
    }
  }
  
  delay(1000);



  val = analogRead(tempPin);
float mv = ( val/1024.0)*5000; 
float cel = mv/10;
   serialOutput();  
   
  if (QS == true) // A Heartbeat Was Found
    {     
      serialOutputWhenBeatHappens();    
      QS = false;    
    }
  delay(20); //  take a break

  int xDir = analogRead(A0);
  int yDir = analogRead(A1);
  int sw = digitalRead(2);

  if(yDir<=400)
  {
   MotorForward(); 
  }
  else if(yDir>= 800)
  {
    MotorBackward();
  }
  else if(xDir<=400)
  {
    TurnRight();
  }
  else if(xDir>=800)
  {
    TurnLeft();
  }
  else
  {
    MotorStop();
  } 

  EthernetClient client = server.available();
  while(client.connected())
  {
    if(client.available())
    {
      char c = client.read();
      Serial.print(c);
      data.concat(c);
      if(c =='\n')
      {
        if(Serial.available() >0)
        incomingByte = Serial.read();
        Serial.println(data);
        if(data.indexOf("gadi=0")>0)
        {
          TurnRight();
        }
        else if(data.indexOf("gadi=1")>0)
        {
          TurnLeft();
        }
        else if(data.indexOf("gadi=2")>0)
        {
          MotorForward();
        }
        else if(data.indexOf("gadi=3")>0)
        {
          MotorBackward();
        }
       
        client.println("HTTP/1.1 200 OK");
        client.println("Content-Type: text/html");
        client.println();
        client.println(" <html><head><meta charset=\"utf-8\"/><title>ASUS CORP </title><link rel=\"stylesheet\" href=\"main1.css\" type=\"text/css\"/></head><body><div style=\"background-color:#000000;color:white;padding:5px;\"><h1><hr><strong> Elderwand's Chair system</strong></h1>");
        client.println("<hr> <div style=\"background-color:white; color:black; padding:20px;\"><h2>Health Monitor</h2><h3>Heart beat condition");
        client.println( serialOutputWhenBeatHappens(); );
        client.println("</h3>");
        client.println("<h3>BPM: ");
        client.print(BPM);
        client.print("</h3>");
        client.println("<h3>Temp: </h3>");
        client.print(cel) ;
        client.println("<h2>Environment Condition </h2>");
        client.println("<h3>Temp: ");
        client.print("</h3>");
        client.println("<h3>Humidity: ");
        client.print("</h3>");
        client.println("<h1>Chair controller </h1>");
        client.println("");
        client.println("</body></html>");
        client.println("<a href='/?gadi=0'>Right</a>");
        client.println("<a href='/?gadi=1'>Left</a>");
        client.println("<a href='/?gadi=2'>Forward</a>");
        client.println("<a href='/?gadi=3'>Backward</a>");
        client.stop();
        data = "";
      }      
    }
  }
}
void interruptSetup()
{     
  TCCR2A = 0x02;     // DISABLE PWM ON DIGITAL PINS 3 AND 11, AND GO INTO CTC MODE
  TCCR2B = 0x06;     // DON'T FORCE COMPARE, 256 PRESCALER 
  OCR2A = 0X7C;      // SET THE TOP OF THE COUNT TO 124 FOR 500Hz SAMPLE RATE
  TIMSK2 = 0x02;     // ENABLE INTERRUPT ON MATCH BETWEEN TIMER2 AND OCR2A
  sei();             // MAKE SURE GLOBAL INTERRUPTS ARE ENABLED      
} 

void beep()
{
  digitalWrite(13, HIGH);
}

void serialOutput()
{
 if (serialVisual == true)
  {  
     arduinoSerialMonitorVisual('-', Signal);   // goes to function that makes Serial Monitor Visualizer
  } 
 else
  {
      sendDataToSerial('S', Signal);     // goes to sendDataToSerial function
   }    
   delay(1000);    
}

void serialOutputWhenBeatHappens()
{    
 if (serialVisual == true) //  Code to Make the Serial Monitor Visualizer Work
   {            
     Client.print("*** Heart-Beat Happened *** ");  //ASCII Art Madness
     Serial.print("BPM: ");
     Serial.println(BPM);
   }
 else
   {
     sendDataToSerial('B',BPM);   // send heart rate with a 'B' prefix
     sendDataToSerial('Q',IBI);   // send time between beats with a 'Q' prefix
   }   
   delay(1000);
}

void arduinoSerialMonitorVisual(char symbol, int data )
{    
  const int sensorMin = 0;      
  const int sensorMax = 1024;
  int sensorReading = data;
  int range = map(sensorReading, sensorMin, sensorMax, 0, 11);
  
}


void sendDataToSerial(char symbol, int data )
{
   Serial.print(symbol);
   Serial.println(data);                
}

ISR(TIMER2_COMPA_vect) //triggered when Timer2 counts to 124
{  
  cli();                                      // disable interrupts while we do this
  Signal = analogRead(pulsePin);              // read the Pulse Sensor 
  sampleCounter += 2;                         // keep track of the time in mS with this variable
  int N = sampleCounter - lastBeatTime;       // monitor the time since the last beat to avoid noise
                                              //  find the peak and trough of the pulse wave
  if(Signal < thresh && N > (IBI/5)*3) // avoid dichrotic noise by waiting 3/5 of last IBI
    {      
      if (Signal < T) // T is the trough
      {                        
        T = Signal; // keep track of lowest point in pulse wave 
      }
    }

  if(Signal > thresh && Signal > P)
    {          // thresh condition helps avoid noise
      P = Signal;                             // P is the peak
    }                                        // keep track of highest point in pulse wave

  //  NOW IT'S TIME TO LOOK FOR THE HEART BEAT
  // signal surges up in value every time there is a pulse
  if (N > 250)
  {                                   // avoid high frequency noise
    if ( (Signal > thresh) && (Pulse == false) && (N > (IBI/5)*3) )
      {        
        Pulse = true;                               
        IBI = sampleCounter - lastBeatTime;         // measure time between beats in mS
        lastBeatTime = sampleCounter;               // keep track of time for next pulse
  
        if(secondBeat)
        {                        // if this is the second beat, if secondBeat == TRUE
          secondBeat = false;                  // clear secondBeat flag
          for(int i=0; i<=9; i++) // seed the running total to get a realisitic BPM at startup
          {             
            rate[i] = IBI;                      
          }
        }
  
        if(firstBeat) // if it's the first time we found a beat, if firstBeat == TRUE
        {                         
          firstBeat = false;                   // clear firstBeat flag
          secondBeat = true;                   // set the second beat flag
          sei();                               // enable interrupts again
          return;                              // IBI value is unreliable so discard it
        }   
      // keep a running total of the last 10 IBI values
      word runningTotal = 0;                  // clear the runningTotal variable    

      for(int i=0; i<=8; i++)
        {                // shift data in the rate array
          rate[i] = rate[i+1];                  // and drop the oldest IBI value 
          runningTotal += rate[i];              // add up the 9 oldest IBI values
        }

      rate[9] = IBI;                          // add the latest IBI to the rate array
      runningTotal += rate[9];                // add the latest IBI to runningTotal
      runningTotal /= 10;                     // average the last 10 IBI values 
      BPM = 60000/runningTotal;               // how many beats can fit into a minute? that's BPM!
      QS = true;                              // set Quantified Self flag 
      // QS FLAG IS NOT CLEARED INSIDE THIS ISR
    }                       
  }

  if (Signal < thresh && Pulse == true)
    {   // when the values are going down, the beat is over.
      Pulse = false;                         // reset the Pulse flag so we can do it again
      amp = P - T;                           // get amplitude of the pulse wave
      thresh = amp/2 + T;                    // set thresh at 50% of the amplitude
      P = thresh;                            // reset these for next time
      T = thresh;
    }

  if (N > 2500)
    {                           // if 2.5 seconds go by without a beat
      thresh = 512;                          // set thresh default
      P = 512;                               // set P default
      T = 512;                               // set T default
      lastBeatTime = sampleCounter;          // bring the lastBeatTime up to date        
      firstBeat = true;                      // set these to avoid noise
      secondBeat = false;                    // when we get the heartbeat back
    }

  sei();                                   // enable interrupts when youre done!
}// end isr


void MotorForward(void)   
{
  digitalWrite(leftMotorForward,HIGH);
  digitalWrite(rightMotorForward,HIGH);
  digitalWrite(leftMotorBackward,LOW);
  digitalWrite(rightMotorBackward,LOW);
  
}

/********************************************* BACKWARD *****************************************************/
void MotorBackward(void)   
{
  digitalWrite(leftMotorBackward,HIGH);
  digitalWrite(rightMotorBackward,HIGH);
  digitalWrite(leftMotorForward,LOW);
  digitalWrite(rightMotorForward,LOW);
 
}

/********************************************* TURN LEFT *****************************************************/
void TurnLeft(void)   
{ 
  digitalWrite(leftMotorForward,LOW);
  digitalWrite(rightMotorForward,HIGH);
  digitalWrite(rightMotorBackward,LOW);
  digitalWrite(leftMotorBackward,HIGH);  
  
}

/********************************************* TURN RIGHT *****************************************************/
void TurnRight(void)   
{
  digitalWrite(leftMotorForward,HIGH);
  digitalWrite(rightMotorForward,LOW);
  digitalWrite(rightMotorBackward,HIGH);
  digitalWrite(leftMotorBackward,LOW);
  
}

/********************************************* STOP *****************************************************/
void MotorStop(void)   
{ digitalWrite(leftMotorForward,LOW);
  digitalWrite(leftMotorBackward,LOW);
  digitalWrite(rightMotorForward,LOW);
  digitalWrite(rightMotorBackward,LOW);
}





