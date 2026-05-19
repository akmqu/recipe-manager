#include "StatisticsPage.h"
#include "ui_StatisticsPage.h"
#include "database/DatabaseManager.h"

#include <QList>
#include <QMap>
#include <QStringList>
#include <QtMath>
#include <limits>

namespace {

QString formatCountLines(const QMap<QString, int> &counts,
                         const QStringList &preferredOrder = {})
{
    if (counts.isEmpty()) {
        return QStringLiteral("Brak danych");
    }

    QStringList lines;

    for (const QString &key : preferredOrder) {
        if (counts.contains(key)) {
            lines << QStringLiteral("%1: %2").arg(key).arg(counts.value(key));
        }
    }

    for (auto it = counts.cbegin(); it != counts.cend(); ++it) {
        if (!preferredOrder.contains(it.key())) {
            lines << QStringLiteral("%1: %2").arg(it.key()).arg(it.value());
        }
    }

    return lines.join(QLatin1Char('\n'));
}

} // namespace

StatisticsPage::StatisticsPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::StatisticsPage)
{
    ui->setupUi(this);
    loadStatistics();
}

StatisticsPage::~StatisticsPage()
{
    delete ui;
}

void StatisticsPage::loadStatistics()
{
    const QList<Recipe> recipes = DatabaseManager::instance().getAllRecipes();
    const int totalCount = recipes.size();

    int favoriteCount = 0;
    int ratingSum = 0;
    int totalTimeSum = 0;

    int fastestTime = std::numeric_limits<int>::max();
    int longestTime = 0;
    int latestId = -1;

    QString fastestName;
    QString longestName;
    QString latestName;

    QMap<QString, int> categoryCounts;
    QMap<QString, int> difficultyCounts;

    for (const Recipe &recipe : recipes) {
        if (recipe.isFavorite) {
            favoriteCount++;
        }

        ratingSum += recipe.rating;

        const int totalTime = recipe.prepTime + recipe.cookTime;
        totalTimeSum += totalTime;

        if (totalTime > 0 && totalTime < fastestTime) {
            fastestTime = totalTime;
            fastestName = recipe.name;
        }

        if (totalTime > longestTime) {
            longestTime = totalTime;
            longestName = recipe.name;
        }

        if (recipe.id > latestId) {
            latestId = recipe.id;
            latestName = recipe.name;
        }

        QString category = recipe.category.trimmed();
        if (category.isEmpty()) {
            category = QStringLiteral("Nie podano");
        }

        QString difficulty = recipe.difficulty.trimmed();
        if (difficulty.isEmpty()) {
            difficulty = QStringLiteral("Nie podano");
        }

        categoryCounts[category]++;
        difficultyCounts[difficulty]++;
    }

    const double averageRating = totalCount > 0
        ? static_cast<double>(ratingSum) / totalCount
        : 0.0;

    const double averageTotalTime = totalCount > 0
        ? static_cast<double>(totalTimeSum) / totalCount
        : 0.0;

    ui->label_TotalCountValue->setText(QString::number(totalCount));
    ui->label_FavoriteCountValue->setText(QString::number(favoriteCount));

    ui->label_AverageRatingValue->setText(
        QStringLiteral("%1 ★").arg(QString::number(averageRating, 'f', 1))
    );

    ui->label_AverageTimeValue->setText(
        QStringLiteral("%1 min").arg(qRound(averageTotalTime))
    );

    if (totalCount == 0) {
        ui->label_FastestRecipe->setText(QStringLiteral("Najszybszy: —"));
        ui->label_LongestRecipe->setText(QStringLiteral("Najdłuższy: —"));
        ui->label_LatestRecipe->setText(QStringLiteral("Ostatnio dodano: —"));

        ui->label_CategoryStats->setText(QStringLiteral("Brak danych"));
        ui->label_DifficultyStats->setText(QStringLiteral("Brak danych"));

        return;
    }

    if (fastestTime == std::numeric_limits<int>::max()) {
        ui->label_FastestRecipe->setText(QStringLiteral("Najszybszy: —"));
    } else {
        ui->label_FastestRecipe->setText(
            QStringLiteral("Najszybszy: %1 (%2 min)")
                .arg(fastestName)
                .arg(fastestTime)
        );
    }

    ui->label_LongestRecipe->setText(
        QStringLiteral("Najdłuższy: %1 (%2 min)")
            .arg(longestName)
            .arg(longestTime)
    );

    ui->label_LatestRecipe->setText(
        QStringLiteral("Ostatnio dodano: %1").arg(latestName)
    );

    ui->label_CategoryStats->setText(
        formatCountLines(categoryCounts)
    );

    const QStringList difficultyOrder = {
        QStringLiteral("Łatwy"),
        QStringLiteral("Średni"),
        QStringLiteral("Trudny")
    };

    ui->label_DifficultyStats->setText(
        formatCountLines(difficultyCounts, difficultyOrder)
    );
}