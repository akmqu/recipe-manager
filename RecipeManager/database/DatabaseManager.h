#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QMap>
#include <QList>
#include "models/Recipe.h"
#include "models/ShoppingItem.h"


class DatabaseManager
{
public:
    static DatabaseManager &instance();

    bool connect();
    bool isConnected() const;
    
    QString lastError() const;

    /** New recipe: sets recipe.id from DB on success. */
    bool addRecipe(Recipe &recipe);
    bool updateRecipe(const Recipe &recipe);
    bool deleteRecipe(const Recipe &recipe);
    bool setFavorite(int id, bool isFavorite);
    bool setRecipeImagePath(int id, const QString &path);

    QList<Recipe> getAllRecipes();
    QList<Recipe> getFavorites();
    Recipe getRecipeById(int id);

        // ── Shopping list ─────────────────────────
    /** Inserts a new item; sets item.id on success. */
    bool addShoppingItem(ShoppingItem &item);
    bool setShoppingItemBought(int id, bool bought);
    bool deleteShoppingItem(int id);
    bool deleteBoughtShoppingItems();
    QList<ShoppingItem> getAllShoppingItems();

    // ── Meal planner ────────────────────────────
    /** Keys: day_of_week * 4 + meal_type (0–3). Values: recipe_id (> 0). */
    QMap<int, int> getMealPlan();
    bool setMealPlanEntry(int dayOfWeek, int mealType, int recipeId);

private:
    DatabaseManager() = default;
    
    bool initSchema();

    void mapRecipeRow(const QSqlQuery &query, Recipe &r) const;

    QSqlDatabase m_db;
    QString m_lastError;
};

#endif // DATABASEMANAGER_H
