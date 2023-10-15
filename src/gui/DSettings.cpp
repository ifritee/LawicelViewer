#include "DSettings.h"

#include <QSerialPort>
#include <QSerialPortInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QStandardPaths>
#include <QFile>
#include <QTimer>
#include <QDebug>

#include "ui_DSettings.h"

namespace law
{

  static const char blankString[] = QT_TRANSLATE_NOOP("SettingsDialog", "N/A");

  DSettings::DSettings(QWidget *parent)
  : QDialog(parent)
  , _Widget_po(new Ui::DSettings)
  {
    _Widget_po->setupUi(this);

    //----- Устанавливаем скорость передачи -----
    _Widget_po->baudRate_cb->addItem(QStringLiteral("9600"), QSerialPort::Baud9600);
    _Widget_po->baudRate_cb->addItem(QStringLiteral("19200"), QSerialPort::Baud19200);
    _Widget_po->baudRate_cb->addItem(QStringLiteral("38400"), QSerialPort::Baud38400);
    _Widget_po->baudRate_cb->addItem(QStringLiteral("57600"), QSerialPort::Baud57600);
    _Widget_po->baudRate_cb->addItem(QStringLiteral("115200"), QSerialPort::Baud115200);
    _Widget_po->baudRate_cb->setCurrentIndex(4);


    //----- Устанавливаем биты данных -----
    _Widget_po->dataBits_cb->addItem(QStringLiteral("5"), QSerialPort::Data5);
    _Widget_po->dataBits_cb->addItem(QStringLiteral("6"), QSerialPort::Data6);
    _Widget_po->dataBits_cb->addItem(QStringLiteral("7"), QSerialPort::Data7);
    _Widget_po->dataBits_cb->addItem(QStringLiteral("8"), QSerialPort::Data8);
    _Widget_po->dataBits_cb->setCurrentIndex(3);

    //----- Установить бит четности -----
    _Widget_po->parity_cb->addItem(tr("None"), QSerialPort::NoParity);
    _Widget_po->parity_cb->addItem(tr("Even"), QSerialPort::EvenParity);
    _Widget_po->parity_cb->addItem(tr("Odd"), QSerialPort::OddParity);
    _Widget_po->parity_cb->addItem(tr("Mark"), QSerialPort::MarkParity);
    _Widget_po->parity_cb->addItem(tr("Space"), QSerialPort::SpaceParity);

    //----- Установить стоп-бит -----
    _Widget_po->stopBit_cb->addItem(QStringLiteral("1"), QSerialPort::OneStop);
    _Widget_po->stopBit_cb->addItem(QStringLiteral("2"), QSerialPort::TwoStop);

    //----- Добавить управление потоком -----
    _Widget_po->flowBit_cb->addItem(tr("None"), QSerialPort::NoFlowControl);
    _Widget_po->flowBit_cb->addItem(tr("RTS/CTS"), QSerialPort::HardwareControl);
    _Widget_po->flowBit_cb->addItem(tr("XON/XOFF"), QSerialPort::SoftwareControl);

    connect(_Widget_po->port_cb, &QComboBox::textActivated, this, &DSettings::setCurrentPort_slt);
    connect(_Widget_po->buttonBox, &QDialogButtonBox::accepted, this, &DSettings::save_slt);
    connect(_Widget_po->buttonBox, &QDialogButtonBox::rejected, this, &DSettings::reject);

    //----- Загрузка данных из файла -----

    QTimer::singleShot(500, this, [this](){
      load();
      emit accepted();
    });
  }

  DSettings::~DSettings()
  {
    delete _Widget_po;
  }

  const SRS232DATA & DSettings::SaveRS232Data_cr() const
  {
    return _SaveRS232Data_o;
  }

  void DSettings::showEvent(QShowEvent * event)
  {
    QDialog::showEvent(event);
    _Widget_po->port_cb->clear();
    // Получить доступные последовательные порты
    QList<QSerialPortInfo> serialPortInfos = QSerialPortInfo::availablePorts();
    QString description;
    QString manufacturer;
    QString serialNumber;
    for (const QSerialPortInfo &serialPortInfo : serialPortInfos) {
      QStringList list;
      description = serialPortInfo.description();
      manufacturer = serialPortInfo.manufacturer();
      serialNumber = serialPortInfo.serialNumber();

      list << serialPortInfo.portName()
           << (!description.isEmpty() ? description : blankString)
           << (!manufacturer.isEmpty() ? manufacturer : blankString)
           << (!serialNumber.isEmpty() ? serialNumber : blankString)
           << serialPortInfo.systemLocation()
           << (serialPortInfo.vendorIdentifier() ? QString::number(serialPortInfo.vendorIdentifier(), 16) : blankString)
           << (serialPortInfo.productIdentifier() ? QString::number(serialPortInfo.productIdentifier(), 16) : blankString);

      _Widget_po->port_cb->addItem(list.first(), list);
    }
    _Widget_po->port_cb->setCurrentText(_SaveRS232Data_o.portName);
    if (_Widget_po->port_cb->currentText().isEmpty()) {
      _Widget_po->baudRate_cb->setEnabled(false);
      _Widget_po->dataBits_cb->setEnabled(false);
      _Widget_po->parity_cb->setEnabled(false);
      _Widget_po->stopBit_cb->setEnabled(false);
      _Widget_po->flowBit_cb->setEnabled(false);
      _SaveRS232Data_o.portName = "";
    } else {
      _Widget_po->baudRate_cb->setEnabled(true);
      _Widget_po->dataBits_cb->setEnabled(true);
      _Widget_po->parity_cb->setEnabled(true);
      _Widget_po->stopBit_cb->setEnabled(true);
      _Widget_po->flowBit_cb->setEnabled(true);
      _Widget_po->baudRate_cb->setCurrentText(QString::number(_SaveRS232Data_o.bountRate));
      _Widget_po->dataBits_cb->setCurrentText(QString::number(_SaveRS232Data_o.dataBits));
      _Widget_po->parity_cb->setCurrentText(QString::number(_SaveRS232Data_o.parity));
      _Widget_po->stopBit_cb->setCurrentText(QString::number(_SaveRS232Data_o.stopBits));
      _Widget_po->flowBit_cb->setCurrentText(QString::number(_SaveRS232Data_o.flowControl));
    }
  }

  bool DSettings::load()
  {
    QString path = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/" + SETTINGS_FILENAME;
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
      qWarning()<<tr("No settings file to load!");
      return false;
    }
    QByteArray data = file.readAll();
    QJsonDocument jsonDocument = QJsonDocument::fromJson(data);
    QJsonObject mainObject = jsonDocument.object();
    if (mainObject.contains(TAG_PORTNAME)) { _SaveRS232Data_o.portName        = mainObject[TAG_PORTNAME].toString(); }
    if (mainObject.contains(TAG_BAUDRATE)) { _SaveRS232Data_o.bountRate       = mainObject[TAG_BAUDRATE].toInt(); }
    if (mainObject.contains(TAG_DATABITS)) { _SaveRS232Data_o.dataBits        = mainObject[TAG_DATABITS].toInt(); }
    if (mainObject.contains(TAG_PARITY)) { _SaveRS232Data_o.parity            = mainObject[TAG_PARITY].toInt(); }
    if (mainObject.contains(TAG_STOPBITS)) { _SaveRS232Data_o.stopBits        = mainObject[TAG_STOPBITS].toInt(); }
    if (mainObject.contains(TAG_FLOWCONTROL)) { _SaveRS232Data_o.flowControl  = mainObject[TAG_FLOWCONTROL].toInt(); }

    file.close();
    return true;
  }

  void DSettings::save_slt()
  {
    setCurrentPort_slt(_Widget_po->port_cb->currentText());
    QString path = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/" + SETTINGS_FILENAME;
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly)) {
      qWarning()<<tr("No settings file to load!");
      return;
    }
    QJsonObject mainObject;
    mainObject[TAG_PORTNAME]    = _Widget_po->port_cb->currentText();
    mainObject[TAG_BAUDRATE]    = _Widget_po->baudRate_cb->currentText().toInt();
    mainObject[TAG_DATABITS]    = _Widget_po->dataBits_cb->currentText().toInt();
    mainObject[TAG_PARITY]      = _Widget_po->parity_cb->currentText().toInt();
    mainObject[TAG_STOPBITS]    = _Widget_po->stopBit_cb->currentText().toInt();
    mainObject[TAG_FLOWCONTROL] = _Widget_po->flowBit_cb->currentText().toInt();

    QJsonDocument jsonDocument(mainObject);
    file.write(jsonDocument.toJson());
    file.close();
    accept();
  }

  void DSettings::setCurrentPort_slt(const QString & port)
  {
    if (!_Widget_po->port_cb->currentText().isEmpty()) {
      _Widget_po->baudRate_cb->setEnabled(true);
      _Widget_po->dataBits_cb->setEnabled(true);
      _Widget_po->parity_cb->setEnabled(true);
      _Widget_po->stopBit_cb->setEnabled(true);
      _Widget_po->flowBit_cb->setEnabled(true);
      _SaveRS232Data_o.portName = port;
      _SaveRS232Data_o.bountRate = _Widget_po->baudRate_cb->currentData().toInt();
      _SaveRS232Data_o.dataBits = _Widget_po->dataBits_cb->currentData().toInt();
      _SaveRS232Data_o.parity = _Widget_po->parity_cb->currentData().toInt();
      _SaveRS232Data_o.stopBits = _Widget_po->stopBit_cb->currentData().toInt();
      _SaveRS232Data_o.flowControl = _Widget_po->flowBit_cb->currentData().toInt();
    }
  }

} // namespace law
