/* BMW Idrive DEMO
 * THis is for the F30 style 7 button 4 direction type controller
 *   
 *   Written By: Darren Siepka - October 25th 2018
 */

#include <mcp_can.h>
#include <SPI.h>
#include "idrive_messages.h"

// CAN TX Variables
unsigned long prevTX750 = 0;                                        // Variable to store last execution time
const unsigned int invlTX750 = 750;                                // 750ms interval constant
unsigned long prevTX = 0;                                        // Variable to store last execution time
const unsigned int invlTX = 1000;                                // One second interval constant
unsigned long prevTX10 = 0;                                        // Variable to store last execution time
const unsigned int invlTX10 = 10000;                                // ten second interval constant
byte data[] = {0xFD, 0x55, 0xFF, 0x10, 0xFF, 0x12, 0x34, 0x56};  // Generic CAN data to send
byte lighthigh[] = {0xFD, 0x55, 0xFF, 0x10, 0xFF, 0x12, 0x34, 0x56};  // Generic CAN data to send
byte lightlow[] = {0x04, 0x55, 0xFF, 0x10, 0xFF, 0x12, 0x34, 0x56};  // Generic CAN data to send
byte data_273[] = {0x1D, 0xE1, 0x00, 0xF0, 0xFF, 0x7F, 0xDE, 0x04};  // Generic CAN data to send    send 8 bttes
byte data_130[] = {0x45, 0x40, 0x21, 0x8F, 0xFE, 0x00, 0x00, 0x00};  // Generic CAN data to send    send 5 bytes
byte data_26E[] = {0x00, 0x40, 0x7F, 0x50, 0xFF, 0xFF, 0xFF, 0xFF};  // Generic CAN data to send    send 8 bytes
byte data_560[] = {0x00, 0x00, 0x00, 0x00, 0x57, 0x2F, 0x00, 0x60};  // Generic CAN data to send

// CAN RX Variables
long unsigned int rxId;
unsigned char len;
unsigned char rxBuf[8];
uint8_t last_message_count;
uint8_t old_button_message = 0 ;
uint8_t old_rotary_message = 0;
uint8_t old_knob_message = 0;

// Serial Output String Buffer
char msgString[128];

// CAN0 INT and CS
#define CAN0_INT 6                              // Set INT to pin 6
MCP_CAN CAN0(10);                               // Set CS to pin 10


void setup()
{
  Serial.begin(115200);  // CAN is running at 500,000BPS; 115,200BPS is SLOW, not FAST, thus 9600 is crippling.
  
  // Initialize MCP2515 running at 8MHz with a baudrate of 500kb/s and the masks and filters disabled.
  if(CAN0.begin(MCP_ANY, CAN_500KBPS, MCP_8MHZ) == CAN_OK)
    Serial.println("MCP2515 Initialized Successfully!");
  else
    Serial.println("Error Initializing MCP2515...");
  
  // Since we do not set NORMAL mode, we are in loopback mode by default.
  CAN0.setMode(MCP_NORMAL);

  pinMode(CAN0_INT, INPUT);                           // Configuring pin for /INT input
  
  //Serial.println("MCP2515 Library Loopback Example...");
}

void loop()
{

  if(!digitalRead(CAN0_INT))                          // If CAN0_INT pin is low, read receive buffer
  {
    CAN0.readMsgBuf(&rxId, &len, rxBuf);              // Read data: len = data length, buf = data byte(s)
    
    if((rxId & 0x80000000) == 0x80000000)             // Determine if ID is standard (11 bits) or extended (29 bits)
    {
      sprintf(msgString, "Extended ID: 0x%.8lX  DLC: %1d  Data:", (rxId & 0x1FFFFFFF), len);
      Serial.print(msgString);
      Serial.println();
      //extended message    
    }
    else
    {
    //  sprintf(msgString, "Standard ID: 0x%.3lX       DLC: %1d  Data:", rxId, len);
    //  Serial.print(msgString);
    //  Serial.println();      
      std_message();
    }  

    if((rxId & 0x40000000) == 0x40000000) // Determine if message is a remote request frame.
      {
      sprintf(msgString, " REMOTE REQUEST FRAME");
      Serial.print(msgString);
      Serial.println();
      } 
  else
     {
      for(byte i = 0; i<len; i++)
         {
   //     sprintf(msgString, " 0x%.2X", rxBuf[i]);
   //     Serial.print(msgString);
         }
     }
             
  }
  
  if(millis() - prevTX >= invlTX750)                    // Send this at a 750ms interval.
    { 
    prevTX750 = millis();
    
    }

  if(millis() - prevTX >= invlTX)                       // Send this at a one second interval. 
    {
    prevTX = millis();
    do_wakeup();
    do_rotary_init();
    }
  
   if(millis() - prevTX10 >= invlTX10)                 // Send this at a ten second interval. 
    {
    prevTX10 = millis(); 

//    do_backlight(0); 
    }   
}

void std_message()
{
  //read and decode std messages
  switch (rxId)
  {
    case 0x264:   //idrive rotary    
        // Serial.println("0x264 received");
        // print_the_string();
        
         // x bytes are used
         // byte 0   0xE1
         // byte 1   0xFD
         // byte 2   this counts each time the code changes
         // byte 3   increases from 0x00 upwards for clockwise turn or decreases from 0xFE for counter clockwise turn according to speed of turn
         // byte 4   0x80 clockwise turn, 0x7F counterclockwise turn
         // byte 5   0x1E
         // byte 6   not used
         // byte 7   not used

         switch (rxBuf[4])
           {            
           
           case 0x80:                  //clockwise rotation
                if (last_message_count != rxBuf[2])
                  { 
                   Serial.println ("CLOCKWISE ROTATION");
                   last_message_count = rxBuf[2];
                   old_rotary_message = 0x80;
                  } 
           break;
           
           case 0x7F:                  //counter clockwise rotation
                if (last_message_count != rxBuf[2])
                  {
                   Serial.println ("COUNTER CLOCKWISE ROTATION");
                   last_message_count = rxBuf[2];
                   old_rotary_message = 0x7F;
                  } 
           break;
           }
    break;

    case 0x267:   //idrive buttons    
  //    sprintf(msgString, "Standard ID: 0x%.3lX       DLC: %1d  Data:", rxId, len);
  //    Serial.print(msgString);
  //    for(byte i = 0; i<len; i++)
  //       {
  //         sprintf(msgString, " 0x%.2X", rxBuf[i]);
  //         Serial.print(msgString);
  //         Serial.println();
  //       }
         // only 6 bytes are used
         // byte 0   0xE1
         // byte 1   0xFD
         // byte 2   this counts each time the code changes
         // byte 3   steps 1 then 2 as button is held, 0x00 when button is released or 81,82 for left knob push  and 21,22 for right knob push and 01,02 for knob push down
         // byte 4   0xC0 for buttons, 0xDD for left right and 0xDE for knob push 
         // byte 5   is button type , is 0x00 for left right knob push and 0x01 for knob push down
         // byte 6   not used
         // byte 7   not used

           //button menu   0x01
           //button back   0x02
           //button option 0x04
           //button radio  0x08
           //button media  0x10
           //button nav    0x20                      
           //button tel    0x40 

         switch (rxBuf[4])
           {
            
           case 0xC0:                  //button pushes
         
            if (rxBuf[3] == 0x01)
              {
               switch (rxBuf[5])
                  {
                    case 0x01:    // button menu
                      Serial.println ("MENU step1");
                      old_button_message = 0x01;
                    break;

                    case 0x02:    // button back
                        Serial.println ("BACK step1");
                        old_button_message = 0x02;
                    break;
                                        
                    case 0x04:    // button option
                        Serial.println ("OPTION step1");
                        old_button_message = 0x04;
                    break;                    
                    
                    case 0x08:    //button radio
                        Serial.println ("RADIO step1");
                        old_button_message = 0x08;
                    break;  
            
                    case 0x10:    // button media
                        Serial.println ("MEDIA step1 ");
                        old_button_message = 0x10;
                    break;
            
                    case 0x20:    // button nav
                        Serial.println ("NAV step1 ");
                        old_button_message = 0x20;
                    break;

                    case 0x40:    // button tel
                        Serial.println ("TEL step 1");
                        old_button_message = 0x40;
                    break;                    
                 }
              }  
              
            if (rxBuf[3] == 0x02)
              {
               switch (rxBuf[5])
                  {
                    case 0x01:    // button menu
                      Serial.println ("MENU step2");
                      old_button_message = 0x01;
                    break;

                    case 0x02:    // button back
                        Serial.println ("BACK step2");
                        old_button_message = 0x02;
                    break;
                                        
                    case 0x04:    // button option
                        Serial.println ("OPTION step2");
                        old_button_message = 0x04;
                    break;                    
                    
                    case 0x08:    //button radio
                        Serial.println ("RADIO step2");
                        old_button_message = 0x08;
                    break;  
            
                    case 0x10:    // button media
                        Serial.println ("MEDIA step2 ");
                        old_button_message = 0x10;
                    break;
            
                    case 0x20:    // button nav
                        Serial.println ("NAV step2");
                        old_button_message = 0x20;
                    break;

                    case 0x40:    // button tel
                        Serial.println ("TEL step2");
                        old_button_message = 0x40;
                    break;                    
                }
             }
             else if (rxBuf[3] == 0x00)    //button released
                 {
                  switch(old_button_message)
                    {
                      case 0x01:
                          Serial.println ("MENU RELEASED");
                      break;
                      
                      case 0x02:
                          Serial.println ("BACK RELEASED");
                      break;
                      
                      case 0x04:
                         Serial.println ("OPTION RELEASED");
                      break;
                      
                      case 0x08:
                          Serial.println ("RADIO RELEASED");
                      break;                                  
              
                      case 0x10:
                          Serial.println ("MEDIA RELEASED");
                      break;
              
                      case 0x20:
                         Serial.println ("NAV RELEASED");
                      break;

                      case 0x40:
                         Serial.println ("TEL RELEASED");
                      break;                      
                           
                      old_button_message = 0x00;
                    } 
                 }
         break;

         case 0xDD:     //knob has been pushed left ,right , up or down                                          
                
                if (rxBuf[3] == 0x81)
                  {
                   if(old_knob_message != 0x81)
                     { 
                      Serial.println ("KNOB PUSHED LEFT step1");
                      old_knob_message = 0x81;
                     } 
                  }  
                if (rxBuf[3] == 0x82)
                  {
                   if(old_knob_message != 0x82)
                     { 
                      Serial.println ("KNOB PUSHED LEFT step2");
                      old_knob_message = 0x82;
                     } 
                  }  
                if (rxBuf[3] == 0x21)
                  {
                   if(old_knob_message != 0x21)
                     { 
                      Serial.println ("KNOB PUSHED RIGHT step1");
                      old_knob_message = 0x21;
                     } 
                  } 
                if (rxBuf[3] == 0x22)
                  {
                   if(old_knob_message != 0x22)
                     {
                      Serial.println ("KNOB PUSHED RIGHT step2");
                      old_knob_message = 0x22;
                     } 
                  } 
                if (rxBuf[3] == 0x41)
                  {
                   if(old_knob_message != 0x41)
                     {
                      Serial.println ("KNOB PUSHED DOWN step1");
                      old_knob_message = 0x41;
                     } 
                  } 
                if (rxBuf[3] == 0x42)
                  {
                   if(old_knob_message != 0x42)
                     {
                      Serial.println ("KNOB PUSHED DOWN step2");
                      old_knob_message = 0x42;
                     } 
                  } 
                if (rxBuf[3] == 0x11)
                  {
                   if(old_knob_message != 0x11)
                     {
                      Serial.println ("KNOB PUSHED UP step1");
                      old_knob_message = 0x11;
                     } 
                  } 
                if (rxBuf[3] == 0x12)
                  {
                   if(old_knob_message != 0x12)
                     {
                      Serial.println ("KNOB PUSHED UP step2");
                      old_knob_message = 0x12;
                     } 
                  }                
                     
                else if (rxBuf[3] == 0x00)    //button released
                 {
                  if(old_knob_message != 0x00)
                     {  
                      Serial.println ("KNOB PUSH RELEASED");
                      old_knob_message = 0x00;
                     } 
                 }
               
           break;
           
           case 0xDE:             //knob pressed down or released up
               
                if (rxBuf[3] == 0x01)
                   {  
                    //knob has been pushed down
                    Serial.println ("KNOB PRESSED DOWN step1");
                    old_knob_message = 0xDE;
                   }  
                if (rxBuf[3] == 0x02)
                   {  
                    //knob has been pushed down
                    Serial.println ("KNOB PRESSED DOWN step2");
                    old_knob_message = 0xDE;
                   }  
             else if (rxBuf[3] == 0x00)    //button released
                  {  
                    Serial.println ("KNOB PRESS RELEASED");
                    old_knob_message = 0x00;
                  }               
          break;
               
          default:       //rxBuf[4] was not 0xC0,0xDD or 0xDE
          {
            Serial.println();                   
          }
          break;
        }  

    break;

    case 0x277:      // response to the 0x273 sent to the idrive controller

    break;
    
    case 0x5E7:   //reply to the wake up
         //print_the_string();
    break;

    case 0x567:
         //print_the_string();
    break;

    case 0x000:
        //ignore blank addresses
    break;
    
    default:       //print anything else we havent checked for
          sprintf(msgString, "Standard ID: 0x%.3lX       DLC: %1d  Data:", rxId, len);
          Serial.print(msgString);
          for(byte i = 0; i<len; i++)
          {
           sprintf(msgString, " 0x%.2X", rxBuf[i]);
           Serial.print(msgString);
           Serial.println();
          }

    break;
  }  
}

void print_the_string()
{
           sprintf(msgString, "Standard ID: 0x%.3lX       DLC: %1d  Data:", rxId, len);
         Serial.print(msgString);
         for(byte i = 0; i<len; i++)
          {
           sprintf(msgString, " 0x%.2X", rxBuf[i]);
           Serial.print(msgString);
           Serial.println();
          }
}
/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
