#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QFileSystemWatcher>
#include <QLineEdit>
#include <QMainWindow>
#include <QSettings>
#include <QTemporaryDir>
#include <QTextEdit>

#include "history.h"

#define toCString(v)       ((v).toStdString ().c_str ())

#define SETTINGS_ORGANISATION	   "qapl"
#define SETTINGS_EDITOR            "Editor"
#define SETTINGS_FONT_FAMILY       "FontFamily"
#define SETTINGS_FONT_SIZE         "FontSize"
#define SETTINGS_HEIGHT            "Height"
#define SETTINGS_WIDTH             "Width"
#define SETTINGS_BG_COLOUR         "BG_Colour"
#define SETTINGS_FG_COLOUR         "FG_COLOUR"

#define DEFAULT_EDITOR \
   "emacs --geometry=40x20  -background '#ffffcc' -font \"DejaVu Sans Mono-10\""

#define DEFAULT_GVIM_EDITOR \
   "gvim -geometry=40x20  -background '#ffffcc' -font \"DejaVu Sans Mono-10\""

#define DEFAULT_FONT_FAMILY "DejaVu Sans Mono"
#define DEFAULT_FONT_SIZE   10.0
#define DEFAULT_BG_COLOUR   "#f4f4d3"	// pale yellow
#define DEFAULT_FG_COLOUR   "#000000"	// black
#define DEFAULT_HEIGHT      512
#define DEFAULT_WIDTH       640


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
  void processLine (bool suppressOppressOutput, QString text);

  QLineEdit *inputLine;
  QTextEdit *outputLog;
  History   *history;

private slots:
  void fileChanged(const QString &path);
  void byebye ();

private:
  void 		     show_fcn (QString text);
  void 		     edit_fcn (QString text);
  void               setEditor ();
  void               setFont ();
  void               setBGColour ();
  void               setFGColour ();
  void               setColours ();
  InputLineFilter   *inputLineFilter;
  QString            editor;
  QTemporaryDir      tempdir;
  QFileSystemWatcher watcher;
  QSettings  	    *settings;
  QColor	     bg_colour;
  QColor	     fg_colour;
  
protected:
  void closeEvent(QCloseEvent *event) override;
};
#endif // MAINWINDOW_H
