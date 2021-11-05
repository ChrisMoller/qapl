#include "mainwindow.h"
#include "plot2dwindow.h"

void
MainWindow::plot2d ()
{
  new Plot2DWindow (this, nullptr);
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
  if (!HWopen) {
    new HelpWindow (this);
    HWopen = true;
  }
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
  colourDialog->setWindowFlags(Qt::WindowStaysOnTopHint);
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
  colourDialog->setWindowFlags(Qt::WindowStaysOnTopHint);
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


void MainWindow::importChart ()
{
  QFileDialog dialog (this, "Export As...", ".",
		      tr("Plot Files (*.plot)"));

  dialog.setOption (QFileDialog::DontUseNativeDialog);
  dialog.setAcceptMode (QFileDialog::AcceptOpen);

   int drc = dialog.exec();
  
  if (drc == QDialog::Accepted) {
    //    currentFile = dialog.selectedFiles().first();
    QString cf  = dialog.selectedFiles().first();
    Plot2DWindow::readXML (cf, this);
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

  QAction *importAct =
    fileMenu->addAction(tr("&Import"), this,
			    &MainWindow::importChart);
  importAct->setStatusTip(tr("Import chart"));
  
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

  /****************************************/

  QMenu *plotMenu = menuBar()->addMenu(tr("&Plot"));

  QAction *plot2dAct =
    plotMenu->addAction(tr("&2D Plots"), this, &MainWindow::plot2d);
  plot2dAct->setStatusTip(tr("2D plots"));

}
