#pragma once

#include <QWidget>
#include <QMap>
#include <array>

namespace Ui {
  class WTransmit;
}

class QTimer;
class QLineEdit;

namespace law
{
  class MessageT;

  class WTransmit : public QWidget
  {
    Q_OBJECT

  public:
    explicit WTransmit(QWidget *parent = nullptr);
    ~WTransmit();

  private:
    Ui::WTransmit *_Widget_po;
    QTimer * _TxTimer_po; ///< @brief Таймер периодической отправки сообщения
    QMap<uint32_t, quint32 > _Counts_map; ///< @brief Счетчик
    std::array<QLineEdit *, 8> _Datas_ar; ///< @brief Набор блоков ввода для данных

    /**
     * @brief Отображение переданных данных
     * @param [1] Пакет с данными */
    void DataView_v(const MessageT &);

  private slots:
    /** @brief Отправка данных CAN */
    void SendData_slt();

  signals:
    /**
     * @brief Отправка сообщения
     * [1] Сообщение */
    void sendMessage_sig(const MessageT &);
  };
} // namespace law
