#include "recipecard.h"
#include "ui_recipecard.h"
#include "utils/RecipeImageStorage.h"
#include <QFile>
#include <QPixmap>
#include <QStyle>

RecipeCard::RecipeCard(const Recipe& recipe, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::RecipeCard)
    , m_id(recipe.id)
    , m_isFavorite(recipe.isFavorite)
{
    ui->setupUi(this);
    setAttribute(Qt::WA_StyledBackground, true);

    int totalTime = recipe.prepTime + recipe.cookTime;
    QString timeText;

    if (totalTime <= 60) {
        timeText = QString("🕒 %1 min").arg(totalTime);
    } else {
        int hours = totalTime / 60;
        int minutes = totalTime % 60;
        
        if (minutes == 0) {
            timeText = QString("🕒 %1 h").arg(hours);
        } else {
            timeText = QString("🕒 %1 h %2 min").arg(hours).arg(minutes);
        }
    }

    QString starsText;
    int maxStars = 5;

    for (int i = 0; i < recipe.rating; ++i) {
        starsText += QStringLiteral("★");
    }

    for (int i = recipe.rating; i < maxStars; ++i) {
        starsText += QStringLiteral("☆");
    }

    ui->label_rating->setText(starsText);

   

    if (!recipe.imagePath.isEmpty() && QFile::exists(recipe.imagePath)) {
        QPixmap pm(recipe.imagePath);
        if (!pm.isNull()) {
            QSize box = ui->label_image->size();
            if (box.width() < 20 || box.height() < 20)
                box = QSize(300, 200);
            constexpr int kCardImageRadius = 13;
            ui->label_image->setPixmap(
                RecipeImageStorage::coverTopRoundPixmap(pm, box, kCardImageRadius));
        } else {
            ui->label_image->setText(QStringLiteral("Brak zdjęcia"));
        }
    } else {
        ui->label_image->setText(QStringLiteral("Brak zdjęcia"));
    }
    
    ui->label_title->setText(recipe.name);
    ui->label_category->setText(recipe.category);
    ui->label_difficulty->setText(recipe.difficulty);
    ui->label_time->setText(timeText);

    QString diffTag = "easy";
    if (recipe.difficulty == "Średni") {
        diffTag = "medium";
    } else if (recipe.difficulty == "Trudny") {
        diffTag = "hard";
    }  

    ui->label_difficulty->setProperty("difficultyLevel", diffTag);
    ui->label_difficulty->style()->unpolish(ui->label_difficulty);
    ui->label_difficulty->style()->polish(ui->label_difficulty);
    updateFavoriteButton();

    connect(ui->pushButton_Favourite, &QPushButton::clicked, this, [this]() {
        m_isFavorite = !m_isFavorite;
        updateFavoriteButton();
        emit favoriteClicked(m_id, m_isFavorite);
    });
}

RecipeCard::~RecipeCard()
{
    delete ui;
}

void RecipeCard::mousePressEvent(QMouseEvent*)
{
    emit cardClicked(m_id);
}

void RecipeCard::updateFavoriteButton()
{
    if (m_isFavorite) {
        ui->pushButton_Favourite->setText("♥");
    } else {
        ui->pushButton_Favourite->setText("♡");
    }

    ui->pushButton_Favourite->setProperty("isFavorite", m_isFavorite);
    ui->pushButton_Favourite->style()->unpolish(ui->pushButton_Favourite);
    ui->pushButton_Favourite->style()->polish(ui->pushButton_Favourite);
}
