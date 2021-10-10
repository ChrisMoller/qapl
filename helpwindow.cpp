#include <QMainWindow>
#include <QTableWidget>
#include <QTableWidgetItem>
#include "mainwindow.h"
#include "helpwindow.h"

typedef struct {
  int arity;
  const char *prim;
  const char *name;
  const char *title;
  const char *desc;
} help_s;

#define help_def(ar, prim, name, title, descr) \
  {ar, prim, name, title, descr},
help_s help[] = {
#include "src/Help.def"
};

void
HelpWindow::closeEvent(QCloseEvent *event __attribute__((unused)))
{
  mw->closeHW ();
}

HelpWindow::HelpWindow (MainWindow *parent)
  : QMainWindow(parent)
{
  mw = parent;
  QWidget *hw = new QWidget ();
  QVBoxLayout *layout = new QVBoxLayout;
  this->setCentralWidget(hw);
  
  int rowCount = (int)sizeof(help)/sizeof(help_s);
  QTableWidget *table = new QTableWidget (rowCount, 2, this);
  table->setShowGrid(false);
  //  QStringList headers = {"Symbol", "Name", "Title", "Description"};
  QStringList headers = {"Name", "Title"};
  table->setHorizontalHeaderLabels (headers);
  
  for (int i = 0; i < rowCount; i++) {
    //  for (int i = 0; i < 10; i++) {
#if 0
    QTableWidgetItem *arityItem
      = new QTableWidgetItem (QString::number (help[i].arity));
    table->setItem (i, 0, arityItem);
#endif

    QTableWidgetItem *primItem
      = new QTableWidgetItem (QString (help[i].prim));
#if 1
    table->setVerticalHeaderItem (i, primItem);
#else
    table->setItem (i, 0, primItem);
#endif
    
    QTableWidgetItem *nameItem
      = new QTableWidgetItem (QString (help[i].name));
    table->setItem (i, 0, nameItem);
    
    QTableWidgetItem *titleItem
      = new QTableWidgetItem (QString (help[i].title));
    table->setItem (i, 1, titleItem);

#if 0
    QTableWidgetItem *descItem
      = new QTableWidgetItem (QString (help[i].desc));
    table->setItem (i, 3, descItem);
#endif
  }

  table->resizeColumnToContents (0);
  table->setColumnWidth (1, 640 - table->columnWidth (0));
  table->setMinimumWidth (640);
  layout->addWidget(table);


  hw->setLayout(layout);
  this->show ();
}

HelpWindow::~HelpWindow()
{;
}

