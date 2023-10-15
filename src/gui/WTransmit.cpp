#include "WTransmit.h"

#include <QTimer>

#include "MessageT.h"
#include "ui_WTransmit.h"

namespace law
{
  WTransmit::WTransmit(QWidget *parent)
  : QWidget(parent)
  , _Widget_po(new Ui::WTransmit)
  , _TxTimer_po(new QTimer)
  {
    _Widget_po->setupUi(this);

    _Widget_po->tableWidget->setColumnWidth(0, 80);
    _Widget_po->tableWidget->setColumnWidth(2, 150);
    _Widget_po->tableWidget->setColumnWidth(3, 80);
    _Widget_po->tableWidget->setColumnWidth(4, 80);

    _Datas_ar[0] = _Widget_po->data1_le;
    _Datas_ar[1] = _Widget_po->data2_le;
    _Datas_ar[2] = _Widget_po->data3_le;
    _Datas_ar[3] = _Widget_po->data4_le;
    _Datas_ar[4] = _Widget_po->data5_le;
    _Datas_ar[5] = _Widget_po->data6_le;
    _Datas_ar[6] = _Widget_po->data7_le;
    _Datas_ar[7] = _Widget_po->data8_le;


    connect(_TxTimer_po, &QTimer::timeout, this, &WTransmit::SendData_slt);
    connect(_Widget_po->Single_pb, &QPushButton::clicked, this, &WTransmit::SendData_slt);
  }

  WTransmit::~WTransmit()
  {
    delete _Widget_po;
    delete _TxTimer_po;
  }

  void WTransmit::SendData_slt()
  {
    if (!_Widget_po->rtr_chb->isChecked()) { // Если полное сообщение
      MessageT message(!_Widget_po->bits29_chb->isChecked());
      message.setID(_Widget_po->id_le->text().toUInt(nullptr, 16));
      message.setDLC(_Widget_po->dlc_le->text().toUInt());
      for (int i = 0; i < message.dlc(); ++i) {
        message.setData(i, _Datas_ar[i]->text().toUInt(nullptr, 16));
      }
      DataView_v(message);
      emit sendMessage_sig(message);
    }
  }

  void WTransmit::DataView_v(const MessageT & message)
  {
    int rowNumber = -1;
    for (int i = 0; i < _Widget_po->tableWidget->rowCount(); ++i) {
      if (_Widget_po->tableWidget->item(i, 0)->text().toUInt(nullptr, 16) == message.id()) {
        rowNumber = i;
        break;
      }
    }
    if (rowNumber == -1) { // Добавим новую строку
      _Counts_map[message.id()] = 0;
      rowNumber = _Widget_po->tableWidget->rowCount();
      _Widget_po->tableWidget->setRowCount(rowNumber + 1);
      for (int j = 0; j < _Widget_po->tableWidget->columnCount(); ++j) {
        QTableWidgetItem * item = new QTableWidgetItem;
        QFont fontMono(font());
        fontMono.setStyleHint(QFont::Monospace);
        item->setFont(fontMono);
        _Widget_po->tableWidget->setItem(rowNumber, j, item);
      }
      _Widget_po->tableWidget->item(rowNumber, 0)->setText(QString::number(message.id(), 16));
    }
    _Widget_po->tableWidget->item(rowNumber, 1)->setText(QString::number(message.dlc(), 10));
    _Counts_map[message.id()]++;
    QString dataText;
    for (int dc = 0; dc < message.dlc(); ++dc) {
      dataText.append(QString::number(message.data(dc), 16).rightJustified(2, '0') + ' ');
    }
    _Widget_po->tableWidget->item(rowNumber, 2)->setText(dataText);

    _Widget_po->tableWidget->item(rowNumber, 4)->setText(QString::number(_Counts_map[message.id()]));
  }

} // namespace law
