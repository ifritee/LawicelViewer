#include "MessageT.h"

#include <QString>
#include <QDebug>

#include "CConstants.h"

namespace law
{
  MessageT::MessageT(bool isShortID)
  : ID_LENGTH(isShortID ? 3 : 8)
  , _IsShortID_b(isShortID)
  , _ID_u32(0)
  , _DLC_u8(0)
  {
    _Data_ar.fill(0);
  }

  MessageT::~MessageT()
  {

  }

  QByteArray MessageT::coding() const
  {
    QByteArray array;
    array.append(_IsShortID_b ? 't' : 'T');
    QString id = QString::number(_ID_u32, 16).rightJustified(ID_LENGTH, '0');;
    array.append(id.toLatin1(), ID_LENGTH);
    array.append(_DLC_u8 + 0x30);
    char buffer[2];
    for (int i = 0; i < _DLC_u8; ++i) {
      byteToHexChar(_Data_ar[i], buffer);
      array.append(buffer, 2);
    }
    return array;
  }

  void MessageT::deCoding(const QByteArray & data)
  {
    if (data.size() >= _IsShortID_b + 1) {
      _ID_u32 = data.left(ID_LENGTH).toUInt(nullptr, 16);
      _DLC_u8 = data[ID_LENGTH] - 0x30;
      if (data.size() == ID_LENGTH + 1 + (_DLC_u8 * 2)) {
        for (int i = 0; i < _DLC_u8; ++i) {
          _Data_ar[i] = data.mid(ID_LENGTH + 1 + i*2, 2).toUInt(nullptr, 16);
        }
      }
    }

  }

  uint32_t MessageT::id() const
  {
    return _ID_u32;
  }

  uint8_t MessageT::dlc() const
  {
    return _DLC_u8;
  }

  uint8_t MessageT::data(uint8_t i) const
  {
    if (i >= _Data_ar.size()) {
      qCritical()<<QObject::tr("Data index out of range!");
      return 0;
    }
    return _Data_ar[i];
  }

  void MessageT::setID(uint32_t id)
  {
    _ID_u32 = id;
  }

  void MessageT::setDLC(uint8_t dlc)
  {
    _DLC_u8 = dlc;
  }

  void MessageT::setData(int index, uint8_t dataByte)
  {
    if (index >= 0 && index < static_cast<int>(_Data_ar.size())) {
      _Data_ar[index] = dataByte;
    }
  }

} //namespace law
