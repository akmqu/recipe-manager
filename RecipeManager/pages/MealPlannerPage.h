#ifndef MEALPLANNERPAGE_H
#define MEALPLANNERPAGE_H

#include <QWidget>

namespace Ui { class MealPlannerPage; }

class MealPlannerPage : public QWidget
{
    Q_OBJECT

public:
    explicit MealPlannerPage(QWidget *parent = nullptr);
    ~MealPlannerPage();

private:
    Ui::MealPlannerPage *ui;
};

#endif // MEALPLANNERPAGE_H