#include "StatisticsPage.h"
#include "ui_StatisticsPage.h"
#include "database/DatabaseManager.h"

#include <QColor>
#include <QLayout>
#include <QLayoutItem>
#include <QList>
#include <QMap>
#include <QPainter>
#include <QPen>
#include <QtMath>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QPieSeries>
#include <QtCharts/QPieSlice>

namespace {

void clearLayout(QLayout *layout)
{
    while (QLayoutItem *item = layout->takeAt(0)) {
        if (item->widget()) {
            item->widget()->deleteLater();
        }
        delete item;
    }
}

} 

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

QtCharts::QChartView *StatisticsPage::createPieChart(const QMap<QString, int> &data)
{
    auto *series = new QtCharts::QPieSeries();

    for (auto it = data.cbegin(); it != data.cend(); ++it) {
        if (it.value() > 0) {
            series->append(QStringLiteral("%1 (%2)").arg(it.key()).arg(it.value()), it.value());
        }
    }

    const QList<QColor> colors = {
        QColor(QStringLiteral("#d08a49")),
        QColor(QStringLiteral("#c96b5c")),
        QColor(QStringLiteral("#8fa77b")),
        QColor(QStringLiteral("#8a5a3c")),
        QColor(QStringLiteral("#d7c3a7"))
    };

    int colorIndex = 0;
    for (QtCharts::QPieSlice *slice : series->slices()) {
        const QColor color = colors.at(colorIndex % colors.size());
        slice->setBrush(color);
        slice->setPen(QPen(QColor(QStringLiteral("#ffffff")), 1));
        slice->setLabelVisible(false);
        colorIndex++;
    }

    auto *chart = new QtCharts::QChart();
    chart->addSeries(series);
    chart->legend()->setVisible(true);
    chart->legend()->setAlignment(Qt::AlignBottom);
    chart->legend()->setLabelColor(QColor(QStringLiteral("#3d2817")));
    chart->setBackgroundVisible(false);
    chart->setPlotAreaBackgroundVisible(false);
    chart->setMargins(QMargins(0, 0, 0, 0));

    auto *chartView = new QtCharts::QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);
    chartView->setMinimumHeight(220);

    return chartView;
}

void StatisticsPage::loadStatistics()
{
    const QList<Recipe> recipes = DatabaseManager::instance().getAllRecipes();
    const int totalCount = recipes.size();

    int favoriteCount = 0;
    int ratingSum = 0;
    int totalTimeSum = 0;

    bool foundFastest = false;
    int fastestTime = 0;
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

        if (totalTime > 0 && (!foundFastest || totalTime < fastestTime)) {
            foundFastest = true;
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
        ui->label_FastestRecipe->setText(QStringLiteral("—"));
        ui->label_LongestRecipe->setText(QStringLiteral("—"));
        ui->label_LatestRecipe->setText(QStringLiteral("—"));
    } else {
        if (!foundFastest) {
            ui->label_FastestRecipe->setText(QStringLiteral("—"));
        } else {
            ui->label_FastestRecipe->setText(
                QStringLiteral(" %1 (%2 min)")
                    .arg(fastestName)
                    .arg(fastestTime)
            );
        }

        ui->label_LongestRecipe->setText(
            QStringLiteral("%1 (%2 min)")
                .arg(longestName)
                .arg(longestTime)
        );

        ui->label_LatestRecipe->setText(latestName);
    }

    clearLayout(ui->categoryChartLayout);
    clearLayout(ui->difficultyChartLayout);

    ui->categoryChartLayout->addWidget(createPieChart(categoryCounts));
    ui->difficultyChartLayout->addWidget(createPieChart(difficultyCounts));
}
