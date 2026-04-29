#include "DatabaseManager.h"
#include <QDebug>

DatabaseManager& DatabaseManager::instance()
{
    static DatabaseManager instance;
    return instance;
}

bool DatabaseManager::connect()
{
    m_db = QSqlDatabase::addDatabase("QPSQL");
    m_db.setHostName("localhost");
    m_db.setPort(5432);
    m_db.setDatabaseName("recipes_db");
    m_db.setUserName("postgres");
    m_db.setPassword("123");

    if (!m_db.open()) {
        m_lastError = m_db.lastError().text();
        return false;
    }
    return true;
}

bool DatabaseManager::isConnected() const
{
    return m_db.isOpen();
}

QString DatabaseManager::lastError() const
{
    return m_lastError;
}

bool DatabaseManager::addRecipe(const Recipe& recipe)
{
    QSqlQuery query;
    query.prepare(
        "INSERT INTO recipe (name, prep_time, cook_time, ingredients, notes, category, difficulty) "
        "VALUES (:name, :prep, :cook, :ingr, :notes, :cat, :diff)");

    query.bindValue(":name", recipe.name);
    query.bindValue(":prep", recipe.prepTime);
    query.bindValue(":cook", recipe.cookTime);
    query.bindValue(":ingr", recipe.ingredients);
    query.bindValue(":notes", recipe.notes);
    query.bindValue(":cat",  recipe.category);
    query.bindValue(":diff", recipe.difficulty);

    if (!query.exec()) {
        m_lastError = query.lastError().text();
        return false;
    }
    return true;
}

bool DatabaseManager::updateRecipe(const Recipe& recipe)
{
    QSqlQuery query;
    query.prepare(
        "UPDATE recipe SET name=:name, prep_time=:prep, cook_time=:cook, "
        "ingredients=:ingr, notes=:notes, category=:cat, difficulty=:diff, "
        "is_favorite=:fav WHERE recipe_id=:id");

    query.bindValue(":name", recipe.name);
    query.bindValue(":prep", recipe.prepTime);
    query.bindValue(":cook", recipe.cookTime);
    query.bindValue(":ingr", recipe.ingredients);
    query.bindValue(":notes", recipe.notes);
    query.bindValue(":cat",  recipe.category);
    query.bindValue(":diff", recipe.difficulty);
    query.bindValue(":fav",  recipe.isFavorite);
    query.bindValue(":id",   recipe.id);

    if (!query.exec()) {
        m_lastError = query.lastError().text();
        return false;
    }
    return true;
}

bool DatabaseManager::deleteRecipe(const Recipe& recipe)
{
    QSqlQuery query;
    query.prepare("DELETE FROM recipe WHERE recipe_id = :id");
    query.bindValue(":id", recipe.id);

    if (!query.exec()) {
        m_lastError = query.lastError().text();
        return false;
    }
    return true;
}

bool DatabaseManager::setFavorite(int id, bool isFavorite)
{
    QSqlQuery query;
    query.prepare("UPDATE recipe SET is_favorite=:fav WHERE recipe_id=:id");
    query.bindValue(":fav", isFavorite);
    query.bindValue(":id",  id);

    if (!query.exec()) {
        m_lastError = query.lastError().text();
        return false;
    }
    return true;
}

QList<Recipe> DatabaseManager::getAllRecipes()
{
    QList<Recipe> list;
    QSqlQuery query;

    if (!query.exec(
            "SELECT recipe_id, name, prep_time, cook_time, "
            "ingredients, notes, category, difficulty, is_favorite "
            "FROM recipe")) {
        m_lastError = query.lastError().text();
        qDebug() << "getAllRecipes error:" << m_lastError;
        return list;
    }

    while (query.next()) {
        Recipe r;
        r.id          = query.value("recipe_id").toInt();
        r.name        = query.value("name").toString();
        r.prepTime    = query.value("prep_time").toInt();
        r.cookTime    = query.value("cook_time").toInt();
        r.ingredients = query.value("ingredients").toString();
        r.notes       = query.value("notes").toString();
        r.category    = query.value("category").toString();
        r.difficulty  = query.value("difficulty").toString();
        r.isFavorite  = query.value("is_favorite").toBool();
        list.append(r);
    }
    return list;
}

QList<Recipe> DatabaseManager::getFavorites()
{
    QList<Recipe> list;
    QSqlQuery query;

    if (!query.exec(
            "SELECT recipe_id, name, prep_time, cook_time, "
            "ingredients, notes, category, difficulty, is_favorite "
            "FROM recipe WHERE is_favorite = true")) {
        m_lastError = query.lastError().text();
        return list;
    }

    while (query.next()) {
        Recipe r;
        r.id         = query.value("recipe_id").toInt();
        r.name       = query.value("name").toString();
        r.prepTime   = query.value("prep_time").toInt();
        r.cookTime   = query.value("cook_time").toInt();
        r.ingredients= query.value("ingredients").toString();
        r.notes      = query.value("notes").toString();
        r.category   = query.value("category").toString();
        r.difficulty = query.value("difficulty").toString();
        r.isFavorite = true;
        list.append(r);
    }
    return list;
}

Recipe DatabaseManager::getRecipeById(int id)
{
    Recipe r;
    QSqlQuery query;
    query.prepare("SELECT * FROM recipe WHERE recipe_id = :id");
    query.bindValue(":id", id);

    if (!query.exec()) {
        m_lastError = query.lastError().text();
        return r;
    }

    if (query.next()) {
        r.id          = query.value("recipe_id").toInt();
        r.name        = query.value("name").toString();
        r.prepTime    = query.value("prep_time").toInt();
        r.cookTime    = query.value("cook_time").toInt();
        r.ingredients = query.value("ingredients").toString();
        r.notes       = query.value("notes").toString();
        r.category    = query.value("category").toString();
        r.difficulty  = query.value("difficulty").toString();
        r.isFavorite  = query.value("is_favorite").toBool();
    }
    return r;
}