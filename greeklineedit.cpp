#include <QtWidgets>
//#include <QLineEdit>

#include "greeklineedit.h"

// https://doc.qt.io/qt-5/qlineedit.html#contextMenuEvent

GreekDialog::GreekDialog (QWidget *parent, Qt::WindowFlags f,
			  QGridLayout *bl)
  : QDialog (parent, f)
{
  buttonLayout = bl;
}

typedef struct {
  const char *name;
  const char *upperCase;
  const char *lowerCase;
  uint        upperUnicode;
  uint        lowerUnicode;
} greek_s;

greek_s greek[] = {
  {"Alpha",	"A", 0x0391,	"α", 0x03b1},
  {"Beta",	"Β", 0x0392,	"β", 0x03b2},
  {"Gamma",	"Γ", 0x0393,	"γ", 0x03b3},
  {"Delta",	"Δ", 0x0394,	"δ", 0x03b4},
  {"Epsilon",	"Ε", 0x0395,	"ε", 0x03b5},
  {"Zeta",	"Ζ", 0x0396,	"ζ", 0x03b6},
  {"Eta",	"Η", 0x0397,	"η", 0x03b7},
  {"Theta",	"Θ", 0x0398,	"θ", 0x03b8},
  {"Iota",	"Ι", 0x0399,	"ι", 0x03b9},
  {"Kappa",	"Κ", 0x039a,	"κ", 0x03ba},
  {"Lambda",	"Λ", 0x039b,	"λ", 0x03bb},
  {"Mu",	"Μ", 0x039c,	"μ", 0x03bc},
  {"Nu",	"Ν", 0x039d, 	"ν", 0x03bd},
  {"Xi",	"Ξ", 0x039e,	"ξ", 0x03be},
  {"Omicron",	"Ο", 0x039f, 	"ο", 0x03bf},
  {"Pi",	"Π", 0x03a0,	"π", 0x03c0},
  {"Rho",	"Ρ", 0x03a1,	"ρ", 0x03c1},
  {"Sigma",	"Σ", 0x03a3,	"σ", 0x03c3},
  {"Tau",	"Τ", 0x03a4,	"τ", 0x03c4},
  {"Upsilon",	"Υ", 0x03a5,	"υ", 0x03c5},
  {"Phi",	"Φ", 0x03a6,	"φ", 0x03c6},
  {"Chi",	"Χ", 0x03a7,	"χ", 0x03c7},
  {"Psi",	"Ψ", 0x03a8,	"ψ", 0x03c8},
  {"Omega",	"Ω", 0x03a9,	"ω", 0x03c9},
};

#define toCString(v)       ((v).toStdString ().c_str ())

void GreekDialog::keyPressEvent(QKeyEvent *keyEvent)
{
  int offset = 0;
  for (int row = 0; row < 4; row++) {
    for (int col = 0; col < 6; col++, offset++) {
      QLayoutItem *li = buttonLayout->itemAtPosition (row, col);
      QPushButton *character = static_cast<QPushButton *>(li->widget ());
      character->setText (QString (greek[offset].upperCase));
    }
  }
  QDialog::keyPressEvent(keyEvent);
}

void GreekDialog::keyReleaseEvent(QKeyEvent *keyEvent)
{
  int offset = 0;
  for (int row = 0; row < 4; row++) {
    for (int col = 0; col < 6; col++, offset++) {
      QLayoutItem *li = buttonLayout->itemAtPosition (row, col);
      QPushButton *character = static_cast<QPushButton *>(li->widget ());
      character->setText (QString (greek[offset].lowerCase));
    }
  }
  QDialog::keyPressEvent(keyEvent);
}

void GreekLineEdit::insertChar ()
{
  QGridLayout *buttonLayout = new QGridLayout;
  GreekDialog dialog (this, Qt::Dialog, buttonLayout);
  QVBoxLayout *dialogLayout = new QVBoxLayout ();
  dialog.setLayout (dialogLayout);

  QGroupBox *gbox = new QGroupBox ();
  gbox->setLayout (buttonLayout);
  int offset = 0;
  for (int row = 0; row < 4; row++) {
    for (int col = 0; col < 6; col++, offset++) {
      QPushButton *character =
	new QPushButton (QString (greek[offset].lowerCase));
      connect(character, &QPushButton::clicked,
	      [=](){
		this->insert (character->text ());
	      });
      buttonLayout->addWidget (character, row, col);
    }
  }  
  dialogLayout->addWidget (gbox);

  QGroupBox *gbox2 = new QGroupBox ();
  QHBoxLayout *unicodeLayout = new QHBoxLayout;
  gbox2->setLayout (unicodeLayout);

  QLabel unicodeLbl (tr ("Unicode"));
  unicodeLayout->addWidget (&unicodeLbl);

  QLineEdit *unicodeLine = new QLineEdit ();
  connect (unicodeLine,
           &QLineEdit::editingFinished,
          [=](){
	    uint val = unicodeLine->text ().toUInt (nullptr, 0);
	    QChar chr (val);
	    this->insert (QString (chr));
          });
  unicodeLayout->addWidget (unicodeLine);
  
  dialogLayout->addWidget (gbox2);
  

  QPushButton *closeButton = new QPushButton (QObject::tr ("Close"));
  closeButton->setAutoDefault (true);
  closeButton->setDefault (true);
  dialogLayout->addWidget (closeButton);
  QObject::connect (closeButton, &QPushButton::clicked,
                    &dialog, &QDialog::accept);

  dialog.exec ();

  emit editingFinished ();
}

void GreekLineEdit::contextMenuEvent(QContextMenuEvent *event)
{
  QMenu *menu = createStandardContextMenu();
  menu->setAttribute(Qt::WA_DeleteOnClose);
  menu->addSeparator();

  QAction *insertAct = 
    menu->addAction(tr("Insert char"), this,
		    & GreekLineEdit::insertChar);
  insertAct->setStatusTip(tr("Insert char"));


  menu->popup(event->globalPos());
}

GreekLineEdit::GreekLineEdit (QWidget *parent)
  : QLineEdit(parent)
{
  // this->contextMenuEvent (contextMenuEvent);
}


GreekLineEdit::~GreekLineEdit ()
{
}
