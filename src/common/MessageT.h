#pragma once

#include <cstdint>
#include <array>
#include <QByteArray>

namespace law
{
  /**
   * @class MessageT
   * @brief Класс-сообщения данных от/на CAN */
  class MessageT
  {
    const int ID_LENGTH; ///< @brief Размер ID в символах
  public:
    /**
     * @brief Основной конструктор
     * @param [isShortID] Флаг короткого ID */
    explicit MessageT(bool isShortID);
    /** @brief Дедструктор */
    virtual ~MessageT();
    /**
     * @brief Упаковка данных в ASCII
     * @return Набор символов ASCII, согласно LAWICEL */
    QByteArray coding() const;
    /**
     * @brief Распаковка данных из строки LAWICEL
     * @param [1] Набор символов ASCII */
    void deCoding(const QByteArray &);
    /**
     * @brief Итендификатор CAN сообщения
     * @return ID в числовом формате */
    uint32_t id() const;
    /**
     * @brief Количество байт (до 8)
     * @return Размер массива байт (0 - 8) */
    uint8_t dlc() const;
    /**
     * @brief Возвращает данные
     * @param [i] Номер нужного данного
     * @return Запрашиваемый байт */
    uint8_t data(uint8_t i) const;

    /**
     * @brief Установка итендификатора
     * @param [id] итендификатор */
    void setID(uint32_t id);
    /**
     * @brief Установка количества байт
     * @param [dlc] количество байт */
    void setDLC(uint8_t dlc);
    /**
     * @brief Установка байта блока данных
     * @param [index] индекс 0-7
     * @param [dataByte] байт данных */
    void setData(int index, uint8_t dataByte);

  private:
    bool _IsShortID_b; ///< @brief Флаг короткого (11 бит) ID
    uint32_t _ID_u32; ///< @brief ID CAN фрейма
    uint8_t _DLC_u8; ///< @brief Количество байт
    std::array<uint8_t, 8> _Data_ar; ///< @brief Набор данных
  };

} //namespace law
