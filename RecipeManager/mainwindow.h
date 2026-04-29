#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "models/Recipe.h"
#include "pages/AllRecipesPage.h"
#include "pages/AddRecipePage.h"
#include "pages/FavoritesPage.h"
#include "pages/ShoppingListPage.h"
#include "pages/StatisticsPage.h"
#include "pages/MealPlannerPage.h"
#include "pages/RecipeDetailsPage.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onCardClicked(int id);
    void onFavoriteClicked(int id, bool isFavorite);
    void onRecipeSaved(const Recipe& recipe);
    void refreshRecipeList();

private:
    Ui::MainWindow *ui;

    AllRecipesPage*    m_allRecipesPage;
    AddRecipePage*     m_addRecipePage;
    FavoritesPage*     m_favoritesPage;
    ShoppingListPage*  m_shoppingListPage;
    StatisticsPage*    m_statisticsPage;
    MealPlannerPage*   m_mealPlannerPage;
    RecipeDetailsPage* m_recipeDetailsPage;

    void setupPages();
    void setupNavigation();
    void setupConnections();
    void showPage(QWidget* page, int menuRow = -1);
};

#endif // MAINWINDOW_H