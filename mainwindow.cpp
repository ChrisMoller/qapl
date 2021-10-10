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
  outputLog->setTextColor (black);
}

void MainWindow::printError (QString emsg)
{
  outputLog->setTextColor (red);
  outputLog->append (emsg);
  outputLog->setTextColor (black);
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

void MainWindow::byebye ()
{
  for (int i = 0; i < processList.size (); i++) {
    qint64 pid = processList[i];
    kill (pid, SIGTERM);
  }
  tempdir.remove ();			// just in case
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
    outputLog->setTextColor (black);
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
    /*HelpWindow *hw = new */HelpWindow ();
    //    hw->show ();
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

void
MainWindow::aboutHelp ()
{
  QMessageBox msgBox;
  msgBox.setText("By:  C. H. L. M&oslash;ller<br>"
		 "moller@mollerware.com");
  msgBox.setTextFormat (Qt::RichText);
  msgBox.setIcon (QMessageBox::Information);
  msgBox.exec();
}

void
MainWindow::symbolsHelp ()
{
  /*HelpWindow *hw =*/ new  HelpWindow ();
  // hw->show ();
}

void
MainWindow::wsLoad ()
{
   /***
      xml )load )copy
      atf  )in
      
      apl not work
      xml works
      atf works but no msg
      
   ***/

  static bool protect = false;
  static bool do_load = true;
  QString filter = QString ("*.xml *.atf");
  QFileDialog dialog (this, QString ("Open APL file"), libpath, filter);
  dialog.setOption (QFileDialog::DontUseNativeDialog);
  QLayout *layout = dialog.layout ();

  QGroupBox *gbox = new QGroupBox ();
  QHBoxLayout *btnlayout = new QHBoxLayout ();
  gbox->setLayout (btnlayout);
  QCheckBox *button_protected  = new QCheckBox ("Protected", this);
  QRadioButton *button_load    = new QRadioButton ("Load", this);
  QRadioButton *button_copy    = new QRadioButton ("Copy", this);
  button_load->setChecked (do_load);
  btnlayout->addWidget (button_protected);
  btnlayout->addWidget (button_load);
  btnlayout->addWidget (button_copy);
  layout->addWidget (gbox);

  dialog.setWindowModality(Qt::WindowModal);
  dialog.setAcceptMode(QFileDialog::AcceptOpen);
  if (dialog.exec() == QDialog::Accepted) {
    do_load = button_load->isChecked();
    protect =
      (button_protected->checkState() == Qt::Checked) ? true : false;
    QString fn = dialog.selectedFiles().first();
    if (fn.endsWith (QString (".xml"),Qt::CaseInsensitive)) {
      if (do_load && protect) {
        QMessageBox msgBox;
  msgBox.setText("Loaded workspaces cannot be protected.  Use )copy instead..");
        msgBox.setIcon (QMessageBox::Warning);
        msgBox.exec();
      }
      else {
        QString op =
          do_load
          ? QString (")load")
          : (protect ? QString (")pcopy") : QString (")copy"));
        QString cmd = QString ("%1 %2").arg (op).arg (fn);
	processLine (false, cmd);
      }
    }
    else if (fn.endsWith (QString (".atf"),Qt::CaseInsensitive)) {
      QString op =
        protect ? QString (")pin") : QString (")in");
      QString cmd = QString ("%1 %2").arg(op).arg (fn);
	processLine (false, cmd);
    }
    else {
      QMessageBox msgBox;
      msgBox.setText("File type not supported.");
      msgBox.setIcon (QMessageBox::Warning);
      msgBox.exec();
    }
  }
  delete gbox;
}

bool
MainWindow::wsSave()
{
/***
    )save and )dump need wsid or argument
    )out always needs argument
 ***/
  bool rc = false;
  QString op;
  switch(save_mode) {
  case SAVE_MODE_NONE:
    break;
  case SAVE_MODE_SAVE:
    op = QString (")save");
    break;
  case SAVE_MODE_DUMP:
    op = QString (")save");
    break;
  case SAVE_MODE_OUT:
    break;
  }
  if (!op.isEmpty ()) {
    processLine (false, op);
    rc = true;
  }
  return rc;
}

bool
MainWindow::wsSaveAs()
{
  bool rc = false;
  QFileDialog dialog(this);
  dialog.setOption (QFileDialog::DontUseNativeDialog);
  QLayout *layout = dialog.layout ();
  QGroupBox *gbox = new QGroupBox ("Save mode");
  QHBoxLayout *btnlayout = new QHBoxLayout ();
  gbox->setLayout (btnlayout);
  QRadioButton *button_save = new QRadioButton("Save", this);
  QRadioButton *button_dump = new QRadioButton("Dump", this);
  QRadioButton *button_out  = new QRadioButton("Out", this);
  btnlayout->addWidget (button_save);
  btnlayout->addWidget (button_dump);
  btnlayout->addWidget (button_out);
  switch (save_mode) {
  case  SAVE_MODE_NONE:
    break;
  case  SAVE_MODE_SAVE:
    button_save->setChecked (true);
    break;
  case  SAVE_MODE_DUMP:
    button_dump->setChecked (true);
    break;
  case  SAVE_MODE_OUT:
    button_out->setChecked (true);
    break;
  }
  layout->addWidget (gbox);
  dialog.setWindowModality(Qt::WindowModal);
  dialog.setAcceptMode(QFileDialog::AcceptSave);
  int drc = dialog.exec();
  delete gbox;
  if (drc == QDialog::Accepted) {
    QString op;
    curFile = dialog.selectedFiles().first();
    if (button_save->isChecked ()) {
      if (!curFile.endsWith (".xml", Qt::CaseInsensitive))
        curFile.append (".xml");
      save_mode = SAVE_MODE_SAVE;
      op = QString (")save");
    }
    else if (button_dump->isChecked ()) {
      save_mode = SAVE_MODE_DUMP;
      if (!curFile.endsWith (".apl", Qt::CaseInsensitive))
        curFile.append (".apl");
      op = QString (")dump");
    }
    else if (button_out->isChecked ()) {
      save_mode = SAVE_MODE_OUT;
      if (!curFile.endsWith (".atf", Qt::CaseInsensitive))
        curFile.append (".atf");
      op = QString (")out");
    }
    if (!op.isEmpty ()) {
      QString cmd = QString ("%1 %2").arg (op).arg (curFile);
      processLine (false, cmd);
      rc = true;
    }
  }
  return rc;
}

void
MainWindow::setEditor ()
{
  QDialog dialog (this, Qt::Dialog);
  QGridLayout *layout = new QGridLayout;
  dialog.setLayout (layout);

  int row = 0;

  QLabel *currentLbl = new QLabel (tr ("Current selection:"));
  layout->addWidget (currentLbl, row, 0);

  row++;
  
  QLineEdit *editorLine = new QLineEdit (editor);
  layout->addWidget (editorLine, row, 0, 1, 2);

  row++;

  QLabel *predefLbl = new QLabel (tr ("Pre-defined editors:"));
  layout->addWidget (predefLbl, row, 0);

  row++;

  QComboBox *editorCombo = new QComboBox ();
  editorCombo->addItem (QString (DEFAULT_EDITOR));
  editorCombo->addItem (QString (DEFAULT_GVIM_EDITOR));
  if (!extraEditors.isEmpty ()) {
    for (int i = 0; i < extraEditors.size (); i++)
      editorCombo->addItem (extraEditors[i]);
  }
  if (editorIndex != -1) editorCombo->setCurrentIndex(editorIndex);
  connect(editorCombo,
	  QOverload<int>::of(&QComboBox::activated),
	  [=](int index){
	    editorIndex = index;
	    editorLine->setText (editorCombo->currentText ());
	  });
  layout->addWidget (editorCombo, row, 0, 1, 2);
  
  row++;

#if 0
  QPushButton *defaultSlickEditor
    = new QPushButton (tr ("Use default Slick editor"));
  connect (defaultSlickEditor,
           &QAbstractButton::clicked,
           [=](){
	     editorLine->setText (QString (DEFAULT_SLICK_EDITOR));
	   });  
  layout->addWidget (defaultSlickEditor, row, 0, 1, 2);
#endif

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
    settings->setValue (SETTINGS_EDITOR, QVariant (editor));
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
    inputLine->setFont (outputFont);
    settings->setValue (SETTINGS_FONT_FAMILY, QVariant (newFont.family ()));
    settings->setValue (SETTINGS_FONT_SIZE, QVariant (ps));
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
    
    p = inputLine->palette(); 
    p.setColor(QPalette::Text, fg_colour);
    inputLine->setPalette(p);

    settings->setValue (SETTINGS_FG_COLOUR, QVariant (fg_colour.name ()));
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
    
    p = inputLine->palette(); 
    p.setColor(QPalette::Base, bg_colour);
    inputLine->setPalette(p);

    settings->setValue (SETTINGS_BG_COLOUR, QVariant (bg_colour.name ()));
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
	     bg_colour = QColor (DEFAULT_BG_COLOUR);
	     fg_colour = QColor (DEFAULT_FG_COLOUR);

	     QPalette p = outputLog->palette();
	     p.setColor(QPalette::Base, bg_colour);
	     p.setColor(QPalette::Text, fg_colour);
	     outputLog->setPalette(p);

	     p = inputLine->palette();
	     p.setColor(QPalette::Base, bg_colour);
	     p.setColor(QPalette::Text, fg_colour);
	     inputLine->setPalette(p);

	     settings->setValue (SETTINGS_BG_COLOUR,
				 QVariant (bg_colour.name ()));
	     settings->setValue (SETTINGS_FG_COLOUR,
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

void MainWindow::read_script (QString pfn) {
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

void MainWindow::createMenubar ()
{
  QMenu *fileMenu = menuBar()->addMenu(tr("&File"));

  const QIcon openIcon =
    QIcon::fromTheme("document-open", QIcon(":/images/open.png"));
  QAction *loadAct =
    fileMenu->addAction(openIcon, tr("&Load"), this, &MainWindow::wsLoad);
  loadAct->setShortcuts(QKeySequence::Open);
  loadAct->setStatusTip(tr("Load workspace"));

  const QIcon saveIcon =
    QIcon::fromTheme("document-save", QIcon(":/images/save.png"));
  QAction *saveAct = 
    fileMenu->addAction(openIcon, tr("&Save"), this, &MainWindow::wsSave);
  saveAct->setShortcuts(QKeySequence::Save);
  saveAct->setStatusTip(tr("Save workspace"));

  const QIcon saveAsIcon = QIcon::fromTheme("document-save-as");
  QAction *saveAsAct =
    fileMenu->addAction(saveAsIcon, tr("Save &As..."), this,
                        &MainWindow::wsSaveAs);
  saveAsAct->setShortcuts(QKeySequence::SaveAs);
  saveAsAct->setStatusTip(tr("Save workspace with name"));

  fileMenu->addSeparator();

  const QIcon exitIcon =
    QIcon::fromTheme("application-exit",
                     QIcon(":/images/application-exit.png"));
  QAction *exitAct =
    fileMenu->addAction(exitIcon, tr("E&xit"), this, &MainWindow::byebye);
  exitAct->setShortcuts(QKeySequence::Quit);

  /****************************************/

  QMenu *settingsMenu = menuBar()->addMenu(tr("&Settings"));

  QAction *editorAct =
    settingsMenu->addAction(tr("&Editor"), this, &MainWindow::setEditor);
  editorAct->setStatusTip(tr("Set editor"));
  
  QAction *fontAct =
    settingsMenu->addAction(tr("&Font"), this, &MainWindow::setFont);
  fontAct->setStatusTip(tr("Set font"));

  QAction *coloursAct =
    settingsMenu->addAction(tr("&Colours"), this, &MainWindow::setColours);
  coloursAct->setStatusTip(tr("Set colours"));

  /****************************************/

  QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));

  QAction *symbolsAct =
    helpMenu->addAction(tr("&Symbols"), this, &MainWindow::symbolsHelp);
  symbolsAct->setStatusTip(tr("Symbols help"));

  helpMenu->addSeparator();

  QAction *aboutAct =
    helpMenu->addAction(tr("&About"), this, &MainWindow::aboutHelp);
  aboutAct->setStatusTip(tr("About qapl"));

}

MainWindow::MainWindow(QCommandLineParser &parser, QWidget *parent)
  : QMainWindow(parent)
{
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
  
  if (!parser.isSet (OPT_noCONT))
    processLine (true, ")load CONTINUE");
  
  if (parser.isSet (OPT_L)) {
    QString cmd = QString (")load %1").arg(parser.value (OPT_L));
    processLine (false, cmd);
  }

  if (!parser.isSet (OPT_noINIT)) {
    QString pfn = QString ("./%1init.txt").arg (APPLICATION_NAME);
    if (!QFile::exists (pfn))
      pfn = QString ("%1/%2init.txt")
	.arg (getenv ("HOME"), APPLICATION_NAME);
    if (QFile::exists (pfn))
      read_script (pfn);
  }

  QStringList pargs = parser.positionalArguments();
  if (0 < pargs.size ()) {
    for (int i = 0; i <  pargs.size (); i++)
      read_script (pargs[i]);
  }
}

MainWindow::~MainWindow()
{
  delete history;
  tempdir.remove ();
}

