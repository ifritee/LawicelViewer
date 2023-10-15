#include <QApplication>
#include <QDebug>
#include <stdexcept>

#include "MainWindow.h"
#include "URS232Abonent.h"
#include "URS232Controller.h"
#include "CLogRegistration.h"

int main(int argc, char *argv[])
{
  ows::cLogRegistration(argc, argv);
  QApplication a(argc, argv);
  int exitCode = 0;
  try {
    law::MainWindow mainWindow;
    law::URS232Abonent rs232Abonent;
    law::URS232Controller rs232Controller(&mainWindow, &rs232Abonent);

    mainWindow.show();
    exitCode = a.exec();
  } catch (std::exception & e) {
    qDebug()<<e.what();
    exitCode = 1;
  }
  return exitCode;
}
