#include "WReceive.h"

#include <QTableWidgetItem>
#include <QElapsedTimer>

#include "MessageT.h"
#include "ui_WReceive.h"

namespace law
{
  WReceive::WReceive(QWidget *parent)
    : QWidget(parent)
    , _Widget_po(new Ui::WReceive)
    , _Timer_po(new QElapsedTimer)
  {
    _Widget_po->setupUi(this);

    _Widget_po->tableWidget->setColumnWidth(0, 80);
    _Widget_po->tableWidget->setColumnWidth(2, 150);
    _Widget_po->tableWidget->setColumnWidth(3, 80);
    _Widget_po->tableWidget->setColumnWidth(4, 80);

    _Timer_po->start();
  }

  WReceive::~WReceive()
  {
    delete _Widget_po;
    delete _Timer_po;
  }

  void WReceive::DataView_v(const MessageT & message)
  {
    int rowNumber = -1;
    for (int i = 0; i < _Widget_po->tableWidget->rowCount(); ++i) {
      if (_Widget_po->tableWidget->item(i, 0)->text().toUInt(nullptr, 16) == message.id()) {
        rowNumber = i;
        break;
      }
    }
    if (rowNumber == -1) { // Добавим новую строку
      _Periods_map[message.id()];
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
    _Periods_map[message.id()].push_back(_Timer_po->elapsed());
    if (_Periods_map[message.id()].size() > 10) {
      _Periods_map[message.id()].pop_front();
    }
    _Widget_po->tableWidget->item(rowNumber, 1)->setText(QString::number(message.dlc(), 10));
    _Counts_map[message.id()]++;
    QString dataText;
    for (int dc = 0; dc < message.dlc(); ++dc) {
      dataText.append(QString::number(message.data(dc), 16).rightJustified(2, '0') + ' ');
    }
    _Widget_po->tableWidget->item(rowNumber, 2)->setText(dataText);
    qreal period = 0.0;
    if (_Periods_map[message.id()].size() > 1) {
      for(int et = 0; et < _Periods_map[message.id()].size() - 1; ++et) {
        period += (_Periods_map[message.id()].at(et + 1) - _Periods_map[message.id()].at(et));
      }
      period = period / (_Periods_map[message.id()].size() - 1);
      _Widget_po->tableWidget->item(rowNumber, 3)->setText(QString::number(period, 'f', 1));
    }
    _Widget_po->tableWidget->item(rowNumber, 4)->setText(QString::number(_Counts_map[message.id()]));
  }
} // namespace law
