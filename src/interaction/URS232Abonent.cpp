#include "URS232Abonent.h"

#include <QSerialPort>
#include <QThread>
#include <QDebug>

#include "CConstants.h"

namespace law
{

  URS232Abonent::URS232Abonent(QObject *parent)
  : QObject{parent}
  , _DevicePort_po(new QSerialPort(this))
  , _IsConnected_b(false)
  , _IsInitialize_b(false)
  {

  }

  URS232Abonent::~URS232Abonent()
  {
    delete _DevicePort_po;
  }

  void URS232Abonent::setSettings_v(const SRS232DATA & data)
  {
    if(_IsConnected_b) {
      _DevicePort_po->close();
      _IsConnected_b = false;
      emit ConnectChanged_sig(_IsConnected_b);
    }
    _DevicePort_po->setPortName(data.portName);
    _DevicePort_po->setBaudRate(data.bountRate);
    _DevicePort_po->setDataBits(static_cast<QSerialPort::DataBits>(data.dataBits));
    _DevicePort_po->setParity(static_cast<QSerialPort::Parity>(data.parity));
    _DevicePort_po->setStopBits(static_cast<QSerialPort::StopBits>(data.stopBits));
    _DevicePort_po->setFlowControl(static_cast<QSerialPort::FlowControl>(data.flowControl));
    _IsInitialize_b = true;
  }

  void URS232Abonent::SetConnection_v(bool isConnect)
  {
    if (_IsInitialize_b) {
      if (isConnect) {
        if (_DevicePort_po->open(QIODevice::ReadWrite)) {
          _IsConnected_b = true;
          connect(_DevicePort_po, &QSerialPort::readyRead, this, &URS232Abonent::ReadData_slt);
          QThread::msleep(2000); // Время на открытие порта
          emit ConnectChanged_sig(_IsConnected_b);
        } else {
          qWarning()<<_DevicePort_po->errorString();
        }
      } else {
        _DevicePort_po->close();
        _IsConnected_b = false;
        emit ConnectChanged_sig(_IsConnected_b);
      }
    } else {
      qWarning()<<tr("Connection RS-232 is not initilized!");
      emit Error_sig(tr("Connection RS-232 is not initilized!"));
    }
  }
  void URS232Abonent::SendData_v(const QByteArray & data)
  {
    if (_IsConnected_b) { // Только если есть связь
      _DevicePort_po->write(data);
    } else {
      qWarning()<<tr("RS-232 is not connected!");
      emit Error_sig(tr("RS-232 is not connected!"));
    }
  }

  void URS232Abonent::ReadData_slt()
  {
    QByteArray buffer = _DevicePort_po->readAll();
    if (!buffer.isEmpty()) {
      emit DataReady_sig(buffer);
    }
  }

} // namespace law
