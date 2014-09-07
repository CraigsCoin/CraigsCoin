#include "showad.h"
#include "ui_showad.h"
#include "adstablemodel.h"

#include <QAbstractItemModel>
#include <QDesktopWidget>

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


void ShowAd::show(const QModelIndex& index)
{
  const QAbstractItemModel* const model = index.model();
  Q_ASSERT(model);
  ui->labelTitle->setText(model->data(
        model->index(index.row(), AdsTableModel::Title)).toString());
  ui->labelDate->setText(model->data(
        model->index(index.row(), AdsTableModel::Date)).toString());
  ui->labelBody->setText(model->data(
        model->index(index.row(), AdsTableModel::Body)).toString());
  setWindowFlags(Qt::Drawer);
  //updateGeometry();
  //QRect rect = frameGeometry();
  //rect.moveCenter(QDesktopWidget().availableGeometry().center());
  //move(rect.topLeft());
  setWindowTitle(ui->labelTitle->text());
  setAttribute(Qt::WA_DeleteOnClose, true);
  QDialog::show();
}
