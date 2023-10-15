#pragma once

#include <QDialog>

#include "CConstants.h"

namespace Ui {
  class DSettings;
}

namespace law
{
  /**
   * @class DSettings
   * @brief Диалог настроек работы ПО */
  class DSettings : public QDialog
  {
    Q_OBJECT

  public:
    explicit DSettings(QWidget *parent = nullptr);
    virtual ~DSettings();
    /**
     * @brief SaveRS232Data Возвращает установленные данные RS-232
     * @return Набор параметров RS-232 */
    const SRS232DATA & SaveRS232Data_cr() const;

  protected:
    /**
     * @brief showEvent Действия при отображении диалога
     * @param event Данные события отображения диалога */
    virtual void showEvent(QShowEvent * event);

  private:
    Ui::DSettings * _Widget_po; ///< @brief Интерфейс пользователя
    SRS232DATA _SaveRS232Data_o; ///< @brief Сохраненные данные RS-232
    /**
     * @brief Загрузка настроек из файла
     * @return true - загрузка успешна, false - ошибка*/
    bool load();

  private slots:
    /** @brief Сохранение данных */
    void save_slt();
    /** @brief Назначение текущего порта */
    void setCurrentPort_slt(const QString &);
  };

} // namespace law
