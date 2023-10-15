#pragma once

#include <QString>

namespace law
{
  /**
   * @struct SRS232DATA
   * @brief Параметры для связи с RS-232 */
  struct SRS232DATA
  {
    QString portName; ///< @brief Имя порта RS-232
    int bountRate; ///< @brief Скорость передачи данных
    int dataBits; ///< @brief Биты данных
    int parity;///< @brief Бит четности
    int stopBits; ///< @brief Стоповый бит
    int flowControl; ///< @brief управление потоком
  };

  /**
   * @enum class EC_MODE
   * @brief Режим работы CAN */
  enum class EC_MODE {
    CLOSE   ///< @brief Закрыт (режим конфигурирования)
,   NORMAL  ///< @brief Нормальный режим работы
,   LOOPBACK ///< @brief маршрутизация без обработки обратно к источнику
,   LISTEN ///< @brief Только прослушка без влияния на сеть
  };

  enum class EC_COMMANDS : char {
      CMD_S = 'S' ///< @brief Настройка стандартных скоростей CAN
  ,   CMD_V = 'V' ///< @brief Запрос версии (ответ версии)
  ,   CMD_C = 'C' ///< @brief Закроет канал CAN
  ,   CMD_O = 'O' ///< @brief Откроет канал CAN
  ,   CMD_L = 'L' ///< @brief Откроет канал CAN для прослушки
  ,   CMD_A = 'A' ///< @deprecated Опрашивает входящий FIFO на наличие в нем фреймов CAN
  ,   CMD_P = 'P' ///< @deprecated Опрашивает входящий FIFO на наличие в нем фреймов CAN (одиночный опрос)
  ,   CMD_F = 'F' ///< @brief Прочитать флаги статуса
  ,   MSG_E = 'E' ///< @brief Сообщение с ошибкой
  ,   MSG_T = 'T' ///< @brief Сообщение CAN с расширенным ID
  ,   MSG_t = 't' ///< @brief Сообщение CAN с нормальным ID
  ,   MSG_R = 'R' ///< @brief Сообщение CAN с расширенным ID фрейм RTR
  ,   MSG_r = 'r' ///< @brief Сообщение CAN с нормальным ID фрейм RTR
  };

  constexpr const char CR = '\r'; ///< @brief Конец сообщения (сообщение выполнено успешно)
  constexpr const char BELL = 7; ///< @brief Обнаружена ошибка
  constexpr const char * SETTINGS_FILENAME= "lawicel_viwer.json"; ///< @brief Имя файла настроек

  constexpr const char * TAG_PORTNAME= "port_name"; ///< @brief Имя порта RS-232
  constexpr const char * TAG_BAUDRATE= "baud_rate"; ///< @brief Скорость передачи данных
  constexpr const char * TAG_DATABITS= "data_bits"; ///< @brief Биты данных
  constexpr const char * TAG_PARITY= "parity"; ///< @brief Бит четности
  constexpr const char * TAG_STOPBITS= "stop_bits"; ///< @brief Стоповый бит
  constexpr const char * TAG_FLOWCONTROL= "flow_control"; ///< @brief управление потоком


  inline unsigned char hexCharToByte(char hex)
  {
    unsigned char result = 0;
    if (hex >= 0x30 && hex <= 0x39) {
      result = hex - 0x30;
    } else if (hex >= 0x41 && hex <= 0x46) {
      result = hex - 0x41 + 0x0A;
    } else if (hex >= 0x61 && hex <= 0x66) {
      result = hex - 0x61 + 0x0A;
    }
    return result;
  }

  /**
   * @brief Преобразует значение байта в строковые 2 байта (пример: 0x6A в {'6', 'A'})
   * @param [byte] Значение, которое необходимо преобразовать
   * @param [buffer] Буфер, куда следует записать полученные символы (должен быть > 2 байт) */
  inline void byteToHexChar(char byte, char * buffer) {
    buffer[0] = (byte & 0xf0) >> 4;
    buffer[1] = (byte & 0x0f);
    for(int i = 0; i < 2; ++i) {
      if (buffer[i] <= 0x9) buffer[i] += 0x30;
      else buffer[i] += 0x61 - 0xA;
    }
  }

} //namespace law
