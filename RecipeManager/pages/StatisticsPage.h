#ifndef STATISTICSPAGE_H
#define STATISTICSPAGE_H

#include <QWidget>

namespace Ui { class StatisticsPage; }
namespace QtCharts {
class QChart;
class QChartView;
}

class StatisticsPage : public QWidget
{
    Q_OBJECT

public:
    explicit StatisticsPage(QWidget *parent = nullptr);
    ~StatisticsPage();

public slots:
    void loadStatistics();

private:
    Ui::StatisticsPage *ui;

    QtCharts::QChartView *m_categoryChartView  = nullptr;
    QtCharts::QChartView *m_difficultyChartView = nullptr;
    QtCharts::QChart     *m_categoryChart       = nullptr;
    QtCharts::QChart     *m_difficultyChart     = nullptr;

    void setupCharts();
};

#endif // STATISTICSPAGE_H