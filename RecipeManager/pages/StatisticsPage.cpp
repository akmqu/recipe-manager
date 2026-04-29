#include "StatisticsPage.h"
#include "ui_StatisticsPage.h"

StatisticsPage::StatisticsPage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StatisticsPage)
{
    ui->setupUi(this);
}

StatisticsPage::~StatisticsPage()
{
    delete ui;
}