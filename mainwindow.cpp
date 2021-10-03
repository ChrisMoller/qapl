#include <QMenuBar>
#include <QRegularExpression>
#include <QVBoxLayout>

#include "mainwindow.h"
#include "aplexec.h"
#include "history.h"

#include <apl/libapl.h>

#define APL_VARIABLE "([⍙∆a-z][⍙∆_a-z0-9]*)"
#define LAMBDA_HEADER "lambda_"
#define EDITOR "emacs"
  // "emacs --geometry=40x20  -background '#ffffcc' -font 'DejaVu Sans Mono-10'"
static const QColor red = QColor (255, 0, 0);
static const QColor black = QColor (0, 0, 0);

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
        else if (!errString.isEmpty ()) {
          fprintf (stderr, "got fns err\n"); // fixme, use msgbox
        }
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
    outputLog->setTextColor (black);
  }
  if (!outString.isEmpty ()) outputLog->append (outString);
  QScrollBar *sb = outputLog->verticalScrollBar();
  sb->setValue (sb->maximum ());
}

/*******  WARNING:  TINKER WITH THESE ONLY IF YOU'RE A MASOCHIST!!  *******/
#define CLINE_RE "(([[:alpha:]][[:alnum:]]*)\\s*)"
      // 0 = whole thing
      // 1 = cmd with trailing spaces
      // 2 = cmd

#define ARG_RE "(((-[[:alpha:]][[:alnum:]]*)\\s+\
(\"([^\"]*)\"|([[:alnum:]]*))?\\s*)|\
(\"([^\"]*)\")\\s*)"
      // 0 = whole thing
      // 1 = whole thing
      // 2 = option flag
      // 3 = option val
      // 4 = quoted val
      // 5 = unquoted val
      // 6 = +cmd

static const QRegularExpression cre (CLINE_RE,
                               QRegularExpression::CaseInsensitiveOption);
static const QRegularExpression are (ARG_RE,
                               QRegularExpression::CaseInsensitiveOption);

void MainWindow::inputLineReturn()
{
  QString outString;
  QString errString;
  QString text = inputLine->text ();
  text = text.trimmed ();
  outputLog->append (text);
  history->insert (toCString (text));
  inputLine->clear ();

  if (text.startsWith (QString ("∇")) &&
      !text.endsWith (QString ("∇"))) {         // function
    bool isLambda = text.startsWith (QString ("∇∇"));
    bool isNew = true;
    text = text.remove (0, (isLambda ? 2 : 1)).trimmed ();

    QString cmd = QString ("⎕cr '%1'").arg(text);
    QStringList list;
    AplExec::aplExec (APL_OP_EXEC, cmd, outString, errString);
    if (!outString.isEmpty ()) {
      list = outString.split (QChar ('\n'));
      isLambda |= list.first ().contains (QString ("λ"));
      isNew = false;
    }
    QString fn;
    if (isLambda) {
      if (isNew || ((list.size () == 3) && (list[2].isEmpty ())))
        fn = QString ("%1/%2%3.apl").
          arg (tempdir.path ()).arg (LAMBDA_HEADER).arg (text);
      else if ((list.size () != 0) && (list.size () != 2)){
        QMessageBox msgBox (QMessageBox::Warning,
                            QString ("Invalid lambda"),
    QString ("A lambda must consist of exactly one expession on one line."));
        msgBox.exec();
        // fixme invalid lambda
        return;
      }
    }
    else
        fn = QString ("%1/%2.apl").arg (tempdir.path ()).arg (text);
    QFile file (fn);
    if  (file.open (QIODevice::WriteOnly | QIODevice::Text)) {
      QTextStream out(&file);
      if (outString.isEmpty ()) {
        if (!isLambda)
          out << text;                  // boilerplate fcn header
      }
      else {
        if (isLambda && !list.isEmpty ()) {
          QString fn = list[1];
          fn = fn.trimmed ();
          fn.remove (0, 2);
          out << fn;
        }
        else if (!outString.isEmpty ())
          out << outString;
      }
      file.close ();

      QStringList args;
      QString real_ed;
      {
        QString editor_copy
	  = editor.isEmpty () ? QString (EDITOR) : editor;
        QRegularExpressionMatch match = cre.match (editor_copy);
        if (match.hasMatch ()) {
          QStringList matches = match.capturedTexts ();
          int offset = match.capturedLength (0);
          editor_copy.remove (0, offset);
          real_ed = matches[2];
          while (!editor_copy.isEmpty ()) {
            match = are.match (editor_copy);
            if (match.hasMatch ()) {
              matches = match.capturedTexts ();
              offset = match.capturedLength (0);
              editor_copy.remove (0, offset);
              if (!matches[2].isEmpty ()) {     // -opt version
                args << matches[3];
                args << ((matches[5].isEmpty ()) ? matches[6] : matches[5]);
              }
              else {                            // +opt version
                args << matches[8];
              }
            }
          }
        }
      }
      args << fn;

      QProcess *edit = new QProcess ();
      watcher.addPath (fn);
      edit->start (real_ed, args);
    }
    else {
      // fixme file open error
    }
    return;
  }
  else if (text.startsWith (QString ("∇")) &&
           text.endsWith (QString ("∇"))) {
    outputLog->append ("      " + text);
    QRegularExpression aplsep (APL_VARIABLE);
    aplsep.setPatternOptions (QRegularExpression::CaseInsensitiveOption);
    QRegularExpressionMatchIterator toks = aplsep.globalMatch (text);
    if (toks.hasNext ()) {
      QRegularExpressionMatch match = toks.next();
      QString tok = match.captured(1);
      QString cmd = QString ("⎕cr '%1'").arg(tok);
      AplExec::aplExec (APL_OP_EXEC, cmd, outString, errString);
      update_screen (errString, outString);
    }
    return;
  }

  outputLog->append ("      " + text);






  
  LIBAPL_error rc = AplExec::aplExec (APL_OP_EXEC, text,outString, errString);
  
  if (rc != LAE_NO_ERROR) {
    QString emsg =
      QString ("APL error %1").arg ((int)rc, 8, 16, QLatin1Char('0'));
    outputLog->setTextColor (red);
    outputLog->append (emsg);
    if (errString.size () > 0)
      outputLog->append (errString);
    outputLog->setTextColor (black);
  }

  if (outString.size () > 0)
    outputLog->append (outString);
  outputLog->moveCursor (QTextCursor::EndOfBlock, QTextCursor::MoveAnchor);
  outputLog->ensureCursorVisible();
  QScrollBar *sb = outputLog->verticalScrollBar();
  sb->setValue (sb->maximum ());

  history->rebase ();
}

void MainWindow::createMenubar ()
{
  QMenu *fileMenu = menuBar()->addMenu(tr("&File"));

  fileMenu->addSeparator();

  const QIcon exitIcon =
    QIcon::fromTheme("application-exit",
                     QIcon(":/images/application-exit.png"));
  QAction *exitAct =
    fileMenu->addAction(exitIcon, tr("E&xit"), this, &QWidget::close);
  exitAct->setShortcuts(QKeySequence::Quit);
}

MainWindow::MainWindow(QWidget *parent)
  : QMainWindow(parent)
{
  QWidget *mainWidget = new QWidget();
  setCentralWidget(mainWidget);

  QVBoxLayout *layout = new QVBoxLayout;

  createMenubar ();

  outputLog = new QTextEdit;
  outputLog->setReadOnly (true);
  layout->addWidget(outputLog);

  inputLine = new QLineEdit;
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
}

MainWindow::~MainWindow()
{
}

