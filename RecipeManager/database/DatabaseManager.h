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

private:
    DatabaseManager() = default;
    void ensureSchema();

    void mapRecipeRow(const QSqlQuery &query, Recipe &r) const;

    QSqlDatabase m_db;
    QString m_lastError;
};

#endif // DATABASEMANAGER_H
