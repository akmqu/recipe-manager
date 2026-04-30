#include "RecipeDetailsPage.h"
#include "ui_RecipeDetailsPage.h"
#include "utils/RecipeImageStorage.h"
#include <QFile>
#include <QPixmap>
#include <QResizeEvent>

void RecipeDetailsPage::applyMetaHtml(const Recipe &recipe)
{
    const int total = recipe.prepTime + recipe.cookTime;
    QString totalStr;
    if (total <= 60)
        totalStr = QStringLiteral("%1 min").arg(total);
    else
        totalStr = QStringLiteral("%1 h %2 min").arg(total / 60).arg(total % 60);

    // Avoid "%" in HTML/CSS when using QString::arg — it is treated as a placeholder.
    const QString html = QStringLiteral(
                             "<div style=\"font-family:Segoe UI, sans-serif; color:#5c6b4a; font-size:13px; "
                             "line-height:1.55;\">"
                             "<div style=\"margin-bottom:10px;\">"
                             "<span style=\"color:#344f1f; font-weight:600;\">%1</span>"
                             "<span style=\"color:#d8e4c8; margin:0 10px;\">·</span>"
                             "<span style=\"color:#344f1f; font-weight:600;\">%2</span>"
                             "</div>"
                             "<div style=\"letter-spacing:0.01em;\">"
                             "Przygotowanie <b style=\"color:#2d3a24;\">%3 min</b>"
                             "<span style=\"color:#d8e4c8; margin:0 12px;\">·</span>"
                             "Gotowanie <b style=\"color:#2d3a24;\">%4 min</b>"
                             "<span style=\"color:#d8e4c8; margin:0 12px;\">·</span>"
                             "Razem <b style=\"color:#2d3a24;\">%5</b>"
                             "</div>"
                             "</div>")
                             .arg(recipe.category.toHtmlEscaped(),
                                  recipe.difficulty.toHtmlEscaped(),
                                  QString::number(recipe.prepTime),
                                  QString::number(recipe.cookTime),
                                  totalStr.toHtmlEscaped());

    ui->textBrowser_Meta->setHtml(html);
}

RecipeDetailsPage::RecipeDetailsPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::RecipeDetailsPage)
{
    ui->setupUi(this);

    ui->label_DetailImage->setMinimumSize(360, 220);
    ui->label_DetailImage->setMaximumSize(520, 340);
    ui->label_DetailImage->setAlignment(Qt::AlignCenter);
    ui->label_DetailImage->setScaledContents(false);

    connect(ui->pushButton_Back, &QPushButton::clicked, this, [this] { emit backClicked(); });

    connect(ui->pushButton_Edit, &QPushButton::clicked, this, [this] {
        if (m_recipeId > 0)
            emit editRequested(m_recipeId);
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
    m_recipeId = recipe.id;
    m_heroImagePath = recipe.imagePath.trimmed();

    ui->label_DetailTitle->setText(recipe.name);
    applyMetaHtml(recipe);

    ui->textBrowser_Ingredients->setPlainText(recipe.ingredients);
    ui->textBrowser_Notes->setPlainText(recipe.notes);

    updateHeroImage();
}

void RecipeDetailsPage::updateHeroImage()
{
    if (m_heroImagePath.isEmpty() || !QFile::exists(m_heroImagePath)) {
        ui->label_DetailImage->clear();
        ui->label_DetailImage->setText(QStringLiteral("Brak zdjęcia"));
        return;
    }

    QPixmap pm(m_heroImagePath);
    if (pm.isNull()) {
        ui->label_DetailImage->clear();
        ui->label_DetailImage->setText(QStringLiteral("Brak podglądu"));
        return;
    }

    ui->label_DetailImage->setText({});
    QSize box = ui->label_DetailImage->size();
    if (box.width() < 80 || box.height() < 80)
        box = ui->label_DetailImage->minimumSize();

    ui->label_DetailImage->setPixmap(
        RecipeImageStorage::coverRoundPixmap(pm, box, kHeroCornerRadius));
}
