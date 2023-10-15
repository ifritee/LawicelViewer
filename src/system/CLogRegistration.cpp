#include "CLogRegistration.h"

#include <QDebug>
#include <QFile>
#include <QDir>
#include <QScopedPointer>
#include <QTextStream>
#include <QDateTime>

namespace ows {

  QScopedPointer<QFile> LoggingFile_go; ///< @brief Умный указатель на файл логирования
  const int LogIndexMax_ci = 10; ///< @brief Максимальное количество сохраняемых логов

  /** @brief Объявляение обработчика логгера */
  void MessageOutput_v(QtMsgType type, const QMessageLogContext &context, const QString &msg);
  /** Выбор файла для записи лога */
  bool SetLoggerData_b(int argc, char * argv []);

  void MessageOutput_v(QtMsgType type, const QMessageLogContext &context, const QString &msg)
  {
      // Открываем поток записи в файл
      QTextStream OStream_o(LoggingFile_go.data());
      const char * FileName_pcc = context.file ? context.file : "";
      const char * FunctionName_pcc = context.function ? context.function : "";
      // Записываем дату записи
      QString DateTime_Str = QDateTime::currentDateTime().toString("[yyyy-MM-dd hh:mm:ss.zzz] ");
      switch (type) {
      case QtDebugMsg:
        OStream_o << "      DD >> "<<DateTime_Str;
        break;
      case QtInfoMsg:
        OStream_o << "      II >> "<<DateTime_Str;
        break;
      case QtWarningMsg:
        OStream_o << "    WW >> "<<DateTime_Str;
        break;
      case QtCriticalMsg:
        OStream_o << "  EE >> "<<DateTime_Str << " "<< FileName_pcc<<":"<< context.line <<"->"<< FunctionName_pcc;
        break;
      case QtFatalMsg:
        OStream_o << "FF >> "<<DateTime_Str << " "<< FileName_pcc<<":"<<context.line <<"->"<< FunctionName_pcc;
        break;
      }

      OStream_o << ": " << msg << Qt::endl; // Записываем в вывод категорию сообщения и само сообщение
      OStream_o.flush();    // Очищаем буферизированные данные
  }

  bool SetLoggerData_b(int argc, char * argv [])
  {
    bool IsOk_b = false;
    for (int i = 1; i < argc; ++i) {
      if ((QString(argv[i]) == "--log" && (i + 1) < argc) || (QString(argv[i]) == "-l" && (i + 1) < argc)) {
        QString FileName_str = argv[i + 1];
        //----- Циклическое добавление индекса -----
        for(int i = LogIndexMax_ci - 1; i > 0; i--) {
          QString TempFileName_str = FileName_str + "." + QString::number(i);
          QString NewFileName_str = FileName_str + "." + QString::number(i + 1);
          QFile::remove(NewFileName_str);
          if(QFile::exists(TempFileName_str)) {
            QFile::copy(TempFileName_str, NewFileName_str);
          }
        }
        QFile::remove(FileName_str + ".1");
        if (QFile::exists(FileName_str)) {
          QFile::copy(FileName_str, FileName_str + ".1");
          QFile::remove(FileName_str);
        }
        LoggingFile_go.reset(new QFile(FileName_str));// Устанавливаем файл логирования,
        LoggingFile_go.data()->open(QFile::Append | QFile::Text);// Открываем файл логирования
        IsOk_b = true;
        break;
      }
    }
    if (!IsOk_b) { // Если файл не был создан, то записываем в поток вывода
      auto * output = new QFile;
      output->open(stdout, QFile::WriteOnly);
      if (output->isOpen()) {
        LoggingFile_go.reset(output);
        IsOk_b = true;
      }
    }
    return IsOk_b;
  }

  void cLogRegistration(int argc, char *argv[])
  {
    if (SetLoggerData_b(argc, argv) == true ) {
      qInstallMessageHandler(MessageOutput_v);
    }
  }

} // namespace ows


