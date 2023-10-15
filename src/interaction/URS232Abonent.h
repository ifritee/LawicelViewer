#pragma once

#include <QObject>

class QTimer;
class QSerialPort;

namespace law
{

  struct SRS232DATA;

  /**
   * @class URS232Abonent
   * @brief Класс приема и первичной */
  class URS232Abonent : public QObject
  {
    Q_OBJECT
  public:
    /**
     * @brief Конструктор
     * @param [parent] Родительский объект */
    explicit URS232Abonent(QObject *parent = nullptr);
    /** @brief Деструктор */
    virtual ~URS232Abonent();
    /**
     * @brief Задание новых настроек
     * @param [1] Данные установок RS-232 */
    void setSettings_v(const SRS232DATA & );
    /**
     * @brief Управление соединением по порту RS-232
     * @param isConnect true - открыть соединение, false - разорвать соединение */
    void SetConnection_v(bool isConnect);
    /**
     * @brief Отправляет данные в канал RS-232
     * @param [1] Массив данных */
    void SendData_v(const QByteArray &);

  private:
    QSerialPort * _DevicePort_po; ///< @brief Порт взаимодействия с устройством
    bool _IsConnected_b; ///< @brief Флаг соединения
    bool _IsInitialize_b; ///< @brief Флаг инициализации

  signals:
    /**
     * @brief Сигнал о смене состояния связи
     * @param [1] true - соединение установленно, false - соединение разорвано */
    void ConnectChanged_sig(bool);
    /**
     * @brief Данные получены
     * @param [1] Массив полученных данных */
    void DataReady_sig(const QByteArray &);
    /**
     * @brief Сообщение с ошибкой
     * @param [1] Строка ошибки */
    void Error_sig(const QString &);

  private slots:
    /** @brief Готовность к считыванию данных */
    void ReadData_slt();
  };

}
