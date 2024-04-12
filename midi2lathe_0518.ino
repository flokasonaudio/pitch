/* Midi to Lathe V1.3 flo@floka.com
*midi in is  based on code done by kuki 
* for new arduino breakoutboard_midi2lathe1
* changed 30-11-2016 for start,stop , fast , mark 
* changed V1.2  for longer mark times on c3,d3
* changed V1.3  18.05.2018 loop function on note 
* ----------------- 
* listen for midi serial data, and light leds for individual notes and set coresponding output to ground 
Function:
this is a midi remote for (neumann lathes)
listen to midi serial data
if one of the four  Notes for START,STOP,FAST, MARK is recieved the output and its led is set for 100ms
############################
HARDWARE NOTE:
The Midi Socket is connected to arduino RX through an opto-isolator to invert the midi signal and seperate the circuits of individual instruments.
connect 4 outputs and 4 otional leds  to pin2-pin9 on your arduino.
Midi Channel1, 

MARK =PIN5 and MARK LED=PIN12= Midi note "C1" =36
FAST =PIN4 and FAST LED=PIN11= Midi note "D1" =38
STOP =PIN3and STOP LED=PIN5= Midi note "E1" =40
START=PIN2 and START LED=PIN3= Midi note "F1" =42
####################################################################################################################################################
*/
//variables setup
byte incomingByte;
byte note;
byte velocity;
int statusLed = 13;   // select the pin for the LED
int action=2; //0 =note off ; 1=note on ; 2= nada

//setup: declaring iputs and outputs and begin serial 
void setup() {
 pinMode(statusLed,OUTPUT);   // declare the LED's pin as output
 pinMode(2,OUTPUT); //START taste
 pinMode(3,OUTPUT); //STOP taste
 pinMode(4,OUTPUT); //FAST taste
 pinMode(5,OUTPUT); //MARK taste
 pinMode(9,OUTPUT);   //  LED4=START orange
 pinMode(10,OUTPUT);  //  LED3=STOP= rot 
 pinMode(11,OUTPUT);  //  LED2=FAST grün
 pinMode(12,OUTPUT);  //  LED1=MARK gelb 

 //start serial with midi baudrate 31250 or 38400 for debugging
 Serial.begin(31250);     
 digitalWrite(statusLed,0);   
 digitalWrite(2,LOW); // START controller are low active. you might use
 digitalWrite(3,LOW); // STOPoptocouplers to prevent any ground loops or other troubles. 
 digitalWrite(4,LOW); // FAST 
 digitalWrite(5,LOW ); // MARK

 digitalWrite(9,HIGH); // led on for test
 digitalWrite(10,HIGH); // led on for test
 digitalWrite(11,HIGH); // led on for test
 digitalWrite(12,HIGH); // led on for test
 delay(300);               // wait 100ms
 digitalWrite(9,LOW); // led off for test
 digitalWrite(10,LOW); // led off for test
 digitalWrite(11,LOW); // led off for test
 digitalWrite(12,LOW); // led off for test
 delay(100);               // wait 100ms
}
//loop: wait for serial data, and interpret the message 
void loop () 
{
  if (Serial.available() > 0) 
  {
    incomingByte = Serial.read();// read the incoming byte:
    // wait for as status-byte, channel 1, note 
     if (incomingByte== 144)//144
        { // note on message starting starting
            action=1;
        }
     else if ( (action==1)&&(note==0) )
         { // if we received a "note on", we wait for the note (databyte)
            note=incomingByte;
         }
     else if ( (action==1)&&(note!=0) )
         { // we just read the velocity to be sure it is a regular midi note on sequence.an option could be to use the velocity to set lenght of the mark-time
            velocity=incomingByte;
            playNote(note);
            note=0;
            velocity=0;
            action=0;
          }
     else{   
         }
  }
  
}



void playNote(byte note )
{  
  if(note==36)         // midinote c1=mark=gelbe led
   {
      digitalWrite(5,HIGH); // sets Port D4=0
      digitalWrite(12,HIGH); // 
      delay(280);               // wait 180ms
      digitalWrite(5,LOW); // reset PORT D4
      digitalWrite(12,LOW); 
      delay(100);  
     }   
  if(note ==38)         // midinote D1=FAST=grün
   {
      digitalWrite(4,HIGH); // sets Port D5=0
      digitalWrite(11,HIGH); // 
      delay(280);               // wait 180ms
      digitalWrite(4,LOW); //  
      digitalWrite(11,LOW); 
      delay(100);   
    }
 if(note ==40)         // midinote E1=STOP=rote led
   {
      digitalWrite(3,HIGH); // sets Port D6=0
      digitalWrite(10,HIGH); // 
      delay(280);               // wait 180ms
      digitalWrite(3,LOW); //  
      digitalWrite(10,LOW); 
      delay(100);   
    }
   if(note ==41)         // midinote F1 START orange led
   {
      digitalWrite(2,HIGH); // sets Port D7=0
      digitalWrite(9,HIGH); // 
      delay(280);               // wait 180ms
      digitalWrite(2,LOW); //  
      digitalWrite(9,LOW); 
      delay(100);   
    }  
//** special funktipn mark mit langen Zeiten //
//******************************************//

if(note==48)         // midinote c2=mark=gelbe led
   {
      digitalWrite(5,HIGH); // sets Port D5=0
      digitalWrite(12,HIGH); // 
      delay(1000);               // wait 180ms
      digitalWrite(5,LOW); // reset PORT D5
      digitalWrite(12,LOW); 
      delay(100);  
     } 

     if(note==50)         // midinote D2=mark=gelbe led
   {
      digitalWrite(5,HIGH); // sets Port D5=0
      digitalWrite(12,HIGH); // 
      delay(2000);               // wait 180ms
      digitalWrite(5,LOW); // reset PORT D5
      digitalWrite(12,LOW); 
      delay(100);  
     } 

     if(note==52)         // midinote E2=mark=gelbe led
   {
      digitalWrite(5,HIGH); // sets Port D5=0
      digitalWrite(12,HIGH); // 
      delay(3000);               // wait 180ms
      digitalWrite(5,LOW); // reset PORT D5
      digitalWrite(12,LOW); 
      delay(100);  
     } 

     if(note==53)         // midinote F2=mark=gelbe led
   {
      digitalWrite(5,HIGH); // sets Port D5=0
      digitalWrite(12,HIGH); // 
      delay(4000);               // wait 180ms
      digitalWrite(5,LOW); // reset PORT D5
      digitalWrite(12,LOW); 
      delay(100);  
     } 

      if(note==55)         // midinote G2=mark=gelbe led
   {
      digitalWrite(5,HIGH); // sets Port D5=0
      digitalWrite(12,HIGH); // 
      delay(4000);               // wait 180ms
      digitalWrite(5,LOW); // reset PORT D5
      digitalWrite(12,LOW); 
      delay(100);  
     } 

           if(note==57)         // midinote A2=mark=gelbe led
   {
      digitalWrite(5,HIGH); // sets Port D5=0
      digitalWrite(12,HIGH); // 
      delay(5000);               // wait 180ms
      digitalWrite(5,LOW); // reset PORT D5
      digitalWrite(12,LOW); 
      delay(100);  
     } 

 // Loop  function//  
 if(note==59)         // midinote B2 start loop function.
   {
     // write start. head hoovers and is ready.
     // write mark ..writes loop 
      digitalWrite(9,HIGH); //  LED START on 
      digitalWrite(12,HIGH); // LED MARK on 
      digitalWrite(2,HIGH); // press start
      delay(100);
      digitalWrite(2,LOW); // release start 
      delay(50);
      digitalWrite(5,HIGH); // press mark and write loop 
      delay(100);
      digitalWrite(5,LOW); // reset PORT D4
      digitalWrite(9,LOW); 
      digitalWrite(12,LOW); 
      delay(100);  
     } 
}
