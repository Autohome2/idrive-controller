/* CAN Loopback Example
 * 
 *   
 *   Written By: Darren Siepka - October 3th 2018
 */

#include <mcp_can.h>
#include <SPI.h>

// CAN TX Variables
unsigned long prevTX = 0;                                        // Variable to store last execution time
const unsigned int invlTX = 1000;                                // One second interval constant
unsigned long prevTX10 = 0;                                        // Variable to store last execution time
const unsigned int invlTX10 = 10000;                                // ten second interval constant
byte data[] = {0xFD, 0x55, 0xFF, 0x10, 0xFF, 0x12, 0x34, 0x56};  // Generic CAN data to send
byte lighthigh[] = {0xFD, 0x55, 0xFF, 0x10, 0xFF, 0x12, 0x34, 0x56};  // Generic CAN data to send
byte lightlow[] = {0x04, 0x55, 0xFF, 0x10, 0xFF, 0x12, 0x34, 0x56};  // Generic CAN data to send
byte data_273[] = {0x1D, 0xE1, 0x00, 0xF0, 0xFF, 0x7F, 0xDE, 0x04};  // Generic CAN data to send
byte data_130[] = {0x45, 0x40, 0x21, 0x8F, 0xFE, 0x00, 0x00, 0x00};  // Generic CAN data to send
byte data_560[] = {0x00, 0x00, 0x00, 0x00, 0x57, 0x2F, 0x00, 0x60};  // Generic CAN data to send


// CAN RX Variables
long unsigned int rxId;
unsigned char len;
unsigned char rxBuf[8];
uint8_t old_button_message = 0 ;
uint8_t old_knob_message = 0;

// Serial Output String Buffer
char msgString[128];

// CAN0 INT and CS
#define CAN0_INT 6                              // Set INT to pin 2
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
  
  Serial.println("MCP2515 Library Loopback Example...");
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
  
  if(millis() - prevTX >= invlTX){                    // Send this at a one second interval. 
    prevTX = millis();
    byte sndStat = CAN0.sendMsgBuf(0x560, 0, 8, data_560); 
    if(sndStat == CAN_OK)
      Serial.println("wakeup Message Sent Successfully!");
    else
      Serial.println("Error Sending wakeup Message...");
  }
   if(millis() - prevTX10 >= invlTX10){                    // Send this at a ten second interval. 
    prevTX10 = millis();  
     if (data[0] == 0xFD){ data[0] = 0x04;} //make backlight low
else if (data[0] == 0x04){ data[0] = 0xFD;} // make backlight high
    byte sndStat = CAN0.sendMsgBuf(0x202, 0, 8, data); 
    if(sndStat == CAN_OK)
      Serial.println("illumination Message Sent Successfully!");
    else
      Serial.println("Error Sending illumination Message...");
  }
  
}

void std_message()
{
  //read and decode std messages
  switch (rxId)
  {
    case 0x264:   //idrive rotary    
        Serial.println("0x264 sent");
  //    sprintf(msgString, "Standard ID: 0x%.3lX       DLC: %1d  Data:", rxId, len);
  //    Serial.print(msgString);
  //    for(byte i = 0; i<len; i++)
  //       {
  //         sprintf(msgString, " 0x%.2X", rxBuf[i]);
  //         Serial.print(msgString);
  //         for(byte i = 0; i<len; i++)
  //           {
  //             sprintf(msgString, " 0x%.2X", rxBuf[i]);
  //             Serial.print(msgString);
   //            Serial.println();
   //          }
   //      }
         // x bytes are used
         // byte 0   0xE1
         // byte 1   0xFD
         // byte 2   this counts each time the code changes
         // byte 3   steps 1 then 2 as button is held, 0x00 when button is released
         // byte 4   0xC0
         // byte 5   is button type
         // byte 6   not used
         // byte 7   not used
         //knob left
         //knob right
         //knob rotate clockwise
         //knob rotate counter-clockwise
         //knob push down
         //knob release up
               
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
           //button audio  0x08
           //button menu   0x01
           //button tel    0x40 
           //button back   0x02
           //button option 0x04
         if (rxBuf[4] == 0xC0) 
           {
            if (rxBuf[3] == 0x01)
              {
               switch (rxBuf[5])
                  {
                    case 0x08:    //button audio
                        Serial.println ("AUDIO step1");
                        old_button_message = 0x08;
                    break;
            
                    case 0x01:    // button menu
                        Serial.println ("MENU step1");
                        old_button_message = 0x01;
                    break; 
            
                    case 0x40:    // button tel
                        Serial.println ("TEL step 1");
                        old_button_message = 0x40;
                    break; 
            
                    case 0x02:    // button back
                        Serial.println ("BACK step1 ");
                        old_button_message = 0x02;
                    break;
            
                    case 0x04:    // button option
                        Serial.println ("OPTION step1 ");
                        old_button_message = 0x04;
                    break;
                  }
              }  
            if (rxBuf[3] == 0x02)
              {
               switch (rxBuf[5])
                  {
                    case 0x08:    //button audio
                       Serial.println ("AUDIO step2 ");
                       old_button_message = 0x08;
                    break;
            
                    case 0x01:    // button menu
                       Serial.println ("MENU step2");
                       old_button_message = 0x01;
                    break; 
            
                    case 0x40:    // button tel
                       Serial.println ("TEL step2");
                       old_button_message = 0x40;
                    break; 
            
                    case 0x02:    // button back
                       Serial.println ("BACK step2");
                       old_button_message = 0x02;
                    break;
            
                    case 0x04:    // button option
                       Serial.println ("OPTION step2");
                       old_button_message = 0x04;
                    break;
                }
             }
             else if (rxBuf[3] == 0x00)    //button released
                 {
                  switch(old_button_message)
                    {
                      case 0x08:
                          Serial.println ("AUDIO RELEASED");
                      break;
              
                      case 0x01:
                          Serial.println ("MENU RELEASED");
                      break;
              
                      case 0x40:
                          Serial.println ("TEL RELEASED");
                      break;
              
                      case 0x02:
                          Serial.println ("BACK RELEASED");
                      break;
             
                      case 0x04:
                         Serial.println ("OPTION RELEASED");
                      break;
              
                      old_button_message = 0x00;
                    } 
                 }
           }
           else if (rxBuf[4] == 0xDD)
               {
                //knob has been pushed left or right                               
                if (rxBuf[3] == 0x81)
                  {
                   Serial.println ("KNOB PUSHED LEFT step1");
                   old_knob_message = 0xDD;
                  }  
                if (rxBuf[3] == 0x82)
                  {
                   Serial.println ("KNOB PUSHED LEFT step2");
                   old_knob_message = 0xDD;
                  }  
                if (rxBuf[3] == 0x21)
                  {
                   Serial.println ("KNOB PUSHED RIGHT step1");
                   old_knob_message = 0xDD;
                  } 
                if (rxBuf[3] == 0x22)
                  {
                   Serial.println ("KNOB PUSHED RIGHT step2");
                   old_knob_message = 0xDD;
                  } 
                else if (rxBuf[3] == 0x00)    //button released
                 {  
                  Serial.println ("KNOB RELEASED");
                   old_knob_message = 0x00;
                 }
               }
           
           else if (rxBuf[4] == 0xDE) 
               {
                if (rxBuf[3] == 0x01)
                   {  
                    //knob has been pushed down
                    Serial.println ("KNOB PUSHED DOWN step1");
                    old_knob_message = 0xDE;
                   }  
                if (rxBuf[3] == 0x02)
                   {  
                    //knob has been pushed down
                    Serial.println ("KNOB PUSHED DOWN step2");
                    old_knob_message = 0xDE;
                   }  
             else if (rxBuf[3] == 0x00)    //button released
                  {  
                    Serial.println ("KNOB RELEASED");
                    old_knob_message = 0x00;
                  }
               } 
               
          else       //rxBuf[4] was not 0xC0,0xDD or 0xDE
          {
            Serial.println();                   
          }
    break;
    
    case 0x5E7:   //reply to the wake up
         
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

/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
