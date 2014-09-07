#ifndef SHOWAD_H
#define SHOWAD_H

#include <QDialog>

namespace Ui {
class ShowAd;
}

class QModelIndex;

class ShowAd : public QDialog
{
  Q_OBJECT

public:
  explicit ShowAd(QWidget *parent = 0);
  void show(const QModelIndex& index);
  ~ShowAd();

private:
  Ui::ShowAd *ui;
};

#endif // SHOWAD_H
