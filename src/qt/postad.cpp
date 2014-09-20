#include "postad.h"
#include "ui_postad.h"
#include "bitcoinunits.h"
#include "ads.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlQueryModel>

class PostAdPrivate
{
public:
  PostAdPrivate(QSqlDatabase& /*db*/):
    countryModel(new QSqlQueryModel),
    regionModel(new QSqlQueryModel),
    cityModel(new QSqlQueryModel),
    categoryModel(new QSqlQueryModel),
    subcategoryModel(new QSqlQueryModel)
    
  {
    countryQuery.prepare("select code, name from countries order by name");
    countryQuery.exec();
    regionQuery.prepare("select code, name from regions where country = ? order by name");
    cityQuery.prepare("select name from cities where country = ? and region = ? order by name");
    categoryQuery.prepare("select id, name from category where id_parent is null");
    categoryQuery.exec();
    subcategoryQuery.prepare("select id, name from category where id_parent = ?");

    countryModel->setQuery(countryQuery);
    categoryModel->setQuery(categoryQuery);
  }
  
  ~PostAdPrivate()
  {
    delete subcategoryModel;
    delete categoryModel;
    delete cityModel;
    delete regionModel;
    delete countryModel;
  }

  void countryChanged(const QVariant& newId)
  {
    if (newId != curCountryId)
    {
      curCountryId = newId;
      regionQuery.bindValue(0, curCountryId);
      regionQuery.exec();
      regionQuery.first();
      regionModel->setQuery(regionQuery);
      regionChanged(QVariant());
    }
  }
  void regionChanged(const QVariant& newId)
  {
    if (newId != curRegionId)
    {
      curRegionId = newId;
      cityQuery.bindValue(0, curCountryId);
      cityQuery.bindValue(1, curRegionId);
      cityQuery.exec();
      cityQuery.first();
      cityModel->setQuery(cityQuery);
    }
  }
  void categoryChanged(const QVariant& newId)
  {
    if (newId != curCategoryId)
    {
      curCategoryId = newId;
      subcategoryQuery.bindValue(0, curCategoryId);
      subcategoryQuery.exec();
      subcategoryQuery.first();
      subcategoryModel->setQuery(subcategoryQuery);
    }
  }
public:
  QSqlQuery countryQuery;
  QSqlQuery regionQuery;
  QSqlQuery cityQuery;
  QSqlQuery categoryQuery;
  QSqlQuery subcategoryQuery;
  QVariant curCountryId;
  QVariant curRegionId;
  QVariant curCategoryId;
  QSqlQueryModel* countryModel;
  QSqlQueryModel* regionModel;
  QSqlQueryModel* cityModel;
  QSqlQueryModel* categoryModel;
  QSqlQueryModel* subcategoryModel;
};

PostAd::PostAd(QSqlDatabase& db, QWidget *parent) :
  QDialog(parent),
  ui(new Ui::PostAd),
  _private(new PostAdPrivate(db))
{
  ui->setupUi(this);
  updateTextLength();
  ui->labelPostFee->setText(QString("Fee: %1").arg(
      BitcoinUnits::formatWithUnit(BitcoinUnits::BTC, AD_FEE_AMOUNT)));
  ui->labelErrors->hide();

  ui->cbxCountry->setModel(_private->countryModel);
  ui->cbxCountry->setModelColumn(1);

  ui->cbxState->setModel(_private->regionModel);
  ui->cbxState->setModelColumn(1);

  ui->cbxTown->setModel(_private->cityModel);
  ui->cbxTown->setModelColumn(0);

  ui->cbxCategory->setModel(_private->categoryModel);
  ui->cbxCategory->setModelColumn(1);

  ui->cbxSubCategory->setModel(_private->subcategoryModel);
  ui->cbxSubCategory->setModelColumn(1);
}

PostAd::~PostAd()
{
  delete _private;
  delete ui;
}

void updateLength(QLabel* label, const size_t sz, const size_t maxSz)
{
  label->setText(QString("%1/%2 bytes").arg(sz).arg(maxSz));
  if (sz > maxSz)
  {
    label->setStyleSheet("background-color: red");
  }
  else
  {
    label->setStyleSheet("");
  }
}

void PostAd::updateTextLength()
{
  updateLength(ui->labelBodyLength,
        _formatMsg().size(),
        AD_MAX_BYTES);
}

std::string getUtf8String(const QString& src)
{
  const QByteArray arr = src.toUtf8();
  return std::string(arr.begin(), arr.end());
}

AdEntry fromUi(const Ui_PostAd* const ui)
{
  AdEntry result;
  result.person.name = getUtf8String(ui->editName->text());
  result.person.contacts = getUtf8String(ui->editContacts->toPlainText());

  result.location.country = getUtf8String(ui->cbxCountry->currentText());
  result.location.state = getUtf8String(ui->cbxState->currentText());
  result.location.city = getUtf8String(ui->cbxTown->currentText());
  result.location.street_address = getUtf8String(ui->editAddress->text());
  result.location.postal_code = getUtf8String(ui->editZipCode->text());
  
  result.category.level_1 = getUtf8String(ui->cbxCategory->currentText());
  result.category.level_2 = getUtf8String(ui->cbxSubCategory->currentText());

  result.ad.title = getUtf8String(ui->editTitle->text());
  result.ad.description = getUtf8String(ui->editMessage->toPlainText());
  {
    const QStringList lst = ui->editTags->text().split(",");
    Q_FOREACH(QString tag, lst)
    {
      result.ad.tags.push_back(getUtf8String(tag.trimmed()));
    }
  }
  result.ad.price = getUtf8String(ui->editPrice->text());
  {
    for (int i = 0; i < ui->listImages->count(); ++i)
    {
      result.ad.images.push_back(getUtf8String(ui->listImages->item(i)->text()));
    }
  }
  {
    for (int i = 0; i < ui->listVideos->count(); ++i)
    {
      result.ad.videos.push_back(getUtf8String(ui->listVideos->item(i)->text()));
    }
  }
  return result;
}

std::string PostAd::_formatMsg() const
{
  AdEntry ad = fromUi(ui);
  return ad.format();
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
    }
    // validate body.
    {
      const QString b = ui->editMessage->toPlainText();
      if (b.isEmpty())
      {
        errors.append("Ad body is empty.");
      }
    }
    // validate total length
    resultMsg = _formatMsg();
    if (_formatMsg().size() > AD_MAX_BYTES)
    {
      errors.append("Total bytes exceeds limit.");
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

QVariant extractId(const QComboBox& cbx, const int index, const int column = 0)
{
  QAbstractItemModel* const model = cbx.model();
  return model->data(model->index(index, column));
}

void PostAd::on_cbxCountry_currentIndexChanged(int index)
{
  _private->countryChanged(extractId(*ui->cbxCountry, index));
}

void PostAd::on_cbxState_currentIndexChanged(int index)
{
  _private->regionChanged(extractId(*ui->cbxState, index));
}

void PostAd::on_cbxCategory_currentIndexChanged(int index)
{
  _private->categoryChanged(extractId(*ui->cbxCategory, index));
}

void addItem(QListWidget* const w, const QString& text)
{
  QListWidgetItem* const item = new QListWidgetItem(text, w);
  item->setFlags(item->flags() | Qt::ItemIsEditable);
  w->insertItem(w->count(), item);
}

void removeCurrentItem(QListWidget* const w)
{
  QListWidgetItem* const item = w->currentItem(); 
  if (item)
  {
    delete item;
  }
}

void PostAd::on_btnVideoAdd_clicked()
{
  addItem(ui->listVideos, "http://");
  updateTextLength();
}

void PostAd::on_btnVideoRemove_clicked()
{
  removeCurrentItem(ui->listVideos);
  updateTextLength();
}

void PostAd::on_btnImageAdd_clicked()
{
  addItem(ui->listImages, "http://");
  updateTextLength();
}

void PostAd::on_btnImageRemove_clicked()
{
  removeCurrentItem(ui->listImages);
  updateTextLength();
}
