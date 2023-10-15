#include "MainWindow.h"

#include <QIcon>
#include <QToolButton>
#include <QActionGroup>
#include <QTimer>
#include <QComboBox>

#include "DSettings.h"
#include "ui_MainWindow.h"

namespace law
{
  MainWindow::MainWindow(QWidget *parent)
  : QMainWindow(parent)
  , _Widget_po(new Ui::MainWindow)
  , _DSettings_po(new DSettings(this))
  , _ModeActions_po(new QActionGroup(this))
  , _ConnectionTimer_po(new QTimer(this))
  , _CANSpeed_po(new QComboBox(this))
  , _CurrentMode_en(EC_MODE::CLOSE)
  , _SetMode_en(EC_MODE::CLOSE)
  {
    _Widget_po->setupUi(this);
    //----- Открыть окно с настройками -----
    connect(_Widget_po->actionPreferences, &QAction::triggered, _DSettings_po, &DSettings::exec);
    connect(_DSettings_po, &DSettings::accepted, [this](){ emit SendNewSettings_sig(_DSettings_po->SaveRS232Data_cr());});
    //----- Соединение с устройством -----
    _OpenAction_po = _Widget_po->toolBar->addAction(QIcon(":/icons/disconnect.png"), "Connect");
    _OpenAction_po->setCheckable(true);
    connect(_OpenAction_po, &QAction::triggered, this, &MainWindow::Connect_slt);
    _Widget_po->toolBar->addSeparator();
    //----- Режимы работы -----
    _ModeActionsDict_map[EC_MODE::CLOSE] = _ModeActions_po->addAction(_Widget_po->toolBar->addAction(QIcon(":/icons/mode_settings.png"), "Mode: Set configuration"));
    _ModeActionsDict_map[EC_MODE::NORMAL] = _ModeActions_po->addAction(_Widget_po->toolBar->addAction(QIcon(":/icons/mode_normal.png"), "Mode: Normal"));
    _ModeActionsDict_map[EC_MODE::LOOPBACK] = _ModeActions_po->addAction(_Widget_po->toolBar->addAction(QIcon(":/icons/mode_loopback.png"), "Mode: Loopback"));
    _ModeActionsDict_map[EC_MODE::LISTEN] = _ModeActions_po->addAction(_Widget_po->toolBar->addAction(QIcon(":/icons/mode_listen.png"), "Mode: Listen"));
    for(QAction * mode : _ModeActions_po->actions()) {
      mode->setCheckable(true);
    }
    _ModeActions_po->setExclusive(true);
    _ModeActions_po->setEnabled(false);
    _ModeActionsDict_map[EC_MODE::CLOSE]->setChecked(true);
    connect(_ModeActions_po, &QActionGroup::triggered, [this](QAction * action){
      QToolButton * toolButton = dynamic_cast<QToolButton*>(_Widget_po->toolBar->widgetForAction(_RunningAction_po));
      if (toolButton) { // Если есть на тулбаре
        toolButton->setStyleSheet("");
      }
      _SetMode_en = _ModeActionsDict_map.key(action);
      emit SetMode_sig(_SetMode_en);
      _RunningAction_po = action;
    });
    connect(_Widget_po->DataTransmission_wdt, &WTransmit::sendMessage_sig, this, &MainWindow::sendMessage_sig);
    //----- Добавим управление скоростью -----
    _CANSpeed_po->addItem("10 K");
    _CANSpeed_po->addItem("20 K");
    _CANSpeed_po->addItem("50 K");
    _CANSpeed_po->addItem("100 K");
    _CANSpeed_po->addItem("125 K");
    _CANSpeed_po->addItem("250 K");
    _CANSpeed_po->addItem("500 K");
    _CANSpeed_po->addItem("500 K");
    _CANSpeed_po->addItem("1 M");
    _CANSpeed_po->setCurrentIndex(3);
    _Widget_po->toolBar->addSeparator();
    _Widget_po->toolBar->addWidget(_CANSpeed_po);
    _CANSpeed_po->setEnabled(false);
    connect(_CANSpeed_po, QOverload<int>::of(&QComboBox::activated), [this](int index){
      emit SetSpeed_sig(index);
    });
  }

  MainWindow::~MainWindow()
  {
    delete _Widget_po;
    delete _DSettings_po;
    delete _ModeActions_po;
    delete _ConnectionTimer_po;
    delete _CANSpeed_po;
  }

  void MainWindow::Completed_v(bool isSuccessfully)
  {
    QToolButton * toolButton = dynamic_cast<QToolButton*>(_Widget_po->toolBar->widgetForAction(_RunningAction_po));
    if (toolButton) { // Если есть на тулбаре
      toolButton->setStyleSheet(isSuccessfully ? "background: green;" : "background: red;");
      if (isSuccessfully) {
        _CurrentMode_en = _SetMode_en;
         _CANSpeed_po->setEnabled(_CurrentMode_en == EC_MODE::CLOSE);
      }
    }
  }

  void MainWindow::DataView_v(const MessageT & message)
  {
    _Widget_po->Receive_wdt->DataView_v(message);
  }

  void MainWindow::ConnectChanged_slt(bool isConnected)
  {
    QToolButton * toolButton = dynamic_cast<QToolButton*>(_Widget_po->toolBar->widgetForAction(_OpenAction_po));
    if (toolButton) {
      toolButton->setStyleSheet(isConnected ? "background: green;" : "");
      QToolButton * runningToolButton = dynamic_cast<QToolButton*>(_Widget_po->toolBar->widgetForAction(_RunningAction_po));
      if (runningToolButton) { // Если есть на тулбаре
        runningToolButton->setStyleSheet("");
      }
      _ModeActions_po->setEnabled(isConnected);
      _ModeActionsDict_map[EC_MODE::CLOSE]->setChecked(true);
      _CANSpeed_po->setEnabled(true);

      _ConnectionTimer_po->stop();
    }
  }

  void MainWindow::Connect_slt(bool isChecked)
  {
    QAction * openAction = dynamic_cast<QAction *>(sender());
    if (openAction) {
      openAction->setIcon(isChecked ? QIcon(":/icons/connect.png") : QIcon(":/icons/disconnect.png"));
      emit Connect_sig(isChecked);
      if (isChecked) {
        _ConnectionTimer_po->singleShot(2000, this, [this, openAction]() {
            if(!_ModeActions_po->isEnabled()) {
              emit openAction->triggered(false);
              openAction->setChecked(false);
            }
        });
      }
    }
  }

} // namespace law


