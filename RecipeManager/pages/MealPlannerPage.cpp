#include "MealPlannerPage.h"
#include "ui_MealPlannerPage.h"
#include "database/DatabaseManager.h"

#include <QComboBox>
#include <QDebug>
#include <QStyledItemDelegate>

namespace {
constexpr int kMealsPerDay = 4;
constexpr int kEmptyRecipeId = 0; 
} 

MealPlannerPage::MealPlannerPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MealPlannerPage)
{
    ui->setupUi(this);
    setupMealCombos();
    loadMealPlan();
}

MealPlannerPage::~MealPlannerPage()
{
    delete ui;
}

void MealPlannerPage::setupMealCombos()//prepare and load saved plan
{
    m_mealCombos = {//store in list
        ui->comboBox, ui->comboBox_2, ui->comboBox_3, ui->comboBox_4,
        ui->comboBox_5, ui->comboBox_6, ui->comboBox_7, ui->comboBox_8,
        ui->comboBox_9, ui->comboBox_10, ui->comboBox_11, ui->comboBox_12,
        ui->comboBox_13, ui->comboBox_14, ui->comboBox_15, ui->comboBox_16,
        ui->comboBox_17, ui->comboBox_18, ui->comboBox_19, ui->comboBox_20,
        ui->comboBox_21, ui->comboBox_22, ui->comboBox_23, ui->comboBox_24,
        ui->comboBox_25, ui->comboBox_26, ui->comboBox_27, ui->comboBox_28
    };

    for (int i = 0; i < m_mealCombos.size(); ++i) {
        QComboBox *combo = m_mealCombos.at(i);
        combo->setItemDelegate(new QStyledItemDelegate(this));
        connect(combo, QOverload<int>::of(&QComboBox::currentIndexChanged),
                this, [this, i](int) {
            if (!m_loading)
                saveComboSlot(i);
        });
    }
}

void MealPlannerPage::loadMealPlan()
{
    m_loading = true;

    const QList<Recipe> recipes = DatabaseManager::instance().getAllRecipes();
    const QMap<int, int> savedPlan = DatabaseManager::instance().getMealPlan();

    // fill every combobox with recipes
    for (int slot = 0; slot < m_mealCombos.size(); ++slot) {
        QComboBox *combo = m_mealCombos.at(slot);

        combo->clear();
        combo->addItem("-", kEmptyRecipeId);

        for (const Recipe &recipe : recipes)
            combo->addItem(recipe.name, recipe.id);

        combo->setCurrentIndex(0);

        const int day = slot / kMealsPerDay;
        const int meal = slot % kMealsPerDay;
        const int recipeId = savedPlan.value(day * kMealsPerDay + meal, kEmptyRecipeId);
        if (recipeId > 0) {
            const int index = combo->findData(recipeId);
            if (index >= 0)
                combo->setCurrentIndex(index);
        }
    }

    m_loading = false;
}

void MealPlannerPage::saveComboSlot(int slotIndex)
{
    if (slotIndex < 0 || slotIndex >= m_mealCombos.size())
        return;
    
    // convert combobox position to day and meal type
    const int day = slotIndex / kMealsPerDay;
    const int meal = slotIndex % kMealsPerDay;
    const int recipeId = m_mealCombos.at(slotIndex)->currentData().toInt(); // get id of selected recipe

    if (!DatabaseManager::instance().setMealPlanEntry(day, meal, recipeId)) {
        qWarning() << "setMealPlanEntry failed:"
                   << DatabaseManager::instance().lastError();
    }
}
