#include "RecipeDetailsPage.h"
#include "ui_RecipeDetailsPage.h"
#include "utils/RecipeImageStorage.h"
#include <QFile>
#include <QPixmap>
#include <QResizeEvent>
#include <QStyle>

RecipeDetailsPage::RecipeDetailsPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::RecipeDetailsPage)
{
    ui->setupUi(this);

    connect(ui->pushButton_Back, &QPushButton::clicked,
            this, [this] { emit backClicked(); });

    connect(ui->pushButton_Edit, &QPushButton::clicked, this, [this] {
        if (m_recipeId > 0)
            emit editRequested(m_recipeId);
    });

    connect(ui->pushButton_Delete, &QPushButton::clicked, this, [this] {
    if (m_recipeId > 0)
        emit deleteRequested(m_recipeId);
});

}

RecipeDetailsPage::~RecipeDetailsPage()
{
    delete ui;
}

void RecipeDetailsPage::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    updateHeroImage();
}

void RecipeDetailsPage::loadRecipe(const Recipe &recipe)
{
    m_recipeId       = recipe.id;
    m_heroImagePath  = recipe.imagePath.trimmed();

    ui->label_DetailTitle->setText(recipe.name);

    ui->label_MetaCategory->setText(recipe.category);

    ui->label_MetaDifficulty->setText(recipe.difficulty);
    const QString lvl = recipe.difficulty.toLower(); // "łatwy" / "średni" / "trudny"
    ui->label_MetaDifficulty->setProperty("difficultyLevel", lvl);
    ui->label_MetaDifficulty->style()->unpolish(ui->label_MetaDifficulty);
    ui->label_MetaDifficulty->style()->polish(ui->label_MetaDifficulty);

    QString prepStr = recipe.prepTime > 0 ? QString("%1 min").arg(recipe.prepTime) : "Brak";
    QString cookStr = recipe.cookTime > 0 ? QString("%1 min").arg(recipe.cookTime) : "Brak";
    
    ui->label_MetaPrepTime->setText(prepStr);
    
    ui->label_MetaCockTime->setText(cookStr);

    QString ingredients = recipe.ingredients;
    QString notes = recipe.notes;
    ui->textBrowser_Ingredients->setPlainText(ingredients.replace("\\n", "\n"));
    ui->textBrowser_Notes->setPlainText(notes.replace("\\n", "\n"));

    QString starsText;
    bool hasRating = (recipe.rating > 0); 

    if (!hasRating) {
        starsText = QStringLiteral("Brak");
    } else {
        for (int i = 0; i < recipe.rating; ++i) {
            starsText += QStringLiteral("★");
        }
    }
    ui->label_MetaRating->setText(starsText);

    ui->label_MetaRating->setProperty("hasRating", hasRating);
    ui->label_MetaRating->style()->unpolish(ui->label_MetaRating);
    ui->label_MetaRating->style()->polish(ui->label_MetaRating);

    updateHeroImage();
}

void RecipeDetailsPage::updateHeroImage()
{
    bool hasValidImg = !m_heroImagePath.isEmpty() && QFile::exists(m_heroImagePath);
    QPixmap pm;
    if (hasValidImg) {
        pm.load(m_heroImagePath);
        if (pm.isNull()) hasValidImg = false;
    }

    ui->label_DetailImage->setProperty("hasImg", hasValidImg);
    ui->label_DetailImage->style()->unpolish(ui->label_DetailImage);
    ui->label_DetailImage->style()->polish(ui->label_DetailImage);

    if (!hasValidImg) {
        ui->label_DetailImage->clear();
        ui->label_DetailImage->setText(m_heroImagePath.isEmpty() ? QStringLiteral("Brak zdjęcia") : QStringLiteral("Brak podglądu"));
        return;
    }

    ui->label_DetailImage->setText({});
    QSize box = ui->label_DetailImage->size();
    if (box.width() < 80 || box.height() < 80)
        box = QSize(ui->label_DetailImage->minimumWidth(), ui->label_DetailImage->minimumHeight());

    ui->label_DetailImage->setPixmap(
        RecipeImageStorage::coverRoundPixmap(pm, box, kHeroCornerRadius));
}