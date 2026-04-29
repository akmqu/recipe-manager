#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "database/DatabaseManager.h"
#include <QMessageBox>
#include <QDebug>
#include <QTimer>

namespace {
// QListWidget sidebar row indices (must match mainwindow.ui)
constexpr int kMenuAllRecipes = 1;
constexpr int kMenuFavorites = 2;
constexpr int kMenuMealPlanner = 4;
constexpr int kMenuShoppingList = 5;
constexpr int kMenuAddRecipe = 7;
constexpr int kMenuStatistics = 8;
constexpr int kNoMenuSync = -1;
} // namespace

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    showMaximized();

    if (!DatabaseManager::instance().connect()) {
        QMessageBox::critical(this, "Błąd bazy danych",
                              DatabaseManager::instance().lastError());
    }

    setupPages();
    setupNavigation();
    setupConnections();

    // Defer first load so the window is fully painted before hitting the DB
    QTimer::singleShot(0, this, [this]{ refreshRecipeList(); });
}

MainWindow::~MainWindow()
{
    delete ui;
}

// ─────────────────────────────────────────
//  Private setup
// ─────────────────────────────────────────

void MainWindow::setupPages()
{
    m_allRecipesPage    = new AllRecipesPage(this);
    m_addRecipePage     = new AddRecipePage(this);
    m_favoritesPage     = new FavoritesPage(this);
    m_shoppingListPage  = new ShoppingListPage(this);
    m_statisticsPage    = new StatisticsPage(this);
    m_mealPlannerPage   = new MealPlannerPage(this);
    m_recipeDetailsPage = new RecipeDetailsPage(this);

    ui->stackedWidget->addWidget(m_allRecipesPage);    // stack index 0
    ui->stackedWidget->addWidget(m_addRecipePage);   // 1
    ui->stackedWidget->addWidget(m_favoritesPage);    // 2
    ui->stackedWidget->addWidget(m_shoppingListPage); // 3
    ui->stackedWidget->addWidget(m_statisticsPage);  // 4
    ui->stackedWidget->addWidget(m_mealPlannerPage); // 5
    ui->stackedWidget->addWidget(m_recipeDetailsPage); // 6
}

void MainWindow::setupNavigation()
{
    connect(ui->listWidget, &QListWidget::currentRowChanged,
            this, [this](int row) {
        switch (row) {
        case kMenuAllRecipes:
            showPage(m_allRecipesPage, kMenuAllRecipes);
            break;
        case kMenuFavorites:
            showPage(m_favoritesPage, kMenuFavorites);
            break;
        case kMenuMealPlanner:
            showPage(m_mealPlannerPage, kMenuMealPlanner);
            break;
        case kMenuShoppingList:
            showPage(m_shoppingListPage, kMenuShoppingList);
            break;
        case kMenuAddRecipe:
            m_addRecipePage->clearForm();
            showPage(m_addRecipePage, kMenuAddRecipe);
            break;
        case kMenuStatistics:
            showPage(m_statisticsPage, kMenuStatistics);
            break;
        default:
            break;
        }
    });
}

void MainWindow::setupConnections()
{
    // AllRecipesPage
    connect(m_allRecipesPage, &AllRecipesPage::cardClicked,
            this, &MainWindow::onCardClicked);
    connect(m_allRecipesPage, &AllRecipesPage::favoriteClicked,
            this, &MainWindow::onFavoriteClicked);

    // AddRecipePage
    connect(m_addRecipePage, &AddRecipePage::recipeSaved,
            this, &MainWindow::onRecipeSaved);
    connect(m_addRecipePage, &AddRecipePage::cancelled, this, [this]{
        showPage(m_allRecipesPage, kMenuAllRecipes);
    });

    // RecipeDetailsPage
    connect(m_recipeDetailsPage, &RecipeDetailsPage::backClicked, this, [this]{
        showPage(m_allRecipesPage, kMenuAllRecipes);
    });

    connect(m_allRecipesPage, &AllRecipesPage::addClicked, this, [this] {
        showPage(m_addRecipePage, kMenuAddRecipe);
    });
}

// ─────────────────────────────────────────
//  Navigation helper
// ─────────────────────────────────────────

void MainWindow::showPage(QWidget* page, int menuRow)
{
    ui->stackedWidget->setCurrentWidget(page);

    if (menuRow >= 0) {
        QSignalBlocker blocker(ui->listWidget);
        ui->listWidget->setCurrentRow(menuRow);
    }
}

// ─────────────────────────────────────────
//  Private slots
// ─────────────────────────────────────────

void MainWindow::refreshRecipeList()
{
    const QList<Recipe> recipes = DatabaseManager::instance().getAllRecipes();
    qDebug() << "Recipes loaded:" << recipes.size();
    m_allRecipesPage->refreshRecipes(recipes);
}

void MainWindow::onRecipeSaved(const Recipe& recipe)
{
    // MainWindow owns all DB writes
    const bool ok = (recipe.id == -1)
        ? DatabaseManager::instance().addRecipe(recipe)
        : DatabaseManager::instance().updateRecipe(recipe);

    if (ok) {
        refreshRecipeList();
        showPage(m_allRecipesPage, kMenuAllRecipes);
    } else {
        QMessageBox::critical(this, "Błąd zapisu",
                              DatabaseManager::instance().lastError());
    }
}

void MainWindow::onFavoriteClicked(int id, bool isFavorite)
{
    if (!DatabaseManager::instance().setFavorite(id, isFavorite)) {
        qWarning() << "setFavorite failed for id=" << id
                   << ":" << DatabaseManager::instance().lastError();
    }
}

void MainWindow::onCardClicked(int id)
{
    const Recipe recipe = DatabaseManager::instance().getRecipeById(id);
    m_recipeDetailsPage->loadRecipe(recipe);
    showPage(m_recipeDetailsPage, kNoMenuSync);
}