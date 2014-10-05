#include "showad.h"
#include "ui_showad.h"
#include "adstablemodel.h"

#include <QAbstractItemModel>
#include <QDesktopWidget>
#include <QDesktopServices>

#include <boost/foreach.hpp>

ShowAd::ShowAd(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::ShowAd)
{
  ui->setupUi(this);
}

ShowAd::~ShowAd()
{
  delete ui;
}

QString getUtf8String(const std::string& src)
{
  const QByteArray arr = QByteArray::fromRawData(src.data(), src.size());
  return QString::fromUtf8(arr);
}

void toUi(const AdEntry& entry, Ui::ShowAd* ui)
{
  ui->name->setText(getUtf8String(entry.person.name));
  ui->contacts->setPlainText(getUtf8String(entry.person.contacts));

  ui->country->setText(getUtf8String(entry.location.country));
  ui->state->setText(getUtf8String(entry.location.state));
  ui->town->setText(getUtf8String(entry.location.city));
  ui->address->setText(getUtf8String(entry.location.street_address));
  ui->zipcode->setText(getUtf8String(entry.location.postal_code));

  ui->category->setText(getUtf8String(entry.category.level_1));
  ui->subcategory->setText(getUtf8String(entry.category.level_2));

  ui->title->setText(getUtf8String(entry.ad.title));
  ui->description->setPlainText(getUtf8String(entry.ad.description));
  {
    std::string stags;
    BOOST_FOREACH (const std::string& item, entry.ad.tags)
    {
      if (!stags.empty())
      {
        stags += ", ";
      }
      stags += item;
    }
    ui->tags->setText(getUtf8String(stags));
  }
  ui->price->setText(getUtf8String(entry.ad.price));
  {
    BOOST_FOREACH(const std::string& item, entry.ad.images)
    {
      ui->images->insertItem(ui->images->count(), getUtf8String(item));
    }
  }
  {
    BOOST_FOREACH(const std::string& item, entry.ad.videos)
    {
      ui->videos->insertItem(ui->videos->count(), getUtf8String(item));
    }
  }
}

void ShowAd::show(const QModelIndex& index)
{
  const QAbstractItemModel* const model = index.model();
  Q_ASSERT(model);
  const AdEntry ad = model->data(model->index(index.row(),
      AdsTableModel::AdEntryIndex, index.parent()))
      .value<AdEntry>();
  toUi(ad, ui);
  ui->date->setText(model->data(model->index(index.row(),
    AdsTableModel::Date, index.parent()))
    .toString());

  setWindowFlags(Qt::Drawer);
  //updateGeometry();
  //QRect rect = frameGeometry();
  //rect.moveCenter(QDesktopWidget().availableGeometry().center());
  //move(rect.topLeft());
  setWindowTitle(ui->title->text());
  setAttribute(Qt::WA_DeleteOnClose, true);
  QDialog::show();
}

void ShowAd::openUrl(QListWidgetItem *item)
{
  if (item)
  {
    QDesktopServices::openUrl(item->data(Qt::DisplayRole).toString());
  }
}
