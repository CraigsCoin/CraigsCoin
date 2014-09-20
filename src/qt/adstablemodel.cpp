#include "adstablemodel.h"
#include "walletmodel.h"
#include "optionsmodel.h"
#include "addresstablemodel.h"
#include "bitcoinunits.h"
#include "guiutil.h"
#include "wallet.h"

#include <QList>

#include <stdexcept>
#include <boost/assert.hpp>

/// Max displayed ads.
static const int MAX_ADS = 1000;

struct AdsModelRecord : public AdEntry
{
  uint256 hash;
  qint64 time;
  std::string rawmsg;

  void parseMsg(const std::string& msg)
  {
    rawmsg = msg;
    try
    {
      parse(msg);
    }
    catch (const std::exception& e)
    {
      ad.title = ad.title + "<parsing error>";
    }
  }

  AdsModelRecord(const uint256& ahash, const int64_t atime, const std::string& msg):
  hash(ahash), time(atime)
  {
    parseMsg(msg);
  }

  static AdsModelRecord decomposeTransaction(const CTransaction &tx)
  {
    BOOST_ASSERT(!tx.msg.empty());
    return AdsModelRecord (tx.GetHash(), tx.nTime, tx.msg);
  }

  static bool showTransaction(const CWalletTx &wtx)
  {
    return !wtx.msg.empty();
  }
};

struct TxLessThan
{
  bool operator()(const AdsModelRecord &a, const AdsModelRecord &b) const
  {
    return a.hash < b.hash;
  }
  bool operator()(const AdsModelRecord &a, const uint256 &b) const
  {
    return a.hash < b;
  }
  bool operator()(const uint256 &a, const AdsModelRecord &b) const
  {
    return a < b.hash;
  }
};

class AdsTableModelPrivate
{
public:
  AdsTableModelPrivate(AdsTableModel *parent):
      parent(parent)
      {
      }
      AdsTableModel *parent;

      QList<AdsModelRecord> cache;

      void refreshWallet()
      {
        cache.clear();
        int cnt = 0;
        for(CBlockIndex* pindex = pindexBest; pindex; pindex = pindex->pprev)
        {
          CBlock block;
          if (!block.ReadFromDisk(pindex))
          {
            continue;
          }
          BOOST_FOREACH(const CTransaction &tx, block.vtx)
          {
            if (!tx.msg.empty())
            {
              const uint256 hashTx = tx.GetHash();
              updateWallet(hashTx, CT_NEW);
              cnt ++;
              if (cnt > MAX_ADS)
              {
                return;
              }
            }
          }
        }
      }

      void updateWallet(const uint256 &hash, int status)
      {
        const QList<AdsModelRecord>::iterator lower = qLowerBound(
                cache.begin(), cache.end(), hash, TxLessThan());
        const bool inModel = (lower != cache.end() && (*lower).hash == hash);
        const int lowerIndex = (lower - cache.begin());
        {
          switch(status)
          {
          case CT_NEW:
            {
              if(inModel)
              {
                break;
              }
              CTransaction tx;
              uint256 hashBlock;
              if (!GetTransaction(hash, tx, hashBlock))
              {
                break;
              }

              const AdsModelRecord toInsert = AdsModelRecord::decomposeTransaction(tx);
              parent->beginInsertRows(QModelIndex(), lowerIndex, lowerIndex);
              cache.insert(lowerIndex, toInsert);
              parent->endInsertRows();
            }
            break;
          case CT_DELETED:
            if(!inModel)
            {
              break;
            }
            // Removed -- remove entire transaction from table
            parent->beginRemoveRows(QModelIndex(), lowerIndex, lowerIndex);
            cache.erase(lower);
            parent->endRemoveRows();
            break;
          case CT_UPDATED:
            // Miscellaneous updates -- nothing to do, status update will take care of this, and is only computed for
            // visible transactions.
            break;
          }
        }
        if (cache.size() > MAX_ADS)
        {
          // remove records, exceeding limit, beginning from oldest. 
          QList<AdsModelRecord>::iterator i = cache.begin(), iend = cache.end();
          QList<AdsModelRecord>::iterator imindate = iend;
          for(; i != iend; ++ i)
          {
            if (imindate == cache.end() || (*i).time < (*imindate).time)
            {
              imindate = i;
            }
          }
          if (imindate != iend)
          {
            updateWallet((*imindate).hash, CT_DELETED);
          }
        }
      }

      int size()
      {
        return cache.size();
      }

      AdsModelRecord *index(int idx)
      {
        if(idx >= 0 && idx < cache.size())
        {
          AdsModelRecord *rec = &cache[idx];

          //// If a status update is needed (blocks came in since last check),
          ////  update the status of this transaction from the wallet. Otherwise,
          //// simply re-use the cached status.
          //if(rec->statusUpdateNeeded())
          //{
          //    {
          //        LOCK(wallet->cs_wallet);
          //        std::map<uint256, CWalletTx>::iterator mi = wallet->mapWallet.find(rec->hash);

          //        if(mi != wallet->mapWallet.end())
          //        {
          //            rec->updateStatus(mi->second);
          //        }
          //    }
          //}
          return rec;
        }
        else
        {
          return 0;
        }
      }

      QString describe(AdsModelRecord *rec)
      {
        return QString("");
      }

};

AdsTableModel::AdsTableModel(CWallet* wallet, WalletModel *parent):
QAbstractTableModel(parent),
  walletModel(parent),
  priv(new AdsTableModelPrivate(this))
{
  columns << tr("Date") << tr("Title") << "AdEntry";
  priv->refreshWallet();
}

int AdsTableModel::rowCount(const QModelIndex &parent) const
{
  Q_UNUSED(parent);
  return priv->size();
}

int AdsTableModel::columnCount(const QModelIndex &parent) const
{
  Q_UNUSED(parent);
  return columns.length();
}

QVariant AdsTableModel::data(const QModelIndex &index, int role) const
{
  if(!index.isValid())
    return QVariant();
  AdsModelRecord *rec = static_cast<AdsModelRecord*>(index.internalPointer());

  switch(role)
  {
  case Qt::DisplayRole:
    switch(index.column())
    {
    case Date:
      return formatTxDate(rec);
    case Title:
      return QString::fromUtf8(rec->ad.title.data(), rec->ad.title.size());
    case AdEntryIndex:
      return QVariant::fromValue<AdEntry>(*static_cast<AdEntry*>(rec));
    }
    break;
  case Qt::EditRole:
    // Edit role is used for sorting, so return the unformatted values
    switch(index.column())
    {
    case Date:
      return rec->time;
    case Title:
      return QString::fromUtf8(rec->ad.title.data(), rec->ad.title.size());
    case AdEntryIndex:
      return QVariant::fromValue<AdEntry>(*static_cast<AdEntry*>(rec));
    }
    break;
  }
  return QVariant();
}

QVariant AdsTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if(orientation == Qt::Horizontal)
  {
    if(role == Qt::DisplayRole)
    {
      return columns[section];
    }
  }
  return QVariant();
}

QModelIndex AdsTableModel::index(int row, int column, const QModelIndex &parent) const
{
  Q_UNUSED(parent);
  AdsModelRecord *data = priv->index(row);
  if(data)
  {
    return createIndex(row, column, priv->index(row));
  }
  else
  {
    return QModelIndex();
  }
}

QString AdsTableModel::formatTxDate(const AdsModelRecord *wtx) const
{
  if(wtx->time)
  {
    return GUIUtil::dateTimeStr(wtx->time);
  }
  else
  {
    return QString();
  }
}

void AdsTableModel::updateTransaction(const QString &hash, int status)
{
  uint256 updated;
  updated.SetHex(hash.toStdString());

  priv->updateWallet(updated, status);
}


AdsProxyModel::AdsProxyModel(QObject *parent)
{
}

int AdsProxyModel::rowCount(const QModelIndex &parent) const
{
  return QSortFilterProxyModel::rowCount(parent);
}

bool AdsProxyModel::filterAcceptsRow(int /*source_row*/,
    const QModelIndex & /*source_parent*/) const
{
  return true;
}
