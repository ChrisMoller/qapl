#ifndef PLOT2DWINDOW_H
#define PLOT2DWINDOW_H

#include <QtWidgets>
#include "mainwindow.h"
#include "complexspinbox.h"

class MainWindow;

class Plot2DWindow : public QMainWindow
{
  Q_OBJECT

public:
  Plot2DWindow (MainWindow *parent = nullptr);
  ~Plot2DWindow ();
  void drawCurve ();
  void createMenubar ();
  void setResolution ();

private:
  MainWindow *mw;
#if 0
  QLineEdit *indexVarName;
#endif
  QLineEdit *aplExpression;
  int resolution;
  double realInit;
  double realFinal;
  double imagInit;
  double imagFinal;
  bool setupComplete;
  
protected:
  void closeEvent(QCloseEvent *event) override;
};

#endif // PLOT2DWINDOW_H
