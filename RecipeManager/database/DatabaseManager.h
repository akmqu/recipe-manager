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

    bool addRecipe(Recipe &recipe);
    bool updateRecipe(const Recipe &recipe);
    bool deleteRecipe(const Recipe &recipe);
    bool setFavorite(int id, bool isFavorite);
    bool setRecipeImagePath(int id, const QString &path);

    QList<Recipe> getAllRecipes();
    Recipe getRecipeById(int id);

    // Shopping list 
    bool addShoppingItem(ShoppingItem &item);
    bool setShoppingItemBought(int id, bool bought);
    bool deleteBoughtShoppingItems();
    QList<ShoppingItem> getAllShoppingItems();

    // Meal planner
    QMap<int, int> getMealPlan();
    bool setMealPlanEntry(int dayOfWeek, int mealType, int recipeId);

private:
    DatabaseManager() = default;
    
    bool initSchema();

    void mapRecipeRow(const QSqlQuery &query, Recipe &r) const;

    QSqlDatabase m_db;
    QString m_lastError;
};

#endif 
