#include "FavoritesPage.h"
#include "ui_FavoritesPage.h"

FavoritesPage::FavoritesPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FavoritesPage)
{
    ui->setupUi(this);
}

FavoritesPage::~FavoritesPage()
{
    delete ui;
}