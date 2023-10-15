#include <Arduino.h>
#include <mcp2515.h> // Подключаем библиотеку mcp2515

// #define __TEST__

MCP2515 mcp2515(10); //Куда подключаем модуль MCP2615

#ifndef __TEST__
#include <lawicel.h>
Lawicel lawicel(&mcp2515, MCP_8MHZ);
char rs232_buffer[128];
#endif //__TEST__

struct can_frame canMsgRead; //Указываем структуру CAN пакета

void setup()
{
    Serial.begin(115200);
#ifndef __TEST__    
    lawicel.init();
#else __TEST__
    mcp2515.setNormalMode(); // Для проверки
#endif // __TEST__
}

void loop()
{
#ifndef __TEST__    
    if(Serial.available() > 0) {
        size_t bytes = Serial.readBytes(rs232_buffer, 128);
        if (bytes > 0) {
            lawicel.processing(rs232_buffer, bytes);
        }
    }
    
    if (mcp2515.readMessage(&canMsgRead) == MCP2515::ERROR_OK) { //Есть сообщение из CAN шины
        // Serial.println("Yes");
        lawicel.processing(canMsgRead);
        // Serial.print("ID=");
        // Serial.println(canMsgRead.can_id);
        // Serial.print("DLC=");
        // Serial.println(canMsgRead.can_dlc);
        // Serial.print("DATA=[");
        // for (int i = 0; i < canMsgRead.can_dlc; ++i) {
        //     Serial.print((int)canMsgSend.data[i]);
        //     Serial.print(", ");
        // }
        // Serial.println("]");
    }
    // delay(1000);

#else __TEST__
//----- T ----------------------------------------------------------------
    __u32 EID = 0x8000000;
    __u32 EFF = 1UL; // Расширенный режим
    __u32 RTR = 0; // Флаг запроса удаленной передачи (1 - true)
    __u32 ERR = 0; // Данные или описание ошибки 
    struct can_frame canMsgSend; //Указываем структуру CAN пакета

    canMsgSend.can_id = (EFF<<31)|(RTR<<30)|(ERR<<29)|(EID); // 29bit
    canMsgSend.can_dlc = 8;
    canMsgSend.data[0] = 1;
    canMsgSend.data[1] = 2;
    canMsgSend.data[2] = 3;
    canMsgSend.data[3] = 4;
    canMsgSend.data[4] = 5;
    canMsgSend.data[5] = 6;
    canMsgSend.data[6] = 7;
    canMsgSend.data[7] = 8;
    mcp2515.sendMessage(&canMsgSend);
    // lawicel.writeFrameToSerial(EC_COMMANDS::MSG_T, canMsgSend);
    delay(100);
//---- tt ----------------------------------------------------------------
    EID = 0x50;
    EFF = 0UL;
    canMsgSend.can_id = (EFF<<31)|(RTR<<30)|(ERR<<29)|(EID); // 11bit
    canMsgSend.can_dlc = 4;
    canMsgSend.data[0] = 0x1F;
    canMsgSend.data[1] = 0xA2;
    canMsgSend.data[2] = 0x31;
    canMsgSend.data[3] = 0xAA;
    canMsgSend.data[4] = 0;
    canMsgSend.data[5] = 0;
    canMsgSend.data[6] = 0;
    canMsgSend.data[7] = 0;
    mcp2515.sendMessage(&canMsgSend);
    // lawicel.writeFrameToSerial(EC_COMMANDS::MSG_T, canMsgSend);
    delay(1000);
    Serial.println("AAFF");
// //----- R ---------------------------------------------------------------
//     EID = 0x008000;
//     EFF = 1UL; // Расширенный режим
//     RTR = 1UL; // Флаг запроса удаленной передачи (1 - true)
//     ERR = 0; // Данные или описание ошибки 

//     canMsgSend.can_id = (EFF<<31)|(RTR<<30)|(ERR<<29)|(EID); // 29bit
//     canMsgSend.can_dlc = 7;
//     canMsgSend.data[0] = 0x10;
//     canMsgSend.data[1] = 0x20;
//     canMsgSend.data[2] = 0x30;
//     canMsgSend.data[3] = 0x40;
//     canMsgSend.data[4] = 0x50;
//     canMsgSend.data[5] = 0x60;
//     canMsgSend.data[6] = 0x70;
//     canMsgSend.data[7] = 8;
//     mcp2515.sendMessage(&canMsgSend);
//     // lawicel.writeFrameToSerial(EC_COMMANDS::MSG_T, canMsgSend);
//     delay(1000);
// //---- rr ----------------------------------------------------------------
//     EID = 0x55;
//     EFF = 0UL;
//     RTR = 1UL; // Флаг запроса удаленной передачи (1 - true)
//     canMsgSend.can_id = (EFF<<31)|(RTR<<30)|(ERR<<29)|(EID); // 11bit
//     canMsgSend.can_dlc = 4;
//     canMsgSend.data[0] = 0x07;
//     canMsgSend.data[1] = 0x12;
//     canMsgSend.data[2] = 0x31;
//     canMsgSend.data[3] = 0xAA;
//     canMsgSend.data[4] = 0;
//     canMsgSend.data[5] = 0;
//     canMsgSend.data[6] = 0;
//     canMsgSend.data[7] = 0;
//     mcp2515.sendMessage(&canMsgSend);
//     // lawicel.writeFrameToSerial(EC_COMMANDS::MSG_T, canMsgSend);
//     delay(1000);
#endif //__TEST__
}
