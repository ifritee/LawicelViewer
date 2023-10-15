#include "URS232Controller.h"

#include <QDebug>
#include <QTimer>

#include "MainWindow.h"
#include "URS232Abonent.h"
#include "CConstants.h"
#include "MessageT.h"

namespace law
{
  URS232Controller::URS232Controller(MainWindow * window, URS232Abonent * abonent, QObject *parent)
  : QObject{parent}
  , _View_po(window)
  , _Model_po(abonent)
  , _CurrentProcessing_po(nullptr)
  , _MessageBuffer_po(new QByteArray)
  {
    connect(_View_po, &MainWindow::SendNewSettings_sig, this, &URS232Controller::CheckRS232Settings_slt);
    connect(_View_po, &MainWindow::Connect_sig, this, &URS232Controller::ConnectControl_slt);
    connect(_View_po, &MainWindow::SetMode_sig, this, &URS232Controller::SetMode_slt);
    connect(_View_po, &MainWindow::SetSpeed_sig, this, &URS232Controller::SetSpeed_slt);

    connect(_Model_po, &URS232Abonent::ConnectChanged_sig, this, &URS232Controller::ConnectChanged_slt);
    connect(_Model_po, &URS232Abonent::DataReady_sig, this, &URS232Controller::DataReady_slt);
    connect(_View_po, &MainWindow::sendMessage_sig, this, &URS232Controller::SendMessage_slt);
  }

  URS232Controller::~URS232Controller()
  {
    delete _MessageBuffer_po;
  }

  void URS232Controller::RequestVersion_v()
  {
    QByteArray data;
    data.append(static_cast<char>(EC_COMMANDS::CMD_V)).append(CR);
    _Model_po->SendData_v(data);
  }

  void URS232Controller::ProcessingVersion_v()
  {
    qDebug()<<QString(*_MessageBuffer_po);
  }

  void URS232Controller::ProcessingMsgtt()
  {
    MessageT messageT(true);
    messageT.deCoding(*_MessageBuffer_po);
    _View_po->DataView_v(messageT);
  }

  void URS232Controller::ProcessingMsgT()
  {
    MessageT messageT(false);
    messageT.deCoding(*_MessageBuffer_po);
    _View_po->DataView_v(messageT);
  }

  void URS232Controller::ProcessingMsgrr()
  {
    qDebug()<<QString(*_MessageBuffer_po);
  }

  void URS232Controller::ProcessingMsgR()
  {
    qDebug()<<QString(*_MessageBuffer_po);
  }

  void URS232Controller::CheckRS232Settings_slt(const SRS232DATA & data)
  {
    /// @todo Тут должна быть проверка данных
    _Model_po->setSettings_v(data);
  }

  void URS232Controller::ConnectControl_slt(bool isConnect)
  {
    _Model_po->SetConnection_v(isConnect);
  }

  void URS232Controller::ConnectChanged_slt(bool isConnect)
  {
    _View_po->ConnectChanged_slt(isConnect);
    if (isConnect) {
      QTimer::singleShot(100, this, [this](){ RequestVersion_v(); });
      }
  }

  void URS232Controller::SetMode_slt(EC_MODE mode)
  {
    QByteArray data;
    switch (mode) {
      case law::EC_MODE::CLOSE: data.append(static_cast<char>(EC_COMMANDS::CMD_C)).append(CR); break;
      case law::EC_MODE::NORMAL:data.append(static_cast<char>(EC_COMMANDS::CMD_O)).append(CR); break;
      case law::EC_MODE::LOOPBACK: return;
      case law::EC_MODE::LISTEN:data.append(static_cast<char>(EC_COMMANDS::CMD_L)).append(CR); break;
      }

    _Model_po->SendData_v(data);
  }

  void URS232Controller::SetSpeed_slt(int index)
  {
    QByteArray data;
    data.append(static_cast<char>(EC_COMMANDS::CMD_S));
    data.append(static_cast<uint8_t>(index) + 0x30).append(CR);
    _Model_po->SendData_v(data);
  }

  void URS232Controller::DataReady_slt(const QByteArray & data)
  {
    for (int i = 0; i < data.size(); ++i) {
      if (_CurrentProcessing_po == nullptr) {
        _MessageBuffer_po->clear();
          if (data[i] == (char)EC_COMMANDS::CMD_V) { _CurrentProcessing_po = &URS232Controller::ProcessingVersion_v; }
          else if (data[i] == (char)EC_COMMANDS::MSG_t) { _CurrentProcessing_po = &URS232Controller::ProcessingMsgtt; }
          else if (data[i] == (char)EC_COMMANDS::MSG_T) { _CurrentProcessing_po = &URS232Controller::ProcessingMsgT; }
          else if (data[i] == (char)EC_COMMANDS::MSG_r) { _CurrentProcessing_po = &URS232Controller::ProcessingMsgrr; }
          else if (data[i] == (char)EC_COMMANDS::MSG_R) { _CurrentProcessing_po = &URS232Controller::ProcessingMsgR; }
          else if (data[i] == CR) { _View_po->Completed_v(true); } // Успешное выполнение команды
          else if (data[i] == BELL) { _View_po->Completed_v(false); } // Ошибка при выполнении команды
      } else {
        if (data[i] == CR) {
          (this->*_CurrentProcessing_po)();
          _CurrentProcessing_po = nullptr;
        } else if (data[i] == BELL) {
          _CurrentProcessing_po = nullptr;
        } else {
          _MessageBuffer_po->append(data[i]);
        }

      }
    }
  }

  void URS232Controller::SendMessage_slt(const MessageT & message)
  {
    QByteArray array = message.coding();
    _Model_po->SendData_v(array);
  }

} // namespace law
