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


/*****
MainWindow window (startupMsgs, args, libpath, nullptr);
QVBoxLayout *layout = new QVBoxLayout ()
ayout->addWidget (aplwin);
this->setCentralWidget (outerGroupBox)

      
 ****/

HelpWindow::HelpWindow (MainWindow *parent)
  : QMainWindow(parent)
{
    //fprintf (stderr, "nr ets = %ld\n", (int)sizeof(help)/sizeof(help_s));


  QWidget *hw = new QWidget ();
  QVBoxLayout *layout = new QVBoxLayout;
  this->setCentralWidget(hw);
  

#if 0
  QLabel *lbl1 = new QLabel ("llllllllll");
  layout->addWidget(lbl1);

  QLabel *lbl2 = new QLabel ("mmmmmmmmmmm");
  layout->addWidget(lbl2);
#endif
  
  QTableWidget *table = new QTableWidget(this);
  table->setSelectionMode(QAbstractItemView::NoSelection);
  table->setShowGrid(false);
  
  int rowCount = (int)sizeof(help)/sizeof(help_s);
  table->setRowCount (rowCount);
  table->setColumnCount (5);
  //  for (int i = 0; i < rowCount; i++) {
  for (int i = 0; i < 10; i++) {
    QTableWidgetItem arityItem (help[i].arity);
    table->setItem (i, 0, &arityItem);
    QTableWidgetItem nameItem (help[i].name);
    table->setItem (i, 1, &nameItem);
  }
  layout->addWidget(table);


  hw->setLayout(layout);
  hw->show ();
}



HelpWindow::~HelpWindow()
{
}

