#ifndef POSTAD_H
#define POSTAD_H

static const unsigned int AD_MAX_BODY_LENGTH = 1000;
static const unsigned int AD_MAX_TITLE_LENGTH = 100;

#include <QDialog>

namespace Ui {
class PostAd;
}

class PostAd : public QDialog
{
  Q_OBJECT
  
public:
  explicit PostAd(QWidget *parent = 0);
  ~PostAd();

public Q_SLOT:
  virtual void 	done(int r);

private slots:
  void on_editMessage_textChanged();
  void on_editTitle_textChanged(const QString &arg1);

private:
  void updateBodyLength();
  void updateTitleLength();
private:
  Ui::PostAd *ui;

  friend class AdsPage;
};

#endif // POSTAD_H
