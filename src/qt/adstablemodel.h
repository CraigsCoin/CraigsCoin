#ifndef ADSTABLEMODEL_H
#define ADSTABLEMODEL_H

#include <QAbstractTableModel>
#include <QSortFilterProxyModel>
#include <QStringList>

class CWallet;
class AdsModelRecord;
class WalletModel;
class AdsTableModelPrivate;

class AdsTableModel : public QAbstractTableModel
{
  Q_OBJECT
public:
  enum ColumnIndex {
      Date = 0,
      Title,
      Body
  };
public:
  explicit AdsTableModel(CWallet* wallet, WalletModel *parent);
  
  int rowCount(const QModelIndex &parent) const;
  int columnCount(const QModelIndex &parent) const;
  QVariant data(const QModelIndex &index, int role) const;
  QVariant headerData(int section, Qt::Orientation orientation, int role) const;
  QModelIndex index(int row, int column, const QModelIndex & parent = QModelIndex()) const;
public Q_SLOTS:
  void updateTransaction(const QString &hash, int status);

private:
  QString formatTxDate(const AdsModelRecord *wtx) const;

private:
  CWallet* wallet;
  WalletModel *walletModel;
  QStringList columns;
  AdsTableModelPrivate* priv;


  friend class AdsTableModelPrivate;
};

class AdsProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit AdsProxyModel(QObject *parent = 0);
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
protected:
    bool filterAcceptsRow(int source_row, const QModelIndex & source_parent) const;
};

#endif // ADSTABLEMODEL_H
