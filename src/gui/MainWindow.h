#pragma once

#include <QMainWindow>
#include <QMap>

#include "CConstants.h"

QT_BEGIN_NAMESPACE
    namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class QComboBox;
class QActionGroup;
class QTimer;

namespace law
{

  struct SRS232DATA;
  class DSettings;
  class MessageT;

  /**
   * @class MainWindow
   * @brief Класс основного окна интерфейса пользователя */
  class MainWindow : public QMainWindow
  {
    Q_OBJECT

  public:
    /**
     * @brief MainWindow Конструктор основной
     * @param parent Родительский объект */
    MainWindow(QWidget *parent = nullptr);
    /** @brief Деструктор */
    virtual ~MainWindow();
    /**
     * @brief Completed_v Команда выполнена
     * @param [isSuccessfully] флаг успешного выполнения */
    void Completed_v(bool isSuccessfully);
    /**
     * @brief Отображение переданных данных
     * @param [1] Пакет с данными */
    void DataView_v(const MessageT &);

  private:
    Ui::MainWindow *_Widget_po; ///< @brief Интерфейс пользователя
    DSettings * _DSettings_po; ///< @brief Диалог настроек

    QAction * _OpenAction_po; ///< @brief Открытие устройства RS-232
    QActionGroup * _ModeActions_po; ///< @brief Набор режимов работы
    QMap<EC_MODE, QAction* > _ModeActionsDict_map; ///< @brief Словарь связи между QAction и EC_MODE
    QTimer * _ConnectionTimer_po; ///< @brief Таймер ожидания соединения
    QAction * _RunningAction_po = nullptr; ///< @brief Текущий выполняемый QAction
    QComboBox * _CANSpeed_po; ///< @brief Выбор скорости CAN
    EC_MODE _CurrentMode_en; ///< @brief Текущий режим работы
    EC_MODE _SetMode_en; ///< @brief Устанавливаемый режим работы

  public slots:
    /**
     * @brief Изменение состояния соединения
     * @param [1] true - соединение установлено, false - соединение разорвано */
    void ConnectChanged_slt(bool);

  private slots:
    /** @brief Соединение с устройством */
    void Connect_slt(bool);

  signals:
    /**
     * @brief Передает новые настройки
     * @param [1] Комплект (структура) настроек */
    void SendNewSettings_sig(const SRS232DATA &);
    /**
     * @brief Сигнал об изменении режима работы
     * @param [1] true - соединиться, false - отключиться */
    void Connect_sig(bool);
    /**
     * @brief Установка нового режима работы
     * @param [1] Номер режима работы */
    void SetMode_sig(EC_MODE);
    /** @brief Отправка сообщения */
    void sendMessage_sig(const MessageT &);
    /**
     * @brief Установка новой скорости работы */
    void SetSpeed_sig(int);
  };

} // namespace law
