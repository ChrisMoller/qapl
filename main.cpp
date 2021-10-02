#include <apl/libapl.h>

#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
  init_libapl ("apl", 0);

  QApplication a(argc, argv);
  MainWindow w;
  w.show();
  return a.exec();
}
