#include "ShoppingListPage.h"
#include "ui_ShoppingListPage.h"

ShoppingListPage::ShoppingListPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ShoppingListPage)
{
    ui->setupUi(this);
}

ShoppingListPage::~ShoppingListPage()
{
    delete ui;
}