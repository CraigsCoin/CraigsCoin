#include "postad.h"
#include "ui_postad.h"
#include "bitcoinunits.h"
#include "main.h"

PostAd::PostAd(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::PostAd)
{
  ui->setupUi(this);
  updateBodyLength();
  updateTitleLength();
  ui->labelPostFee->setText(QString("Fee: %1").arg(
      BitcoinUnits::formatWithUnit(BitcoinUnits::BTC, AD_FEE_AMOUNT)));
  ui->labelErrors->hide();
}

PostAd::~PostAd()
{
  delete ui;
}

void updateLength(QLabel* label, const size_t sz, const size_t maxSz)
{
  label->setText(QString("%1/%2").arg(sz).arg(maxSz));
  if (sz > maxSz)
  {
    label->setStyleSheet("background-color: red");
  }
  else
  {
    label->setStyleSheet("");
  }
}

void PostAd::updateBodyLength()
{
  updateLength(ui->labelBodyLength,
        ui->editMessage->toPlainText().size(),
        AD_MAX_BODY_LENGTH);
}

void PostAd::updateTitleLength()
{
  updateLength(ui->labelTitleLength,
        ui->editTitle->text().size(),
        AD_MAX_TITLE_LENGTH);
}

void PostAd::on_editMessage_textChanged()
{
  updateBodyLength();
}

void PostAd::on_editTitle_textChanged(const QString &)
{
   updateTitleLength();
}

void PostAd::done(int r)
{
  if (r == QDialog::Accepted)
  {
    QStringList errors;

    // validate title.
    {
      const QString t = ui->editTitle->text();
      if (t.isEmpty())
      {
        errors.append("Ad title is empty.");
      }
      else if (t.size() > AD_MAX_TITLE_LENGTH)
      {
        errors.append("Ad title is too long.");
      }
    }
    // validate body.
    {
      const QString b = ui->editMessage->toPlainText();
      if (b.isEmpty())
      {
        errors.append("Ad body is empty.");
      }
      if (b.size() > AD_MAX_BODY_LENGTH)
      {
        errors.append("Ad body is too long.");
      }
    }

    ui->labelErrors->setVisible(!errors.empty());
    if (!errors.empty())
    {
      ui->labelErrors->setText(errors.join("\n"));
      return;
    }
  }
  QDialog::done(r);
  return;
}
