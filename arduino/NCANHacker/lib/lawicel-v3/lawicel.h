#pragma once

#include <stdint.h>

#include "lawicel_consts.h"

class MCP2515;
struct can_frame;

/**
 * @class Lawicel 
 * @brief Обрабатывает пришедшие команды от RS-232 согласно протоколу Lawicel */
class Lawicel 
{
    typedef void (Lawicel::*TPROCESSING)();
public:
    const char CR = '\r'; ///< @brief Конец сообщения (сообщение выполнено успешно)
    const char BELL = 7; ///< @brief Обнаружена ошибка

    /** @brief Конструктор */
    explicit Lawicel(MCP2515 * mcp, uint8_t can_freq);  
    /** @brief Деструктор */
    virtual ~Lawicel();

    void init();
    /** 
     * @brief Обработка пришедших данных 
     * @param [data] Данные сообщения
     * @param [length] Количество байт сообщения */
    void processing(const char * data, int length);
    /** 
     * @brief Обработка пришедших данных по CAN
     * @param [frame] Данные сообщения */
    void processing(const can_frame & frame);

private:
    MCP2515 * _MCP2515_po; ///< @brief Указатель на обработчик MCP2515
    TPROCESSING _CurrentProcessing_po; ///< @brief Текущий метод разбора сообщения
    char _MessageBuffer_ac[50]; ///< @brief Буфер сообщения
    int _MessagePosition_i; ///< @brief Позиция записи в буфер
    EC_MODE _Mode_en; ///< @brief Текущий режим работы
    uint8_t _CanFreq_u8; ///< @brief Кварц MCP2515
    uint8_t _CanRate_u8; ///< @brief Скорость работы CAN
    
    /** @brief Обработка данных со скоростью CAN */
    void processingCANSpeed();
    /** @brief Обработка данных с запросом версии */
    void processingVersion();
    /** @brief Обработка данных с установкой режима работы */
    void processingModeClose();
    /** @brief Обработка данных с установкой режима работы */
    void processingModeNormal();
    /** @brief Обработка данных с установкой режима работы */
    void processingModeLoopback();
    /** @brief Обработка данных с установкой режима работы */
    void processingModeListen();
    /** @brief Обработка данных с запросом статусных флагов */
    void processingStatusFlags();
    /** @brief Запрещенный запрос */
    void processingDepricated();
    /** @brief Обработка сообщения с фреймом */
    void processingMsgtt();
    /** @brief Обработка сообщения с фреймом */
    void processingMsgT();
    /** @brief Обработка сообщения с фреймом */
    void processingMsgrr();
    /** @brief Обработка сообщения с фреймом */
    void processingMsgR();
    /** @brief Отсылает команду правильно */
    void writeCommand(char);
    /** @brief Записывает данные в буфер и отправляет в Serial */
    void writeDataToSerial(EC_COMMANDS, const char * value, uint8_t length, char end);
    /** @brief Записывает данные в буфер и отправляет в Serial */
    void writeFrameToSerial(EC_COMMANDS, const can_frame &);
    /** @brief Формируем фрейм для отправки на CAN */
    void formationFrame(can_frame & frame, const char * message);
};