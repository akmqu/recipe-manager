#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QList>
#include "models/Recipe.h"

class DatabaseManager
{
public:
    static DatabaseManager& instance();

    bool connect();
    bool isConnected() const;
    QString lastError() const;

    bool addRecipe(const Recipe& recipe);
    bool updateRecipe(const Recipe& recipe);
    bool deleteRecipe(const Recipe& recipe);
    bool setFavorite(int id, bool isFavorite);

    QList<Recipe> getAllRecipes();
    QList<Recipe> getFavorites();
    Recipe        getRecipeById(int id);

private:
    DatabaseManager() = default;
    QSqlDatabase m_db;
    QString      m_lastError;
};

#endif // DATABASEMANAGER_H