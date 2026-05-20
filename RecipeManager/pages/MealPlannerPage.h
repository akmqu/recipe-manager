#ifndef MEALPLANNERPAGE_H
#define MEALPLANNERPAGE_H

#include <QVector>
#include <QWidget>
#include <QMap>

class QComboBox;

namespace Ui { class MealPlannerPage; }

class MealPlannerPage : public QWidget
{
    Q_OBJECT

public:
    explicit MealPlannerPage(QWidget *parent = nullptr);
    ~MealPlannerPage();

public slots:
    void loadMealPlan();

private:
    Ui::MealPlannerPage *ui;

    QVector<QComboBox *> m_mealCombos;
    bool m_loading = false;

    void setupMealCombos();
    void fillRecipeLists();
    void applySavedPlan(const QMap<int, int> &savedPlan);
    void saveComboSlot(int slotIndex);
    static int slotKey(int dayOfWeek, int mealType);
};

#endif // MEALPLANNERPAGE_H
