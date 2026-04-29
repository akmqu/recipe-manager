#include "RecipeDetailsPage.h"
#include "ui_RecipeDetailsPage.h"

RecipeDetailsPage::RecipeDetailsPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::RecipeDetailsPage)
{
    ui->setupUi(this);

    connect(ui->pushButton_Back, &QPushButton::clicked,
            this, [this]{ emit backClicked(); });
}

RecipeDetailsPage::~RecipeDetailsPage()
{
    delete ui;
}

void RecipeDetailsPage::loadRecipe(const Recipe& recipe)
{
    ui->label_DetailTitle->setText(recipe.name);
    ui->label_DetailInfo->setText(
        QString("%1  |  %2  |  ⏱ %3 min")
            .arg(recipe.category, recipe.difficulty)
            .arg(recipe.prepTime + recipe.cookTime));

    ui->textBrowser_Ingredients->setText("Składniki:\n" + recipe.ingredients);
    ui->textBrowser_Notes->setText("Instrukcje:\n" + recipe.notes);

    if (!recipe.imagePath.isEmpty()) {
        QPixmap pixmap(recipe.imagePath);
        if (!pixmap.isNull()) {
            ui->label_DetailImage->setPixmap(
                pixmap.scaled(ui->label_DetailImage->size(),
                              Qt::KeepAspectRatio,
                              Qt::SmoothTransformation));
        } else {
            ui->label_DetailImage->clear();
        }
    } else {
        ui->label_DetailImage->clear();
    }
}