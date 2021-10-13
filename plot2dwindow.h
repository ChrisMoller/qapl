#ifndef PLOT2DWINDOW_H
#define PLOT2DWINDOW_H

#include <QtWidgets>
#include "mainwindow.h"

class MainWindow;

class Plot2DWindow : public QMainWindow
{
  Q_OBJECT

public:
  Plot2DWindow (MainWindow *parent = nullptr);
  ~Plot2DWindow ();

private:
  MainWindow *mw;
  
protected:
  void closeEvent(QCloseEvent *event) override;
};

#endif // PLOT2DWINDOW_H
