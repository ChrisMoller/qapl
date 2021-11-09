#ifndef GREEKLINEEDIT_H
#define GREEKLINEEDIT_H

#include <QLineEdit>

class
GreekLineEdit : public QLineEdit
{
  Q_OBJECT

public:
  GreekLineEdit (QWidget *parent = nullptr);
  ~GreekLineEdit ();

 protected:
  void contextMenuEvent(QContextMenuEvent *event) override;

 private:
  void insertChar ();

};
#endif  // GREEKLINEEDIT_H

