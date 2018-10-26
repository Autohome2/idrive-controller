
void do_wakeup()
 {
    byte sndStat = CAN0.sendMsgBuf(0x560, 0, 8, data_560); 
    if(sndStat == CAN_OK)
      {
//   Serial.println("wakeup Message Sent Successfully!");
      }
    else
      {
      Serial.println("Error Sending wakeup Message...");
      }
 }

void do_rotary_init()
 {
   byte sndStat = CAN0.sendMsgBuf(0x273, 0, 8, data_273); 
    if(sndStat == CAN_OK)
      {
//      Serial.println("rotary init Message Sent Successfully!");
      }
    else
      {
      Serial.println("Error Sending rotary init Message...");
      } 
 }

void do_backlight(byte bright)
 {
     if (data[0] == 0xFD){ data[0] = 0x04;} //make backlight low
else if (data[0] == 0x04){ data[0] = 0xFD;} // make backlight high
     byte sndStat = CAN0.sendMsgBuf(0x202, 0, 8, data); 
     if(sndStat == CAN_OK)
       {
//      Serial.println("illumination Message Sent Successfully!");
       }
    else
       {
        Serial.println("Error Sending illumination Message...");
       }
 }


