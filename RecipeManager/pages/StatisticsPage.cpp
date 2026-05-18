#include "StatisticsPage.h"
#include "ui_StatisticsPage.h"
#include "database/DatabaseManager.h"

#include <QtCharts/QAbstractAxis>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QChart>
#include <QtCharts/QChartView>
#include <QtCharts/QLegend>
#include <QtCharts/QPieSeries>
#include <QtCharts/QValueAxis>

#include <QList>
#include <QMap>
#include <QPainter>
#include <QStringList>
#include <QVBoxLayout>
#include <QtMath>
#include <limits>

QT_CHARTS_USE_NAMESPACE

StatisticsPage::StatisticsPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::StatisticsPage)
{
    ui->setupUi(this);
    setupCharts();
    loadStatistics();
}

StatisticsPage::~StatisticsPage()
{
    delete ui;
}

// ─────────────────────────────────────────
//  One-time chart setup
// ─────────────────────────────────────────

void StatisticsPage::setupCharts()
{
    m_categoryChart  = new QChart();
    m_difficultyChart = new QChart();

    m_categoryChartView = new QChartView(m_categoryChart,
                                         ui->widget_CategoryChartPlaceholder);
    m_categoryChartView->setRenderHint(QPainter::Antialiasing);

    m_difficultyChartView = new QChartView(m_difficultyChart,
                                           ui->widget_DifficultyChartPlaceholder);
    m_difficultyChartView->setRenderHint(QPainter::Antialiasing);

    auto *catLayout = new QVBoxLayout(ui->widget_CategoryChartPlaceholder);
    catLayout->setContentsMargins(0, 0, 0, 0);
    catLayout->addWidget(m_categoryChartView);

    auto *diffLayout = new QVBoxLayout(ui->widget_DifficultyChartPlaceholder);
    diffLayout->setContentsMargins(0, 0, 0, 0);
    diffLayout->addWidget(m_difficultyChartView);
}

// ─────────────────────────────────────────
//  Load / refresh all statistics
// ─────────────────────────────────────────

void StatisticsPage::loadStatistics()
{
    const QList<Recipe> recipes = DatabaseManager::instance().getAllRecipes();
    const int totalCount = recipes.size();

    // ── Accumulators ──────────────────────────────────────────
    int favoriteCount = 0;
    int ratingSum     = 0;
    int totalTimeSum  = 0;

    int     fastestTime = std::numeric_limits<int>::max();
    int     longestTime = 0;
    int     latestId    = -1;

    QString fastestName;
    QString longestName;
    QString latestName;

    QMap<QString, int> categoryCounts;
    QMap<QString, int> difficultyCounts;

    // ── Single pass ───────────────────────────────────────────
    for (const Recipe &r : recipes) {
        if (r.isFavorite)
            ++favoriteCount;

        ratingSum    += r.rating;
        const int tt  = r.prepTime + r.cookTime;
        totalTimeSum += tt;

        // Fastest: skip recipes with zero total time
        if (tt > 0 && tt < fastestTime) {
            fastestTime = tt;
            fastestName = r.name;
        }

        // Longest
        if (tt > longestTime) {
            longestTime = tt;
            longestName = r.name;
        }

        // Latest: highest id = most recently inserted
        if (r.id > latestId) {
            latestId   = r.id;
            latestName = r.name;
        }

        const QString cat  = r.category.trimmed().isEmpty()
                                 ? QStringLiteral("Nie podano")
                                 : r.category.trimmed();
        const QString diff = r.difficulty.trimmed().isEmpty()
                                 ? QStringLiteral("Nie podano")
                                 : r.difficulty.trimmed();

        ++categoryCounts[cat];
        ++difficultyCounts[diff];
    }

    // ── Derived values ────────────────────────────────────────
    const double averageRating    = totalCount > 0
        ? static_cast<double>(ratingSum) / totalCount : 0.0;
    const double averageTotalTime = totalCount > 0
        ? static_cast<double>(totalTimeSum) / totalCount : 0.0;

    // ── Update summary tiles ──────────────────────────────────
    ui->label_TotalCountValue->setText(QString::number(totalCount));
    ui->label_FavoriteCountValue->setText(QString::number(favoriteCount));
    ui->label_AverageRatingValue->setText(
        QStringLiteral("%1 ★").arg(QString::number(averageRating, 'f', 1)));
    ui->label_AverageTimeValue->setText(
        QStringLiteral("%1 min").arg(qRound(averageTotalTime)));

    if (totalCount == 0) {
        ui->label_FastestRecipe->setText(QStringLiteral("Najszybszy: —"));
        ui->label_LongestRecipe->setText(QStringLiteral("Najdłuższy: —"));
        ui->label_LatestRecipe->setText(QStringLiteral("Ostatnio dodano: —"));
    } else {
        const QString fastText = (fastestTime == std::numeric_limits<int>::max())
            ? QStringLiteral("Najszybszy: —")
            : QStringLiteral("Najszybszy: %1 (%2 min)").arg(fastestName).arg(fastestTime);

        ui->label_FastestRecipe->setText(fastText);
        ui->label_LongestRecipe->setText(
            QStringLiteral("Najdłuższy: %1 (%2 min)").arg(longestName).arg(longestTime));
        ui->label_LatestRecipe->setText(
            QStringLiteral("Ostatnio dodano: %1").arg(latestName));
    }

    // ── Pie chart: categories ─────────────────────────────────
    m_categoryChart->removeAllSeries();

    auto *pieSeries = new QPieSeries();
    for (auto it = categoryCounts.cbegin(); it != categoryCounts.cend(); ++it)
        pieSeries->append(it.key(), it.value());
    pieSeries->setLabelsVisible(true);

    m_categoryChart->addSeries(pieSeries);
    m_categoryChart->setTitle(QStringLiteral("Przepisy według kategorii"));
    m_categoryChart->legend()->setAlignment(Qt::AlignBottom);
    m_categoryChart->setBackgroundVisible(false);

    // ── Bar chart: difficulty ─────────────────────────────────
    m_difficultyChart->removeAllSeries();
    const auto oldAxes = m_difficultyChart->axes();
    for (QAbstractAxis *axis : oldAxes) {
        m_difficultyChart->removeAxis(axis);
        delete axis;
    }

    auto *difficultySet = new QBarSet(QStringLiteral("Liczba przepisów"));
    QStringList difficultyCategories;

    // Preferred order first, then anything else
    const QStringList preferredOrder = {
        QStringLiteral("Łatwy"),
        QStringLiteral("Średni"),
        QStringLiteral("Trudny")
    };
    for (const QString &d : preferredOrder) {
        if (difficultyCounts.contains(d)) {
            difficultyCategories << d;
            *difficultySet << difficultyCounts.value(d);
        }
    }
    for (auto it = difficultyCounts.cbegin(); it != difficultyCounts.cend(); ++it) {
        if (!difficultyCategories.contains(it.key())) {
            difficultyCategories << it.key();
            *difficultySet << it.value();
        }
    }

    auto *barSeries = new QBarSeries();
    barSeries->append(difficultySet);
    m_difficultyChart->addSeries(barSeries);
    m_difficultyChart->setTitle(QStringLiteral("Przepisy według trudności"));
    m_difficultyChart->setBackgroundVisible(false);
    m_difficultyChart->legend()->setVisible(false);

    auto *axisX = new QBarCategoryAxis();
    axisX->append(difficultyCategories);
    m_difficultyChart->addAxis(axisX, Qt::AlignBottom);
    barSeries->attachAxis(axisX);

    auto *axisY = new QValueAxis();
    axisY->setLabelFormat(QStringLiteral("%d"));
    int maxCount = 0;
    for (auto it = difficultyCounts.cbegin(); it != difficultyCounts.cend(); ++it)
        maxCount = qMax(maxCount, it.value());
    axisY->setRange(0, qMax(1, maxCount));
    m_difficultyChart->addAxis(axisY, Qt::AlignLeft);
    barSeries->attachAxis(axisY);
}