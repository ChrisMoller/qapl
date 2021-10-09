#ifndef HELPWINDOW_H
#define HELPWINDOW_H

#include <QtWidgets>
#include "mainwindow.h"
#include "helpwindow.h"

class HelpWindow : public QMainWindow
{
  Q_OBJECT

public:
  HelpWindow (MainWindow *parent=nullptr);
  ~HelpWindow ();
};

#endif // HELPWINDOW_H
