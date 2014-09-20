#ifndef ADSPAGE_H
#define ADSPAGE_H

#include <QWidget>

namespace Ui {
class AdsPage;
}

class WalletModel;
class AdsTableModel;
class AdsProxyModel;
class AdsPagePrivate;

class AdsPage : public QWidget
{
  Q_OBJECT
  
public:
  explicit AdsPage(QWidget *parent = 0);
  ~AdsPage();
  void setModel(WalletModel *model);
public Q_SLOTS:  
  void showAd(const QModelIndex &index);
private slots:
  void on_actionPostAd_triggered();
  void on_listAds_clicked(const QModelIndex &index);
private:
  Ui::AdsPage *ui;
  WalletModel* walletModel;
  AdsProxyModel* proxyModel;
  AdsPagePrivate* _private;
};

#endif // ADSPAGE_H
