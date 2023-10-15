#include "lawicel.h"

#include <mcp2515.h>

const char hex_ascii[] = {"0123456789ABCDEF"};

Lawicel::Lawicel(MCP2515 * mcp, uint8_t can_freq)
: _MCP2515_po(mcp)
, _CurrentProcessing_po(nullptr)
, _MessagePosition_i(0)
, _CanFreq_u8(can_freq)
, _CanRate_u8(CAN_100KBPS)
{

}

Lawicel::~Lawicel()
{

}

void Lawicel::init()
{
    _MCP2515_po->reset();
    _MCP2515_po->setConfigMode();
    _Mode_en = EC_MODE::CLOSE;
}

void Lawicel::processing(const char * data, int length)
{
    for (int i = 0; i < length; ++i) {
        if (_CurrentProcessing_po == nullptr) {
            _MessagePosition_i = 0;
            if (data[i] == (char)EC_COMMANDS::CMD_S) { _CurrentProcessing_po = &Lawicel::processingCANSpeed; }
            else if (data[i] == (char)EC_COMMANDS::CMD_V) { _CurrentProcessing_po = &Lawicel::processingVersion; }
            else if (data[i] == (char)EC_COMMANDS::CMD_C) { _CurrentProcessing_po = &Lawicel::processingModeClose; }
            else if (data[i] == (char)EC_COMMANDS::CMD_O) { _CurrentProcessing_po = &Lawicel::processingModeNormal; }
            else if (data[i] == (char)EC_COMMANDS::CMD_L) { _CurrentProcessing_po = &Lawicel::processingModeListen; }
            else if (data[i] == (char)EC_COMMANDS::CMD_A) { _CurrentProcessing_po = &Lawicel::processingDepricated; }
            else if (data[i] == (char)EC_COMMANDS::CMD_P) { _CurrentProcessing_po = &Lawicel::processingDepricated; }
            else if (data[i] == (char)EC_COMMANDS::CMD_F) { _CurrentProcessing_po = &Lawicel::processingStatusFlags; }     
            else if (data[i] == (char)EC_COMMANDS::MSG_t) { _CurrentProcessing_po = &Lawicel::processingMsgtt; }       
            else if (data[i] == (char)EC_COMMANDS::MSG_T) { _CurrentProcessing_po = &Lawicel::processingMsgT; }    
            else if (data[i] == (char)EC_COMMANDS::MSG_r) { _CurrentProcessing_po = &Lawicel::processingMsgrr; }    
            else if (data[i] == (char)EC_COMMANDS::MSG_R) { _CurrentProcessing_po = &Lawicel::processingMsgR; }     
        } else {
            if (data[i] == CR) {
                (this->*_CurrentProcessing_po)();
                _CurrentProcessing_po = nullptr;
            } else {
                _MessageBuffer_ac[_MessagePosition_i++] = data[i];
            }
            
        }
    }
}

void Lawicel::processing(const can_frame & frame)
{
    int isRTR = (frame.can_id & CAN_RTR_FLAG) ? 1 : 0;
    if (frame.can_id & CAN_ERR_FLAG) {
        
    } else if (frame.can_id & CAN_EFF_FLAG) { // Расширенный ID (29 бит)
        writeFrameToSerial(isRTR ? EC_COMMANDS::MSG_R : EC_COMMANDS::MSG_T, frame);
    } else { // Нормальный ID (11 бит)
        writeFrameToSerial(isRTR ? EC_COMMANDS::MSG_r : EC_COMMANDS::MSG_t, frame);
    }
}

void Lawicel::processingCANSpeed()
{
    if (_MessagePosition_i == 1) { // Тут размер константный
        char value = _MessageBuffer_ac[_MessagePosition_i - 1];
        if (value >= 0x30 && value <= 0x38) { // Обработка
            value = value - 0x30;
            if (_Mode_en == EC_MODE::CLOSE) { // Установка только в конфигурационном режиме
                uint8_t rate = 0;
                switch (value)
                {
                case 0: rate = CAN_10KBPS; break;
                case 1: rate = CAN_20KBPS; break;
                case 2: rate = CAN_50KBPS; break;
                case 3: rate = CAN_100KBPS; break;
                case 4: rate = CAN_125KBPS; break;
                case 5: rate = CAN_250KBPS; break;
                case 6: rate = CAN_500KBPS; break;
                case 7: rate = CAN_500KBPS; break; // CAN_800KBPS нету
                case 8: rate = CAN_1000KBPS; break;
                }
                _CanRate_u8 = rate;
                // Serial.print("Set bitrate ");
                // Serial.print(_CanRate_u8);
                // Serial.print("  ");
                // Serial.println(_CanFreq_u8);
                MCP2515::ERROR error = _MCP2515_po->setBitrate(CAN_SPEED(_CanRate_u8), CAN_CLOCK(_CanFreq_u8));
                if (error == MCP2515::ERROR_OK) {
                    writeCommand(CR);
                } else {
                    writeDataToSerial(EC_COMMANDS::MSG_E, "004", 3, BELL);
                }
            }
        } else {
            writeDataToSerial(EC_COMMANDS::MSG_E, "001", 3, BELL);
        }
    } else {
        writeDataToSerial(EC_COMMANDS::MSG_E, "002", 3, BELL);
    }
}

void Lawicel::processingVersion()
{
    char hw = 0x1, sw = (0x1 << 2) + static_cast<uint8_t>(_Mode_en); 
    char buffer[4];
    byteToHexChar(hw, buffer);
    byteToHexChar(sw, buffer + 2);
    writeDataToSerial(EC_COMMANDS::CMD_V, buffer, 4, CR);
}

    void Lawicel::processingModeClose()
    {
        if (_Mode_en != EC_MODE::CLOSE) {
            _MCP2515_po->setConfigMode();
            _Mode_en = EC_MODE::CLOSE;
            writeCommand(CR);
        } else {
            writeCommand(BELL);
        }
    }

    void Lawicel::processingModeNormal()
    {
        if (_Mode_en == EC_MODE::CLOSE) {
            _MCP2515_po->setNormalMode();
            _Mode_en = EC_MODE::NORMAL;
            writeCommand(CR);
        } else {
            writeCommand(BELL);
        }
    }

    void Lawicel::processingModeLoopback()
    {
        if (_Mode_en == EC_MODE::CLOSE) {
            _MCP2515_po->setLoopbackMode();
            _Mode_en = EC_MODE::LOOPBACK;
            writeCommand(CR);
        } else {
            writeCommand(BELL);
        }
    }

void Lawicel::processingModeListen()
{
    if (_Mode_en == EC_MODE::CLOSE) {
        _MCP2515_po->setListenOnlyMode();
        _Mode_en = EC_MODE::LISTEN;
        writeCommand(CR);
    } else {
        writeCommand(BELL);
    }
}

void Lawicel::processingStatusFlags()
{
    if (_Mode_en != EC_MODE::CLOSE) {
        writeDataToSerial(EC_COMMANDS::CMD_F, "00", 2, CR);
    } else {
        writeCommand(BELL);
    }
}

void Lawicel::processingDepricated()
{
    if (_Mode_en != EC_MODE::CLOSE) {
        writeCommand(BELL);
    }
}

void Lawicel::processingMsgtt()
{
    can_frame frame;
    union { uint32_t value; uint8_t buffer[4]; } un_m;
    un_m.value = 0;
    un_m.buffer[1] = hexCharToByte(_MessageBuffer_ac[0]);
    un_m.buffer[0] = (hexCharToByte(_MessageBuffer_ac[1])) << 4;
    un_m.buffer[0] += hexCharToByte(_MessageBuffer_ac[2]);
    frame.can_id = un_m.value;
    frame.can_dlc = _MessageBuffer_ac[3] - 0x30;
    formationFrame(frame, &_MessageBuffer_ac[4]);
}

void Lawicel::processingMsgT()
{
    can_frame frame;
    union { uint32_t value; uint8_t buffer[4]; } un_m;
    un_m.value = 0;

    for(uint32_t i = 0; i < 8; ++i) {
      int index = 3 - (i / 2);
      un_m.buffer[index] = (hexCharToByte(_MessageBuffer_ac[i])) << 4;
      un_m.buffer[index] += hexCharToByte(_MessageBuffer_ac[++i]);
    }
    frame.can_id = (1UL<<31)|(un_m.value); // 29bit
    frame.can_dlc = _MessageBuffer_ac[8] - 0x30;
    formationFrame(frame, &_MessageBuffer_ac[9]);
}

void Lawicel::processingMsgrr()
{
    can_frame frame;
    union { uint32_t value; uint8_t buffer[4]; } un_m;
    un_m.value = 0;
    un_m.buffer[1] = hexCharToByte(_MessageBuffer_ac[0]);
    un_m.buffer[0] = (hexCharToByte(_MessageBuffer_ac[1])) << 4;
    un_m.buffer[0] += hexCharToByte(_MessageBuffer_ac[2]);
    frame.can_id = (1UL<<30)|(un_m.value);
    frame.can_dlc = _MessageBuffer_ac[3] - 0x30;
    _MCP2515_po->sendMessage(&frame);
    writeCommand(CR);
}

void Lawicel::processingMsgR()
{
    can_frame frame;
    union { uint32_t value; uint8_t buffer[4]; } un_m;
    un_m.value = 0;

    for(uint32_t i = 0; i < 8; ++i) {
      int index = 3 - (i / 2);
      un_m.buffer[index] = (hexCharToByte(_MessageBuffer_ac[i])) << 4;
      un_m.buffer[index] += hexCharToByte(_MessageBuffer_ac[++i]);
    }
    frame.can_id = (1UL<<31)|(1UL<<30)|(un_m.value); // 29bit
    frame.can_dlc = _MessageBuffer_ac[8] - 0x30;
    _MCP2515_po->sendMessage(&frame);
    writeCommand(CR);
}

void Lawicel::writeCommand(char value) 
{
    char buffer[] = {value, '\0'};
    Serial.write(buffer);
    Serial.flush();
}

void Lawicel::writeDataToSerial(EC_COMMANDS cmd, const char * value, uint8_t length, char end)
{
    char buffer[length + 3];
    buffer[0] = char(cmd);
    buffer[length + 1] = end;
    buffer[length + 2] = '\0';
    memcpy(buffer + 1, value, length);
    Serial.write(buffer);
    Serial.flush();
}

void Lawicel::writeFrameToSerial(EC_COMMANDS cmd, const can_frame & frame)
{
    const int idCount = (cmd == EC_COMMANDS::MSG_t || cmd == EC_COMMANDS::MSG_r) ? 3 : 8;
    const int bufferLength = idCount + 1 + ((cmd == EC_COMMANDS::MSG_t || cmd == EC_COMMANDS::MSG_T) ? frame.can_dlc * 2 : 0) + 3;
    char buffer[bufferLength] = { 0 };
    buffer[0] = char(cmd);
    union { __u32 value; uint8_t buffer[4]; } un_m;
    un_m.value = frame.can_id;
    // Обнулим биты управления
    un_m.value &= ~(1UL << 31); 
    un_m.value &= ~(1UL << 30);
    un_m.value &= ~(1UL << 29);
    if (cmd == EC_COMMANDS::MSG_t || cmd == EC_COMMANDS::MSG_r) {
        buffer[1] = hex_ascii[(un_m.buffer[1] & 0xF0) >> 4];
        buffer[2] = hex_ascii[(un_m.buffer[0] & 0xF0) >> 4];
        buffer[3] = hex_ascii[(un_m.buffer[0] & 0x0F)];
    } else {
        for (int i = 0; i < 4; ++i) {
            buffer[1 + i * 2] = hex_ascii[(un_m.buffer[3 - i] & 0xF0) >> 4];
            buffer[1 + i * 2 + 1] = hex_ascii[(un_m.buffer[3 - i] & 0x0F)];
        }
    }
    buffer[idCount + 1] = hex_ascii[frame.can_dlc];
    int end = idCount + 2;
    if (cmd == EC_COMMANDS::MSG_t || cmd == EC_COMMANDS::MSG_T) { // Отправляем еще и данные
        for (int i = 0; i < frame.can_dlc; ++i) {
            buffer[end++] = hex_ascii[(frame.data[i] & 0xF0) >> 4];
            buffer[end++] = hex_ascii[frame.data[i] & 0x0F];
        }
    }
    buffer[end] = CR;
    buffer[end + 1] = '\0';

    Serial.write(buffer);
    Serial.flush();
}

void Lawicel::formationFrame(can_frame & frame, const char * message)
{
    for (int i = 0; i < frame.can_dlc; ++i) {
        frame.data[i] = (hexCharToByte(message[i * 2]))<<4;
        frame.data[i] += (hexCharToByte(message[i * 2 + 1]));
    }
    _MCP2515_po->sendMessage(&frame);
    writeCommand(CR);
}
