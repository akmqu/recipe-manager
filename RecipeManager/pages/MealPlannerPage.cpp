#include "MealPlannerPage.h"
#include "ui_MealPlannerPage.h"

MealPlannerPage::MealPlannerPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MealPlannerPage)
{
    ui->setupUi(this);
}

MealPlannerPage::~MealPlannerPage()
{
    delete ui;
}