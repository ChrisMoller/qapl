#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QCommandLineParser>
#include <QFileSystemWatcher>
#include <QLineEdit>
#include <QMainWindow>
#include <QSettings>
#include <QTemporaryDir>
#include <QTextEdit>

#include "history.h"
#include "helpwindow.h"

#define toCString(v)       ((v).toStdString ().c_str ())

#define APPLICATION_ORGANISATION   "qapl"
#define APPLICATION_NAME	   "qapl"
#define APPLICATION_VERSION	   "1.0"

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

#if 0
#define DEFAULT_SLICK_EDITOR "/opt/slickedit-pro2020/bin/vs -q"
#endif

#define DEFAULT_FONT_FAMILY "DejaVu Sans Mono"
#define DEFAULT_FONT_SIZE   10.0
#define DEFAULT_BG_COLOUR   "#f4f4d3"	// pale yellow
#define DEFAULT_FG_COLOUR   "#000000"	// black
#define DEFAULT_HEIGHT      512
#define DEFAULT_WIDTH       700

typedef enum {
  SAVE_MODE_NONE,
  SAVE_MODE_SAVE,
  SAVE_MODE_DUMP,
  SAVE_MODE_OUT
} save_mode_e;

class MainWindow;
class HelpWindow;

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
  MainWindow(QCommandLineParser &parser, QWidget *parent = nullptr);
  ~MainWindow();
  void createMenubar ();
  void update_screen (QString &errString, QString &outString);
  void processLine (bool suppressOppressOutput, QString text);
  void readScript (QString pfn, bool &noCONT, bool &noSETUP);
  void readScript (QString pfn);
  QStringList parseCl (QString str);
  void printError (QString emsg, QString estr);
  void printError (QString emsg);
  void closeHW () { HWopen = false; }

  QLineEdit *inputLine;
  QTextEdit *outputLog;
  History   *history;

private slots:
  void fileChanged(const QString &path);
  void byebye ();

private:
  void 		     show_fcn (QString text);
  void 		     edit_fcn (QString text);
  void               wsLoad ();
  bool               wsSave ();
  bool               wsSaveAs ();
  void               setEditor ();
  void               setFont ();
  void               setBGColour ();
  void               setFGColour ();
  void               setColours ();
  void               symbolsHelp ();
  void               aboutHelp ();
  InputLineFilter   *inputLineFilter;
  QString            editor;
  int		     editorIndex;
  QStringList	     extraEditors;
  QTemporaryDir      tempdir;
  QFileSystemWatcher watcher;
  QSettings  	    *settings;
  QColor	     bg_colour;
  QColor	     fg_colour;
  QVector<qint64>    processList;
  QString 	     libpath;
  save_mode_e        save_mode;                                                 
  QString    	     curFile;
  bool		     HWopen;
  
protected:
  void closeEvent(QCloseEvent *event) override;
};
#endif // MAINWINDOW_H
