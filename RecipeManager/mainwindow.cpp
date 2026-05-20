#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "database/DatabaseManager.h"
#include "utils/RecipeImageStorage.h"
#include <QMessageBox>
#include <QDebug>
#include <QTimer>

namespace {
// QListWidget sidebar row indices 
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
    this->resize(1280, 720);

    if (!DatabaseManager::instance().connect()) {
        QMessageBox::critical(this, "Błąd bazy danych", DatabaseManager::instance().lastError());
        ui->listWidget->setEnabled(false);
        ui->stackedWidget->setEnabled(false);
        return;
    }

    setupPages();
    setupNavigation();
    setupConnections();

    QTimer::singleShot(0, this, [this]{ refreshRecipeList(); });
}

MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::setupPages()
{
    m_allRecipesPage    = new AllRecipesPage(this);
    m_addRecipePage     = new AddRecipePage(this);
    m_favoritesPage     = new FavoritesPage(this);
    m_shoppingListPage  = new ShoppingListPage(this);
    m_statisticsPage    = new StatisticsPage(this);
    m_mealPlannerPage   = new MealPlannerPage(this);
    m_recipeDetailsPage = new RecipeDetailsPage(this);
    

    ui->stackedWidget->addWidget(m_allRecipesPage);    
    ui->stackedWidget->addWidget(m_addRecipePage);  
    ui->stackedWidget->addWidget(m_favoritesPage);   
    ui->stackedWidget->addWidget(m_shoppingListPage);
    ui->stackedWidget->addWidget(m_statisticsPage); 
    ui->stackedWidget->addWidget(m_mealPlannerPage); 
    ui->stackedWidget->addWidget(m_recipeDetailsPage); 
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
            m_mealPlannerPage->loadMealPlan();
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
            m_statisticsPage->loadStatistics();
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
    connect(m_favoritesPage, &FavoritesPage::cardClicked,
            this, &MainWindow::onCardClicked);
    connect(m_favoritesPage, &FavoritesPage::favoriteClicked,
            this, &MainWindow::onFavoriteClicked);

    // AddRecipePage
    connect(m_addRecipePage, &AddRecipePage::recipeSaved,
            this, &MainWindow::onRecipeSaved);
    connect(m_addRecipePage, &AddRecipePage::cancelled, this, [this](int previousEditRecipeId) {
        if (previousEditRecipeId > 0) {
            //if in edit mode
            const Recipe r = DatabaseManager::instance().getRecipeById(previousEditRecipeId);
            m_recipeDetailsPage->loadRecipe(r);
            showPage(m_recipeDetailsPage, kNoMenuSync);
        } else {
            showPage(m_allRecipesPage, kMenuAllRecipes);
        }
    });

    // RecipeDetailsPage
    connect(m_recipeDetailsPage, &RecipeDetailsPage::backClicked, this, [this]{
        showPage(m_allRecipesPage, kMenuAllRecipes);
    });
    connect(m_recipeDetailsPage, &RecipeDetailsPage::editRequested,
            this, &MainWindow::onEditRecipeRequested);

    connect(m_allRecipesPage, &AllRecipesPage::addClicked, this, [this] {
        showPage(m_addRecipePage, kMenuAddRecipe);
    });

    connect(m_recipeDetailsPage, &RecipeDetailsPage::deleteRequested,
        this, [this](int recipeId) {
    auto reply = QMessageBox::question(
        this,
        QStringLiteral("Usuń przepis"),
        QStringLiteral("Czy na pewno chcesz usunąć ten przepis? Tej operacji nie można cofnąć."),
        QMessageBox::Yes | QMessageBox::No,
        QMessageBox::No);

    if (reply != QMessageBox::Yes)
        return;

    Recipe r;
    r.id = recipeId;
    if (!DatabaseManager::instance().deleteRecipe(r)) {
        QMessageBox::critical(this, QStringLiteral("Błąd"),
                              DatabaseManager::instance().lastError());
        return;
    }

    refreshRecipeList();
    showPage(m_allRecipesPage, kMenuAllRecipes);
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
    m_favoritesPage->refreshRecipes(recipes);
}

void MainWindow::onRecipeSaved(const Recipe &recipeIn)
{
    const bool wasEditingExisting = recipeIn.id > 0; //> 0 edit mode
    Recipe recipe = recipeIn;
    const QString srcImage = recipe.imagePath.trimmed();

    if (recipe.id <= 0) { //new recipe
        recipe.imagePath.clear();
        recipe.isFavorite = false;
        if (!DatabaseManager::instance().addRecipe(recipe)) {
            QMessageBox::critical(this, QStringLiteral("Błąd zapisu"),DatabaseManager::instance().lastError());
            return;
        }

        const QString stored = RecipeImageStorage::importForRecipe(recipe.id, srcImage); //copy image to app folder and return new path
        if (!srcImage.isEmpty() && stored.isEmpty()) {
            QMessageBox::warning(this, QStringLiteral("Obraz"),
                                 QStringLiteral("Nie udało się skopiować zdjęcia - przepis zapisany bez obrazu."));
        }
        if (!DatabaseManager::instance().setRecipeImagePath(recipe.id, stored)) {
            QMessageBox::critical(this, QStringLiteral("Błąd zapisu"), DatabaseManager::instance().lastError());
            return; //try to save path to db
        }
    } else { //edit recipe
        QString imgForDb;
        if (srcImage.isEmpty()) {
            imgForDb.clear();
        } else {
            imgForDb = RecipeImageStorage::importForRecipe(recipe.id, srcImage);
            if (imgForDb.isEmpty()) {
                if (RecipeImageStorage::isManagedPath(srcImage))
                    imgForDb = srcImage;
                else {
                    QMessageBox::warning(this, QStringLiteral("Obraz"),
                                         QStringLiteral("Nie udało się zapisać pliku obrazu."));
                    return;
                }
            }
        }
        recipe.imagePath = imgForDb; //write new final path to recipe
        if (!DatabaseManager::instance().updateRecipe(recipe)) {
            QMessageBox::critical(this, QStringLiteral("Błąd zapisu"),
                                  DatabaseManager::instance().lastError());
            return;
        }
    }

    refreshRecipeList();
    m_addRecipePage->clearForm();

    if (wasEditingExisting) {
        //edit(get new updatedd recipe from db and show new details)
        const Recipe updated = DatabaseManager::instance().getRecipeById(recipe.id);
        m_recipeDetailsPage->loadRecipe(updated);
        showPage(m_recipeDetailsPage, kNoMenuSync);
    } else {
        showPage(m_allRecipesPage, kMenuAllRecipes);
    }
}

void MainWindow::onEditRecipeRequested(int recipeId)
{
    const Recipe r = DatabaseManager::instance().getRecipeById(recipeId);
    if (r.id <= 0)
        return;
    m_addRecipePage->loadForEdit(r);
    showPage(m_addRecipePage, kNoMenuSync);
}

void MainWindow::onFavoriteClicked(int id, bool isFavorite)
{
    if (!DatabaseManager::instance().setFavorite(id, isFavorite)) {
        qWarning() << "setFavorite failed for id=" << id
                   << ":" << DatabaseManager::instance().lastError();
        return;
    }
    refreshRecipeList();
}

void MainWindow::onCardClicked(int id)
{
    const Recipe recipe = DatabaseManager::instance().getRecipeById(id);
    m_recipeDetailsPage->loadRecipe(recipe);
    showPage(m_recipeDetailsPage, kNoMenuSync);
}
