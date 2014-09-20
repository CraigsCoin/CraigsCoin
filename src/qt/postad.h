#ifndef POSTAD_H
#define POSTAD_H

static const unsigned int AD_MARKUP_LENGTH = 300;
static const unsigned int AD_MAX_BYTES = 10000 + AD_MARKUP_LENGTH;

#include <QDialog>

#include <string>

namespace Ui {
class PostAd;
}

class QSqlDatabase;
class PostAdPrivate;

class PostAd : public QDialog
{
  Q_OBJECT
  
public:
  explicit PostAd(QSqlDatabase& db, QWidget *parent = 0);
  ~PostAd();

public slots:
  virtual void 	done(int r);
  void updateTextLength();

private slots:
  void on_cbxCountry_currentIndexChanged(int index);
  void on_cbxState_currentIndexChanged(int index);
  void on_cbxCategory_currentIndexChanged(int index);
  void on_btnVideoAdd_clicked();

  void on_btnVideoRemove_clicked();

  void on_btnImageAdd_clicked();

  void on_btnImageRemove_clicked();

private:
  std::string _formatMsg() const;
private:
  Ui::PostAd *ui;
  PostAdPrivate* _private;
  std::string resultMsg;

  friend class AdsPage;
};

#endif // POSTAD_H
