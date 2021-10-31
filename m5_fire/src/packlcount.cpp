/*
*******************************************************************************
* Copyright (c) 2021 by M5Stack
*                  Equipped with M5Core sample source code
*                          配套  M5Core 示例源代码
* Visit the website for more information：https://docs.m5stack.com/en/unit/can
* 获取更多资料请访问：https://docs.m5stack.com/zh_CN/unit/can
*
* describe: can.
* date：2021/9/1
*******************************************************************************
  Please connect to PORT-B,Press buttonA or buttonB to Send message if received message, screen will display
  请连接端口B,按钮A或按钮B发送消息，如果收到消息，屏幕将显示
*/

#include <M5Stack.h>
#include "ESP32CAN.h"
#include "CAN_config.h"

#define TX GPIO_NUM_26
#define RX GPIO_NUM_36

CAN_device_t CAN_cfg; // CAN Config

int i = 0;
uint16_t packlCount = 0;
uint16_t treeCount = 0;

void setup()
{
  M5.begin(true, false, true);
  M5.Power.begin();
  // M5.Lcd.drawString("CAN Fire", 40, 3, 4);
  M5.Lcd.setCursor(0, 60, 4);

  CAN_cfg.speed = CAN_SPEED_250KBPS; //Set the Can speed.  设置Can速度
  CAN_cfg.tx_pin_id = TX;            //Set the Pin foot.  设置引脚
  CAN_cfg.rx_pin_id = RX;

  CAN_cfg.rx_queue = xQueueCreate(10, sizeof(CAN_frame_t));

  ESP32Can.CANInit(); // Init CAN Module.  初始化Can
}

void loop()
{
  CAN_frame_t rx_frame;

  if (M5.BtnA.wasPressed())
  {
    rx_frame.FIR.B.FF = CAN_frame_std;
    rx_frame.MsgID = 100;
    rx_frame.FIR.B.DLC = 1;
    rx_frame.data.u8[0] = 15;

    ESP32Can.CANWriteFrame(&rx_frame);
    M5.Lcd.println("Send Message1");
  }
  if (M5.BtnB.wasPressed())
  {
    rx_frame.FIR.B.FF = CAN_frame_std;
    rx_frame.MsgID = 100;
    rx_frame.FIR.B.DLC = 1;
    rx_frame.data.u8[0] = 'M';

    ESP32Can.CANWriteFrame(&rx_frame);
    M5.Lcd.println("Send Message2");
    Serial.println("B");
  }

  if (xQueueReceive(CAN_cfg.rx_queue, &rx_frame, 3 * portTICK_PERIOD_MS) == pdTRUE)
  {
    // M5.Lcd.setCursor(0, 60, 4);
    //do stuff!
    // if(rx_frame.FIR.B.FF==CAN_frame_std){
    //   printf("New standard frame");
    //   M5.Lcd.printf("New standard frame");
    // } else{
    //   printf("New extended frame");
    //   M5.Lcd.printf("New extended frame");
    // }
    if (rx_frame.FIR.B.RTR == CAN_RTR)
    {
      // printf(" RTR from %d, DLC %d\r\n",rx_frame.MsgID,  rx_frame.FIR.B.DLC);
      // M5.Lcd.printf(" RTR from 0x%08x, DLC %d\r\n",rx_frame.MsgID,  rx_frame.FIR.B.DLC);
    }
    else
    {

      if (rx_frame.MsgID == 512)
      {
        uint16_t data = rx_frame.data.u8[0] + (rx_frame.data.u8[1] << 8);

        if (data != treeCount)
        {
          treeCount = data;
          if (treeCount < 10)
          {
            // M5.Lcd.drawFillRect(100,0,130,90,GREEN);
            M5.Lcd.fillRect(100,0,130,90,BLACK);
            M5.Lcd.drawString(String(treeCount), 140, 3, 8);
          }
          else
          {
            M5.Lcd.drawString(String(treeCount), 110, 3, 8);
          }

          printf("treeCount %d", treeCount);
          printf("\n");
        }
      }

      // printf(" from %d, DLC %d\n",rx_frame.MsgID,  rx_frame.FIR.B.DLC);
      // M5.Lcd.printf(" from %d, DLC %d\r\n",rx_frame.MsgID,  rx_frame.FIR.B.DLC);

      //   printf("%d.", rx_frame.data.u8[0]+(rx_frame.data.u8[1]<<8));
      //   M5.Lcd.printf("%d.", rx_frame.data.u8[0]+(rx_frame.data.u8[1]<<8));
    }
  }

  M5.update();
  delay(100);
}