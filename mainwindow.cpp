#include <QDoubleSpinBox>
#include <QFontComboBox>
#include <QMenuBar>
#include <QPushButton>
#include <QRegularExpression>
#include <QVBoxLayout>
#include <signal.h>

#include "mainwindow.h"
#include "helpwindow.h"
#include "optionstrings.h"
#include "aplexec.h"
#include "history.h"

#include <apl/libapl.h>

static const QColor red = QColor (255, 0, 0);
static const QColor black = QColor (0, 0, 0);

void MainWindow::printError (QString emsg, QString estr)
{
  outputLog->setTextColor (red);
  outputLog->append (emsg);
  if (estr.size () > 0)
    outputLog->append (estr);
  outputLog->setTextColor (fg_colour);
}

void MainWindow::printError (QString emsg)
{
  outputLog->setTextColor (red);
  outputLog->append (emsg);
  outputLog->setTextColor (fg_colour);
}

static void
lineKey (MainWindow  *mainwin, int dir)
{
  char *str = (dir > 0)
    ? mainwin->history->previous ()
    : mainwin->history->next ();
  QString text = QString (str);
  mainwin->inputLine->setText (text);
}

static void
doPage (MainWindow  *mainwin, int dir)
{
  QFont font = mainwin->outputLog->currentFont ();
  // the 12 and 20 are hacks to get the line spacing
  int lines =  (12 * mainwin->outputLog->height ()) /
    (20 * font.pointSize ());
  lines = (lines > 8) ? lines -2 : 1;
  int i;
  if (dir > 0) {
    for (i = 0; i < lines; i++)
      mainwin->outputLog->moveCursor (QTextCursor::Up,
                                   QTextCursor::MoveAnchor);
  }
  else {
    for (i = 0; i < lines; i++)
      mainwin->outputLog->moveCursor (QTextCursor::Down,
                                   QTextCursor::MoveAnchor);
  }
}

void MainWindow::killTempdir ()
{
  bool rc = false;
  for (int i = 0; !rc && i < 5; i++)
    rc = tempdir.remove ();
  if (!rc) fprintf (stderr, "Removing tempdir failed.\n");
}

void MainWindow::byebye ()
{
  for (int i = 0; i < processList.size (); i++) {
    qint64 pid = processList[i];
    kill (pid, SIGTERM);
  }
  killTempdir ();
  QRect geo = this->geometry ();
  settings->setValue (SETTINGS_WIDTH,  geo.width ());
  settings->setValue (SETTINGS_HEIGHT, geo.height ());
  QCoreApplication::quit ();
}

void
MainWindow::closeEvent(QCloseEvent *event __attribute__((unused)))
{
  byebye ();
}


bool
InputLineFilter::eventFilter(QObject *obj, QEvent *event)
{
  if (obj == watched) {
    if (event->type() == QEvent::Wheel) {
      QWheelEvent* wheelEvent = static_cast<QWheelEvent*>(event);
      lineKey (mainwin, wheelEvent->angleDelta ().y ());
    }
    else if (event->type() == QEvent::KeyPress) {
      QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
      if (keyEvent->key() == Qt::Key_PageUp) {
        doPage (mainwin, 1);
        return true;
      }
      else if (keyEvent->key() == Qt::Key_PageDown) {
        doPage (mainwin, -1);
        return true;
      }
      else if (keyEvent->key() == Qt::Key_Up) {
        lineKey (mainwin, 1);
        return true;
      }
      else if (keyEvent->key() == Qt::Key_Down) {
        lineKey (mainwin, -1);
        return true;
      }
      else if (keyEvent->key() == Qt::Key_Tab) {
        QRegularExpression aplsep (APL_VARIABLE);
        aplsep.setPatternOptions (QRegularExpression::CaseInsensitiveOption);
        QString outString;
        QString errString;
        QString cmd = QString (")fns");
        AplExec::aplExec (APL_OP_COMMAND, cmd, outString, errString);
        QString vbls = outString;
        cmd = QString (")vars");
        AplExec::aplExec (APL_OP_COMMAND, cmd, outString, errString);
        vbls.append (outString);
        if (!vbls.isEmpty ()) {
          QString text = mainwin->inputLine->text();
          QRegularExpressionMatchIterator toks = aplsep.globalMatch (text);
          QString tok;
          while (toks.hasNext ()) {
            QRegularExpressionMatch match = toks.next();
            tok = match.captured(1);
          }
          if (!tok.isEmpty ()) {
            int i;
            QStringList fns = vbls.split (QRegularExpression ("\\s+"));
            QStringList possibles = QStringList ();
            for (i = 0; i < fns.size (); i++)
              if (fns[i].startsWith (tok)) possibles.push_back (fns[i]);
            if (0 < possibles.size ()) {
              if (1 == possibles.size ()) {
                text.chop (tok.size ());
                text.append (possibles[0]);
                mainwin->inputLine->setText (text);
              }
              else {
                QDialog dialog (mainwin, Qt::Popup);
                QVBoxLayout *layout = new QVBoxLayout ();
                dialog.setLayout (layout);
                QComboBox *completion_ops =  new QComboBox ();
                completion_ops->setMaxVisibleItems (10);
                completion_ops->showPopup ();
                for (i = 0; i < possibles.size (); i++)
                  completion_ops->addItem (possibles[i]);
                layout->addWidget(completion_ops);
                dialog.exec ();
                QString sel = completion_ops->currentText ();
                text.chop (tok.size ());
                text.append (sel);
                mainwin->inputLine->setText (text);
                delete completion_ops;
                delete layout;
              }
            }
          }
        }
#if 0			// nothing found
        else if (!errString.isEmpty ()) {
          fprintf (stderr, "got fns err\n"); // fixme, use msgbox
        }
#endif
        return true;
      }
    }
  }
  return QObject::eventFilter(obj, event);
}

void
MainWindow::update_screen (QString &errString, QString &outString)
{
  if (!errString.isEmpty ()) {
    outputLog->setTextColor (red);
    outputLog->append (outString);
    outputLog->setTextColor (fg_colour);
  }
  if (!outString.isEmpty ()) outputLog->append (outString);
  QScrollBar *sb = outputLog->verticalScrollBar();
  sb->setValue (sb->maximum ());
}

void MainWindow::inputLineReturn()
{
  QString text = inputLine->text ();
  text = text.trimmed ();
  inputLine->clear ();

  if (text.startsWith (QString ("]help"), Qt::CaseInsensitive)) {
    if (!HWopen) {
      new HelpWindow (this);
      HWopen = true;
    }
    return;
  }

  if (text.startsWith (QString ("∇"))) {
    if (text.endsWith (QString ("∇"))) show_fcn (text);
    else edit_fcn (text);
    return;
  }

  history->insert (toCString (text));
  history->rebase ();
  outputLog->append ("      " + text);

  processLine (false, text);
}

void MainWindow::readScript (QString pfn, bool &noCONT, bool &noSETUP) {
  QFile pfile(pfn);
  if (pfile.open (QIODevice::ReadOnly | QIODevice::Text)) {
#define BUFFER_SIZE 512
    char buffer[BUFFER_SIZE];
    qint64 charsIn;
    while (0 < (charsIn = pfile.readLine(buffer, BUFFER_SIZE))) {
      if (charsIn > 0) {
	QString pb (buffer);
	QString pbt = pb.trimmed ();
	if (pbt.endsWith ("\n")) pbt.chop (1);
	if (!pbt.startsWith ("#")) {
	  bool suppress = true;
#define QAPL_TAG "qapl"
	  if (pbt.startsWith (QAPL_TAG, Qt::CaseInsensitive)) {
	    pbt = pbt.remove (QAPL_TAG, Qt::CaseInsensitive);
	    pbt = pbt.trimmed ();
#define OP_EDITOR_PLUS "editor+"
	    if (pbt.startsWith (OP_EDITOR_PLUS, Qt::CaseInsensitive)) {
	      pbt = pbt.remove (OP_EDITOR_PLUS, Qt::CaseInsensitive);
	      pbt = pbt.trimmed ();
	      extraEditors.append (pbt);
	    }
#define OP_NOCONT "nocont"
	    else if (pbt.startsWith (OP_NOCONT, Qt::CaseInsensitive)) {
	      noCONT = true;
	    }
#define OP_NOSETUP "nosetup"
	    else if (pbt.startsWith (OP_NOSETUP, Qt::CaseInsensitive)) {
	      noSETUP = true;
	    }
	    else {
	      // fixme unknown option
	    }
	    
	  }
	  else {
	    if (pbt.startsWith ("!")) {
	      pbt.remove (0, 1);
	      suppress = false;
	    }
	    processLine (suppress, pbt);
	  }
	}
      }
    }
    pfile.close ();
  }
}

void MainWindow::readScript (QString pfn) {
  bool fake;
  readScript (pfn, fake, fake);
}

MainWindow::MainWindow(QCommandLineParser &parser, QWidget *parent)
  : QMainWindow(parent)
{
  HWopen = false;
  tempdir.setAutoRemove (true);
  QString pfn = QString ("%1/.gnu-apl/preferences").arg (getenv ("HOME"));
  if (!QFile::exists (pfn))
    pfn = QString ("%1/.config/gnu-apl/preferences")
      .arg (getenv ("HOME"));
  QFile pfile(pfn);
  if (pfile.open (QIODevice::ReadOnly | QIODevice::Text)) {
#define BUFFER_SIZE 512
    char buffer[BUFFER_SIZE];
    while (0 < pfile.readLine(buffer, BUFFER_SIZE)) {
      if (strcasestr (buffer, "LIBREF-0")) {
        char path[BUFFER_SIZE];
        if (0 < sscanf (buffer, " %*s %s \n", path))
          libpath = QString (path);
      }
    }
    pfile.close ();
  }

  {		// override if env vbl exists
    char *lp = getenv ("APL_LIB_ROOT");
    if (lp)
      libpath = QString (lp) + "/workspaces";
  }
  

  static QSettings lsettings;
  settings = &lsettings;

  editor = settings->value (SETTINGS_EDITOR).toString ();
  if (editor.isEmpty ()) {
    editor = QString (DEFAULT_EDITOR);
    settings->setValue (SETTINGS_EDITOR, QVariant (editor));
  }
  editorIndex = -1;

  QString fontFamily = settings->value (SETTINGS_FONT_FAMILY).toString ();
  if (fontFamily.isEmpty ()) {
    fontFamily = QString (DEFAULT_FONT_FAMILY);
    settings->setValue (SETTINGS_FONT_FAMILY, QVariant (fontFamily));
  }

  qreal fontSize = DEFAULT_FONT_SIZE;
  QVariant fontSizeVar = settings->value (SETTINGS_FONT_SIZE);
  if (fontSizeVar.isValid ()) fontSize = fontSizeVar.toReal ();
  else settings->setValue (SETTINGS_FONT_SIZE, fontSize);

  int height = DEFAULT_HEIGHT;
  QVariant heightVar = settings->value (SETTINGS_HEIGHT);
  if (heightVar.isValid ()) height = heightVar.toInt ();
  else settings->setValue (SETTINGS_HEIGHT, height);
  
  int width = DEFAULT_WIDTH;
  QVariant widthVar = settings->value (SETTINGS_WIDTH);
  if (widthVar.isValid ()) width = widthVar.toInt ();
  else settings->setValue (SETTINGS_WIDTH, width);
  
  QString foregroundString = settings->value (SETTINGS_FG_COLOUR).toString ();
  if (foregroundString.isEmpty ()) {
    foregroundString = QString (DEFAULT_FG_COLOUR);
    settings->setValue (SETTINGS_FG_COLOUR, QVariant (foregroundString));
  }
  fg_colour = QColor (foregroundString);
  
  QString backgroundString = settings->value (SETTINGS_BG_COLOUR).toString ();
  if (backgroundString.isEmpty ()) {
    backgroundString = QString (DEFAULT_BG_COLOUR);
    settings->setValue (SETTINGS_BG_COLOUR, QVariant (backgroundString));
  }
  bg_colour = QColor (backgroundString);  
  
  history = new History ();

  connect(&watcher,
          &QFileSystemWatcher::fileChanged,
          this, &MainWindow::fileChanged);
  this->resize (width, height);

  QWidget *mainWidget = new QWidget();
  setCentralWidget(mainWidget);

  QVBoxLayout *layout = new QVBoxLayout;

  createMenubar ();

  outputLog = new QTextEdit;
  QFont outputFont (fontFamily, fontSize);
  outputLog->setFont (outputFont);
#if 0
  QFont font = outputLog->property("font").value<QFont>();
  fprintf (stderr, "font %s %g\n",
	   toCString (font.family ()),
	   (double)font.pointSize ());
#endif
  QPalette p = outputLog->palette(); 
  p.setColor(QPalette::Base, bg_colour);
  p.setColor(QPalette::Text, fg_colour); 
  outputLog->setPalette(p);
  outputLog->setReadOnly (true);
  layout->addWidget(outputLog);

  inputLine = new QLineEdit;
  inputLine->setFont (outputFont);
  p = inputLine->palette(); 
  p.setColor(QPalette::Base, bg_colour);
  p.setColor(QPalette::Text, fg_colour); 
  inputLine->setPalette(p);
  inputLine->setPlaceholderText ("APL");
  inputLineFilter = new InputLineFilter (inputLine, this);
  inputLine->setEnabled (true);
  inputLine->installEventFilter(inputLineFilter);
  layout->addWidget(inputLine);
  QObject::connect(inputLine,
		   SIGNAL(returnPressed()),
		   this,
		   SLOT(inputLineReturn()));

  mainWidget->setLayout(layout);

  bool noCONT  = false;
  bool noSETUP = false;
  if (!parser.isSet (OPT_noINIT)) {
    QString pfn = QString ("./%1init.txt").arg (APPLICATION_NAME);
    if (!QFile::exists (pfn))
      pfn = QString ("%1/%2init.txt")
	.arg (getenv ("HOME"), APPLICATION_NAME);
    if (QFile::exists (pfn))
      readScript (pfn, noCONT, noSETUP);
  }

  bool contDone = false;
  if (!parser.isSet (OPT_noCONT) && !noCONT) {
    QString cf = QString ("%1/CONTINUE.xml").arg (libpath);
    if (QFile::exists (cf)) {
      processLine (false, ")load CONTINUE");
      contDone = true;
    }
  }
  
  if (!parser.isSet (OPT_noSETUP) && !noSETUP && !contDone) {
    QString cf = QString ("%1/SETUP.xml").arg (libpath);
    if (QFile::exists (cf)) {
      processLine (false, ")load SETUP");
    }
  }
  
  if (parser.isSet (OPT_L)) {
    QString cmd = QString (")load %1").arg(parser.value (OPT_L));
    processLine (false, cmd);
  }

  QStringList pargs = parser.positionalArguments();
  if (0 < pargs.size ()) {
    for (int i = 0; i <  pargs.size (); i++)
      readScript (pargs[i]);
  }
}

MainWindow::~MainWindow()
{
  delete history;
  killTempdir ();
}

