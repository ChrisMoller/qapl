#ifndef HELPWINDOW_H
#define HELPWINDOW_H

#include <QtWidgets>
#include "mainwindow.h"

class MainWindow;

class HelpWindow : public QMainWindow
{
  Q_OBJECT

public:
  HelpWindow (MainWindow *parent = nullptr);
  ~HelpWindow ();

private:
  MainWindow *mw;
  
protected:
  void closeEvent(QCloseEvent *event) override;
};

#endif // HELPWINDOW_H
