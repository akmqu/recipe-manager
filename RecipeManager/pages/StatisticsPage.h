#ifndef STATISTICSPAGE_H
#define STATISTICSPAGE_H

#include <QMap>
#include <QString>
#include <QWidget>

namespace Ui {
class StatisticsPage;
}

namespace QtCharts {
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
    QtCharts::QChartView *createPieChart(const QMap<QString, int> &data);

    Ui::StatisticsPage *ui;
};

#endif
