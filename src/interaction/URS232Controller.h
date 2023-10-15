#pragma once

#include <QObject>

#include "CConstants.h"

namespace law
{
  class MainWindow;
  class URS232Abonent;
  class MessageT;
  struct SRS232DATA;

  class URS232Controller : public QObject
  {
    Q_OBJECT
    typedef void (URS232Controller::*TPROCESSING)();
  public:
    /**
     * @brief URS232Controller Конструктор пщ-умолчанию
     * @param window Интерфейс пользователя
     * @param abonent Модель доставки информации
     * @param parent Родительский объект */
    explicit URS232Controller(MainWindow * window, URS232Abonent * abonent, QObject *parent = nullptr);
    /** @brief Деструктор */
    virtual ~URS232Controller();

  private:
    MainWindow * _View_po;  ///< @brief Интерфейс пользователя
    URS232Abonent * _Model_po;  ///< @brief Модель доставки информации
    TPROCESSING _CurrentProcessing_po; ///< @brief Текущий метод разбора сообщения
    QByteArray * _MessageBuffer_po; ///< @brief Массив байт для обработки

    /** @brief Запрос версии прошивки */
    void RequestVersion_v();

    /** @brief Обработка данных с запросом версии */
    void ProcessingVersion_v();
    /** @brief Обработка данных с пакетом */
    void ProcessingMsgtt();
    /** @brief Обработка данных с пакетом */
    void ProcessingMsgT();
    /** @brief Обработка данных с пакетом */
    void ProcessingMsgrr();
    /** @brief Обработка данных с пакетом */
    void ProcessingMsgR();

  private slots:
    void CheckRS232Settings_slt(const SRS232DATA &);
    void ConnectControl_slt(bool);
    void ConnectChanged_slt(bool);
    /**
      * @brief Установка нового режима работы
      * @param [1] Номер режима работы */
    void SetMode_slt(EC_MODE);
    /**
     * @brief Установка скорости работы CAN
     * @param [1] Скорость работы */
    void SetSpeed_slt(int);
    /**
     * @brief Данные получены
     * @param [1] Массив полученных данных */
    void DataReady_slt(const QByteArray &);
    /**
     * @brief Отправка сообщения
     * [1] Сообщение */
    void SendMessage_slt(const MessageT &);

  };

} // namespace law
