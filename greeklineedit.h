#ifndef GREEKLINEEDIT_H
#define GREEKLINEEDIT_H

#include <QLineEdit>
#include <QDialog>
#include <QGridLayout>

class
GreekDialog : public QDialog
{
  Q_OBJECT

 public:
  GreekDialog (QWidget *parent = nullptr,
	       Qt::WindowFlags f = Qt::WindowFlags(),
	       QGridLayout *bl = nullptr);
  ~GreekDialog () {}

protected:
  void	keyPressEvent(QKeyEvent *e) override;
  void	keyReleaseEvent(QKeyEvent *e) override;

 private:
  QGridLayout *buttonLayout;
};

class
GreekLineEdit : public QLineEdit
{
  Q_OBJECT

public:
  GreekLineEdit (QWidget *parent = nullptr);
  ~GreekLineEdit ();

 protected:
  void contextMenuEvent(QContextMenuEvent *event) override;
  //void keyPressEvent(QKeyEvent *event);

 private:
  void insertChar ();

};
#endif  // GREEKLINEEDIT_H

