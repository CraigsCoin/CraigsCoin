#include "adspage.h"
#include "ui_adspage.h"
#include "walletmodel.h"
#include "postad.h"
#include "ui_postad.h"
#include "bitcoinunits.h"
#include "optionsmodel.h"
#include "coincontroldialog.h"
#include "adstablemodel.h"
#include "showad.h"
#include "main.h"

#include <QMessageBox>

AdsPage::AdsPage(QWidget *parent) :
QWidget(parent),
  ui(new Ui::AdsPage),
  walletModel(0)
{
  ui->setupUi(this);
}

AdsPage::~AdsPage()
{
  delete ui;
}

void AdsPage::setModel(WalletModel *amodel)
{
  walletModel = amodel;
  if (walletModel)
  {
    proxyModel = new AdsProxyModel(this);
    proxyModel->setSourceModel(amodel->getAdsTableModel());
    proxyModel->setDynamicSortFilter(true);
    proxyModel->setSortCaseSensitivity(Qt::CaseInsensitive);
    proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);

    proxyModel->setSortRole(Qt::EditRole);


    ui->listAds->setModel(proxyModel);
    ui->listAds->setAlternatingRowColors(true);
    ui->listAds->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->listAds->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui->listAds->setSortingEnabled(true);
    ui->listAds->sortByColumn(AdsTableModel::Date, Qt::DescendingOrder);
    ui->listAds->verticalHeader()->hide();
    ui->listAds->setColumnHidden(AdsTableModel::Body, true);

    ui->listAds->horizontalHeader()->resizeSection(
          AdsTableModel::Date, 120);
    ui->listAds->horizontalHeader()->setStretchLastSection(true);
  }
}


void AdsPage::on_actionShowAd_triggered()
{

}

void AdsPage::on_actionPostAd_triggered()
{
  PostAd dlg;
  const QDialog::DialogCode result = (QDialog::DialogCode)dlg.exec();
  if (result != QDialog::Accepted)
  {
    return;
  }
  const QString msg = dlg.ui->editTitle->text() + "\n" + dlg.ui->editMessage->document()->toPlainText();
  QList<SendCoinsRecipient> recipients;

  if(!walletModel)
  {
    return;
  }

  SendCoinsRecipient rv;

  if (fTestNet)
    rv.address = AD_FEE_ADDRESS_TESTNET;
  else
    rv.address = AD_FEE_ADDRESS;
  rv.label = "Ad fee";
  rv.amount = AD_FEE_AMOUNT;

  recipients.append(rv);

  // Format confirmation message
  QStringList formatted;
  foreach(const SendCoinsRecipient &rcp, recipients)
  {
    formatted.append(tr("<b>%1</b> to %2 (%3)").arg(BitcoinUnits::formatWithUnit(BitcoinUnits::BTC, rcp.amount), Qt::escape(rcp.label), rcp.address));
  }

  QMessageBox::StandardButton retval = QMessageBox::question(this, tr("Confirm send coins"),
    tr("Are you sure you want to send %1?").arg(formatted.join(tr(" and "))),
    QMessageBox::Yes|QMessageBox::Cancel,
    QMessageBox::Cancel);

  if(retval != QMessageBox::Yes)
  {
    return;
  }

  WalletModel::UnlockContext ctx(walletModel->requestUnlock());
  if(!ctx.isValid())
  {
    return;
  }

  WalletModel::SendCoinsReturn sendstatus;

  if (!walletModel->getOptionsModel() || !walletModel->getOptionsModel()->getCoinControlFeatures())
    sendstatus = walletModel->sendCoins(recipients, msg);
  else
    sendstatus = walletModel->sendCoins(recipients, msg, CoinControlDialog::coinControl);

  switch(sendstatus.status)
  {
  case WalletModel::InvalidAddress:
    QMessageBox::warning(this, tr("Send Coins"),
      tr("The recipient address is not valid, please recheck."),
      QMessageBox::Ok, QMessageBox::Ok);
    break;
  case WalletModel::InvalidAmount:
    QMessageBox::warning(this, tr("Send Coins"),
      tr("The amount to pay must be larger than 0."),
      QMessageBox::Ok, QMessageBox::Ok);
    break;
  case WalletModel::AmountExceedsBalance:
    QMessageBox::warning(this, tr("Send Coins"),
      tr("The amount exceeds your balance."),
      QMessageBox::Ok, QMessageBox::Ok);
    break;
  case WalletModel::AmountWithFeeExceedsBalance:
    QMessageBox::warning(this, tr("Send Coins"),
      tr("The total exceeds your balance when the %1 transaction fee is included.").
      arg(BitcoinUnits::formatWithUnit(BitcoinUnits::BTC, sendstatus.fee)),
      QMessageBox::Ok, QMessageBox::Ok);
    break;
  case WalletModel::DuplicateAddress:
    QMessageBox::warning(this, tr("Send Coins"),
      tr("Duplicate address found, can only send to each address once per send operation."),
      QMessageBox::Ok, QMessageBox::Ok);
    break;
  case WalletModel::TransactionCreationFailed:
    QMessageBox::warning(this, tr("Send Coins"),
      tr("Error: Transaction creation failed."),
      QMessageBox::Ok, QMessageBox::Ok);
    break;
  case WalletModel::TransactionCommitFailed:
    QMessageBox::warning(this, tr("Send Coins"),
      tr("Error: The transaction was rejected. This might happen if some of the coins in your wallet were already spent, such as if you used a copy of wallet.dat and coins were spent in the copy but not marked as spent here."),
      QMessageBox::Ok, QMessageBox::Ok);
    break;
  case WalletModel::Aborted: // User aborted, nothing to do
    break;
  case WalletModel::OK:
    break;
  }
}

void AdsPage::on_listAds_clicked(const QModelIndex &index)
{
  showAd(index);
}


void AdsPage::showAd(const QModelIndex &index)
{
  if (!index.isValid())
    return;

  ShowAd* showAd = new ShowAd(this);
  showAd->show(index);
}
