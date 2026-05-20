#include "MealPlannerPage.h"
#include "ui_MealPlannerPage.h"
#include "database/DatabaseManager.h"

#include <QComboBox>
#include <QDebug>
#include <QSignalBlocker>
#include <QStyledItemDelegate>

namespace {
constexpr int kEmptyRecipeId = 0;
constexpr auto kEmptyLabel   = "-";
} // namespace

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

void MealPlannerPage::setupMealCombos()
{
    m_mealCombos = {
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

    fillRecipeLists();

    const QMap<int, int> savedPlan = DatabaseManager::instance().getMealPlan();
    applySavedPlan(savedPlan);

    m_loading = false;
}

void MealPlannerPage::fillRecipeLists()
{
    const QList<Recipe> recipes = DatabaseManager::instance().getAllRecipes();

    for (QComboBox *combo : m_mealCombos) {
        QSignalBlocker blocker(combo);
        combo->clear();
        combo->addItem(QString::fromUtf8(kEmptyLabel), kEmptyRecipeId);

        for (const Recipe &recipe : recipes)
            combo->addItem(recipe.name, recipe.id);

        combo->setCurrentIndex(0);
    }
}

void MealPlannerPage::applySavedPlan(const QMap<int, int> &savedPlan)
{
    for (int slot = 0; slot < m_mealCombos.size(); ++slot) {
        QComboBox *combo = m_mealCombos.at(slot);
        const int day  = slot / 4;
        const int meal = slot % 4;

        QSignalBlocker blocker(combo);
        combo->setCurrentIndex(0);

        const int recipeId = savedPlan.value(slotKey(day, meal), kEmptyRecipeId);
        if (recipeId > 0) {
            const int idx = combo->findData(recipeId);
            if (idx >= 0)
                combo->setCurrentIndex(idx);
        }
    }
}

void MealPlannerPage::saveComboSlot(int slotIndex)
{
    if (slotIndex < 0 || slotIndex >= m_mealCombos.size())
        return;

    const int day  = slotIndex / 4;
    const int meal = slotIndex % 4;
    const int recipeId = m_mealCombos.at(slotIndex)->currentData().toInt();

    if (!DatabaseManager::instance().setMealPlanEntry(day, meal, recipeId)) {
        qWarning() << "setMealPlanEntry failed:"
                   << DatabaseManager::instance().lastError();
    }
}

int MealPlannerPage::slotKey(int dayOfWeek, int mealType)
{
    return dayOfWeek * 4 + mealType;
}
