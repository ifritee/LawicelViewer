#pragma once

#include <QWidget>
#include <QMap>
#include <QQueue>

namespace Ui {
  class WReceive;
}

class QElapsedTimer;

namespace law
{
  class MessageT;

  /**
   * @class WReceive
   * @brief Виджет отображения принимаемых данных */
  class WReceive : public QWidget
  {
    Q_OBJECT

  public:
    explicit WReceive(QWidget *parent = nullptr);
    virtual ~WReceive();
    /**
     * @brief Отображение переданных данных
     * @param [1] Пакет с данными */
    void DataView_v(const MessageT &);

  private:
    Ui::WReceive *_Widget_po; ///< @brief Интерфейс пользователя

    QMap<uint32_t, QQueue<qint64> > _Periods_map; ///< @brief Сохраненные периоды
    QMap<uint32_t, quint32 > _Counts_map; ///< @brief Счетчик
    QElapsedTimer * _Timer_po; ///< @brief Замеры времени
  };

} // namespace law
