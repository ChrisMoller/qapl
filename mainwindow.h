#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QLineEdit>
#include <QMainWindow>
#include <QTextEdit>

#include "history.h"

#define toCString(v)       ((v).toStdString ().c_str ())

class MainWindow;

class InputLineFilter : public QObject
{
    Q_OBJECT

public:
  InputLineFilter (QLineEdit *obj, MainWindow *mw)
  {watched = obj; mainwin = mw;}

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

private:
  MainWindow *mainwin;
  QLineEdit *watched;
};

class MainWindow : public QMainWindow
{
  Q_OBJECT

public slots:
  void inputLineReturn ();

public:
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow();
  void createMenubar ();

  QLineEdit *inputLine;
  QTextEdit *outputLog;
  History   *history;

private:
  InputLineFilter *inputLineFilter;
};
#endif // MAINWINDOW_H
