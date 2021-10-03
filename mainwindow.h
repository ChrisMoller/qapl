#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QFileSystemWatcher>
#include <QLineEdit>
#include <QMainWindow>
#include <QTemporaryDir>
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
  MainWindow    *mainwin;
  QLineEdit     *watched;
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
  void update_screen (QString &errString, QString &outString);

  QLineEdit *inputLine;
  QTextEdit *outputLog;
  History   *history;

private slots:
  void fileChanged(const QString &path);

private:
  void 		     show_fcn (QString text);
  void 		     edit_fcn (QString text);
  InputLineFilter   *inputLineFilter;
  QString            editor;
  QTemporaryDir      tempdir;
  QFileSystemWatcher watcher;
};
#endif // MAINWINDOW_H
