#include <QDoubleSpinBox>
#include <QFontComboBox>
#include <QLabel>
#include <QMenuBar>
#include <QPushButton>
#include <QRegularExpression>
#include <QVBoxLayout>

#include "mainwindow.h"
#include "aplexec.h"
#include "history.h"

#include <apl/libapl.h>

#if 0
#define LAMBDA_HEADER "lambda_"
#define EDITOR \
   "emacs --geometry=40x20  -background '#ffffcc' -font \"DejaVu Sans Mono-10\""
#endif
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

void MainWindow::byebye ()
{
  QRect geo = this->geometry ();
  settings.setValue (SETTINGS_WIDTH,  geo.width ());
  settings.setValue (SETTINGS_HEIGHT, geo.height ());
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

void MainWindow::inputLineReturn()
{
  QString outString;
  QString errString;
  QString text = inputLine->text ();
  text = text.trimmed ();
  inputLine->clear ();

  if (text.startsWith (QString ("∇"))) {
    if (text.endsWith (QString ("∇"))) show_fcn (text);
    else edit_fcn (text);
    return;
  }

  outputLog->append (text);
  history->insert (toCString (text));
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


void
MainWindow::setEditor ()
{
  QDialog dialog (this, Qt::Dialog);
  QGridLayout *layout = new QGridLayout;
  dialog.setLayout (layout);

  int row = 0;
  
  QLineEdit *editorLine = new QLineEdit (editor);
  layout->addWidget (editorLine, row, 0);

  row++;

  QPushButton *defaultEditor = new QPushButton (tr ("Use default editor"));
  connect (defaultEditor,
           &QAbstractButton::clicked,
           [=](){
	     editorLine->setText (QString (DEFAULT_EDITOR));
	   });  
  layout->addWidget (defaultEditor, row, 0, 1, 2);

  row++;

  QPushButton *defaultGvimEditor
    = new QPushButton (tr ("Use default gvim editor"));
  connect (defaultGvimEditor,
           &QAbstractButton::clicked,
           [=](){
	     editorLine->setText (QString (DEFAULT_GVIM_EDITOR));
	   });  
  layout->addWidget (defaultGvimEditor, row, 0, 1, 2);

  row++;  
  
  QPushButton *closeButton = new QPushButton (QObject::tr ("Accept"));
  closeButton->setAutoDefault (true);
  closeButton->setDefault (true);
  layout->addWidget (closeButton, row, 1);
  QObject::connect (closeButton, &QPushButton::clicked,
                    &dialog, &QDialog::accept);
  QPushButton *cancelButton = new QPushButton (QObject::tr ("Cancel"));
  layout->addWidget (cancelButton, row, 0);
  QObject::connect (cancelButton, &QPushButton::clicked,
                    &dialog, &QDialog::reject);

  if (QDialog::Accepted == dialog.exec ()) {
    editor = editorLine->text ();
    settings.setValue (SETTINGS_EDITOR, QVariant (editor));
  }
}

void
MainWindow::setFont ()
{
  QDialog dialog (this, Qt::Dialog);
  QGridLayout *layout = new QGridLayout;
  dialog.setLayout (layout);
  
  QFont font = outputLog->property("font").value<QFont>();

  int row = 0;
  
  QFontComboBox *fontCombo = new QFontComboBox();
  fontCombo->setCurrentFont (font);
  fontCombo->setFontFilters (QFontComboBox::MonospacedFonts);
  layout->addWidget (fontCombo, row, 0);

  QDoubleSpinBox *fontSize = new QDoubleSpinBox ();
  fontSize->setDecimals (1);
  fontSize->setRange (2.0, 24.0);
  fontSize->setValue ((double)font.pointSize ());
  layout->addWidget (fontSize, row, 1);

  row++;

  QPushButton *defaultFont = new QPushButton (tr ("Use default font"));
  connect (defaultFont,
           &QAbstractButton::clicked,
           [=](){
	     fontSize->setValue (DEFAULT_FONT_SIZE);
	     fontCombo->setCurrentFont (QString (DEFAULT_FONT_FAMILY));
	   });  
  layout->addWidget (defaultFont, row, 0, 1, 2);

  row++;
  
  QPushButton *closeButton = new QPushButton (QObject::tr ("Accept"));
  closeButton->setAutoDefault (true);
  closeButton->setDefault (true);
  layout->addWidget (closeButton, row, 1);
  QObject::connect (closeButton, &QPushButton::clicked,
                    &dialog, &QDialog::accept);
  QPushButton *cancelButton = new QPushButton (QObject::tr ("Cancel"));
  layout->addWidget (cancelButton, row, 0);
  QObject::connect (cancelButton, &QPushButton::clicked,
                    &dialog, &QDialog::reject);

  if (QDialog::Accepted == dialog.exec ()) {
    QFont newFont = fontCombo->currentFont ();
    double ps = fontSize->value ();
    QFont outputFont (newFont.family (), ps);
    outputLog->setFont (outputFont);
    settings.setValue (SETTINGS_FONT_FAMILY, QVariant (newFont.family ()));
    settings.setValue (SETTINGS_FONT_SIZE, QVariant (ps));
  }
}

void  MainWindow::setFGColour ()
{
  QColorDialog *colourDialog = new QColorDialog (fg_colour);
  colourDialog->setCustomColor (0, QColor (DEFAULT_BG_COLOUR));
  colourDialog->setCustomColor (1, QColor (DEFAULT_FG_COLOUR));
  colourDialog->setWindowTitle (tr ("Select foreground colour"));
  if (QDialog::Accepted == colourDialog->exec ()) {
    fg_colour = colourDialog->selectedColor ();
    QPalette p = outputLog->palette(); 
    p.setColor(QPalette::Text, fg_colour);
    outputLog->setPalette(p);
    settings.setValue (SETTINGS_FG_COLOUR, QVariant (fg_colour.name ()));
  }
}

void  MainWindow::setBGColour ()
{
  QColorDialog *colourDialog = new QColorDialog (bg_colour);
  colourDialog->setCustomColor (0, QColor (DEFAULT_BG_COLOUR));
  colourDialog->setCustomColor (1, QColor (DEFAULT_FG_COLOUR));
  colourDialog->setWindowTitle (tr ("Select background colour"));
  if (QDialog::Accepted == colourDialog->exec ()) {
    bg_colour = colourDialog->selectedColor ();
    QPalette p = outputLog->palette(); 
    p.setColor(QPalette::Base, bg_colour);
    outputLog->setPalette(p);
    settings.setValue (SETTINGS_BG_COLOUR, QVariant (bg_colour.name ()));
  }
}

void
MainWindow::setColours ()
{
  QDialog dialog (this, Qt::Dialog);
  QGridLayout *layout = new QGridLayout;
  dialog.setLayout (layout);

  int row = 0;
  
  QPushButton *setFG = new QPushButton (tr ("Set foreground colour"));
  connect (setFG,
           &QAbstractButton::clicked,
           [=](){ setFGColour (); });
  layout->addWidget (setFG, row, 0);

  row++;

  QPushButton *setBG = new QPushButton (tr ("Set background colour"));
  connect (setBG,
           &QAbstractButton::clicked,
           [=](){ setBGColour (); });
  layout->addWidget (setBG, row, 0);

  row++;

  QPushButton *defaultColours = new QPushButton (tr ("Set default colours"));
  connect (defaultColours,
           &QAbstractButton::clicked,
           [=](){
	     QPalette p = outputLog->palette();
	     bg_colour = QColor (DEFAULT_BG_COLOUR);
	     fg_colour = QColor (DEFAULT_FG_COLOUR);
	     p.setColor(QPalette::Base, bg_colour);
	     p.setColor(QPalette::Text, fg_colour);
	     outputLog->setPalette(p);
	     settings.setValue (SETTINGS_BG_COLOUR,
				QVariant (bg_colour.name ()));
	     settings.setValue (SETTINGS_FG_COLOUR,
				QVariant (fg_colour.name ()));
	   });
  layout->addWidget (defaultColours, row, 0);

  row++;

  QPushButton *closeButton = new QPushButton (QObject::tr ("Close"));
  closeButton->setAutoDefault (true);
  closeButton->setDefault (true);
  layout->addWidget (closeButton, row, 1);
  QObject::connect (closeButton, &QPushButton::clicked,
                    &dialog, &QDialog::accept);
  
  dialog.exec ();
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


  QMenu *settingsMenu = menuBar()->addMenu(tr("&Settings"));

  QAction *editorAct =
    settingsMenu->addAction(tr("&Editor"), this, &MainWindow::setEditor);
  editorAct->setStatusTip(tr("Set editor"));
  
  QAction *fontAct =
    settingsMenu->addAction(tr("&Font"), this, &MainWindow::setFont);
  fontAct->setStatusTip(tr("Set font"));

#if 1
  QAction *coloursAct =
    settingsMenu->addAction(tr("&Colours"), this, &MainWindow::setColours);
  coloursAct->setStatusTip(tr("Set colours"));
#else
  QAction *FGcolourAct =
    settingsMenu->addAction(tr("&Foreground Colour"), this,
			    &MainWindow::setFGColour);
  FGcolourAct->setStatusTip(tr("Set foreground colour"));
  
  QAction *BGcolourAct =
    settingsMenu->addAction(tr("&Background Colour"), this,
			    &MainWindow::setBGColour);
  BGcolourAct->setStatusTip(tr("Set background colour"));
#endif
}

MainWindow::MainWindow(QWidget *parent)
  : QMainWindow(parent)
{
  editor = settings.value (SETTINGS_EDITOR).toString ();
  if (editor.isEmpty ()) {
    editor = QString (DEFAULT_EDITOR);
    settings.setValue (SETTINGS_EDITOR, QVariant (editor));
  }

  QString fontFamily = settings.value (SETTINGS_FONT_FAMILY).toString ();
  if (fontFamily.isEmpty ()) {
    fontFamily = QString (DEFAULT_FONT_FAMILY);
    settings.setValue (SETTINGS_FONT_FAMILY, QVariant (fontFamily));
  }

  qreal fontSize = DEFAULT_FONT_SIZE;
  QVariant fontSizeVar = settings.value (SETTINGS_FONT_SIZE);
  if (fontSizeVar.isValid ()) fontSize = fontSizeVar.toReal ();
  else settings.setValue (SETTINGS_FONT_SIZE, fontSize);

  //  QApplication::setFont (font);
  
  int height = DEFAULT_HEIGHT;
  QVariant heightVar = settings.value (SETTINGS_HEIGHT);
  if (heightVar.isValid ()) height = heightVar.toInt ();
  else settings.setValue (SETTINGS_HEIGHT, height);
  
  int width = DEFAULT_WIDTH;
  QVariant widthVar = settings.value (SETTINGS_WIDTH);
  if (widthVar.isValid ()) width = widthVar.toInt ();
  else settings.setValue (SETTINGS_WIDTH, width);
  
  QString foregroundString = settings.value (SETTINGS_FG_COLOUR).toString ();
  if (foregroundString.isEmpty ()) {
    foregroundString = QString (DEFAULT_FG_COLOUR);
    settings.setValue (SETTINGS_FG_COLOUR, QVariant (foregroundString));
  }
  fg_colour = QColor (foregroundString);
  
  QString backgroundString = settings.value (SETTINGS_BG_COLOUR).toString ();
  if (backgroundString.isEmpty ()) {
    backgroundString = QString (DEFAULT_BG_COLOUR);
    settings.setValue (SETTINGS_BG_COLOUR, QVariant (backgroundString));
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

