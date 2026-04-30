#include "DatabaseManager.h"
#include <QDebug>
#include <QVariant>

namespace {
constexpr auto kSelectFields =
    "SELECT recipe_id, name, prep_time, cook_time, "
    "ingredients, notes, category, difficulty, is_favorite, image_path "
    "FROM recipe";
}

void DatabaseManager::mapRecipeRow(const QSqlQuery &query, Recipe &r) const
{
    // Column order must match kSelectFields
    r.id = query.value(0).toInt();
    r.name = query.value(1).toString();
    r.prepTime = query.value(2).toInt();
    r.cookTime = query.value(3).toInt();
    r.ingredients = query.value(4).toString();
    r.notes = query.value(5).toString();
    r.category = query.value(6).toString();
    r.difficulty = query.value(7).toString();
    r.isFavorite = query.value(8).toBool();
    r.imagePath = query.value(9).toString();
}

void DatabaseManager::ensureSchema()
{
    QSqlQuery q;
    if (!q.exec(QStringLiteral("ALTER TABLE recipe ADD COLUMN IF NOT EXISTS image_path TEXT"))) {
        m_lastError = q.lastError().text();
        qDebug() << "ensureSchema (image_path):" << m_lastError;
    }
}

DatabaseManager &DatabaseManager::instance()
{
    static DatabaseManager instance;
    return instance;
}

bool DatabaseManager::connect()
{
    m_db = QSqlDatabase::addDatabase(QStringLiteral("QPSQL"));
    m_db.setHostName(QStringLiteral("localhost"));
    m_db.setPort(5432);
    m_db.setDatabaseName(QStringLiteral("recipes_db"));
    m_db.setUserName(QStringLiteral("postgres"));
    m_db.setPassword(QStringLiteral("123"));

    if (!m_db.open()) {
        m_lastError = m_db.lastError().text();
        return false;
    }
    ensureSchema();
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

bool DatabaseManager::addRecipe(Recipe &recipe)
{
    QSqlQuery query;
    query.prepare(
        QStringLiteral(
            "INSERT INTO recipe (name, prep_time, cook_time, ingredients, notes, category, "
            "difficulty, is_favorite, image_path) "
            "VALUES (:name, :prep, :cook, :ingr, :notes, :cat, :diff, :fav, :img) "
            "RETURNING recipe_id"));
    query.bindValue(QStringLiteral(":name"), recipe.name);
    query.bindValue(QStringLiteral(":prep"), recipe.prepTime);
    query.bindValue(QStringLiteral(":cook"), recipe.cookTime);
    query.bindValue(QStringLiteral(":ingr"), recipe.ingredients);
    query.bindValue(QStringLiteral(":notes"), recipe.notes);
    query.bindValue(QStringLiteral(":cat"), recipe.category);
    query.bindValue(QStringLiteral(":diff"), recipe.difficulty);
    query.bindValue(QStringLiteral(":fav"), recipe.isFavorite);
    query.bindValue(QStringLiteral(":img"), recipe.imagePath.isEmpty()
        ? QVariant(QVariant::String)
        : recipe.imagePath);

    if (!query.exec()) {
        m_lastError = query.lastError().text();
        return false;
    }
    if (query.next()) {
        recipe.id = query.value(0).toInt();
        return true;
    }
    m_lastError = QStringLiteral("RETURNING recipe_id failed");
    return false;
}

bool DatabaseManager::updateRecipe(const Recipe &recipe)
{
    QSqlQuery query;
    query.prepare(
        QStringLiteral(
            "UPDATE recipe SET name=:name, prep_time=:prep, cook_time=:cook, "
            "ingredients=:ingr, notes=:notes, category=:cat, difficulty=:diff, "
            "is_favorite=:fav, image_path=:img WHERE recipe_id=:id"));
    query.bindValue(QStringLiteral(":name"), recipe.name);
    query.bindValue(QStringLiteral(":prep"), recipe.prepTime);
    query.bindValue(QStringLiteral(":cook"), recipe.cookTime);
    query.bindValue(QStringLiteral(":ingr"), recipe.ingredients);
    query.bindValue(QStringLiteral(":notes"), recipe.notes);
    query.bindValue(QStringLiteral(":cat"), recipe.category);
    query.bindValue(QStringLiteral(":diff"), recipe.difficulty);
    query.bindValue(QStringLiteral(":fav"), recipe.isFavorite);
    query.bindValue(QStringLiteral(":img"), recipe.imagePath.isEmpty()
        ? QVariant(QVariant::String)
        : recipe.imagePath);
    query.bindValue(QStringLiteral(":id"), recipe.id);

    if (!query.exec()) {
        m_lastError = query.lastError().text();
        return false;
    }
    return true;
}

bool DatabaseManager::setRecipeImagePath(int id, const QString &path)
{
    QSqlQuery query;
    query.prepare(QStringLiteral("UPDATE recipe SET image_path = :img WHERE recipe_id = :id"));
    query.bindValue(QStringLiteral(":img"), path.isEmpty() ? QVariant(QVariant::String) : path);
    query.bindValue(QStringLiteral(":id"), id);
    if (!query.exec()) {
        m_lastError = query.lastError().text();
        return false;
    }
    return true;
}

bool DatabaseManager::deleteRecipe(const Recipe &recipe)
{
    QSqlQuery query;
    query.prepare(QStringLiteral("DELETE FROM recipe WHERE recipe_id = :id"));
    query.bindValue(QStringLiteral(":id"), recipe.id);

    if (!query.exec()) {
        m_lastError = query.lastError().text();
        return false;
    }
    return true;
}

bool DatabaseManager::setFavorite(int id, bool isFavorite)
{
    QSqlQuery query;
    query.prepare(QStringLiteral("UPDATE recipe SET is_favorite=:fav WHERE recipe_id=:id"));
    query.bindValue(QStringLiteral(":fav"), isFavorite);
    query.bindValue(QStringLiteral(":id"), id);

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
    if (!query.exec(QString(kSelectFields) + QStringLiteral(" ORDER BY recipe_id DESC"))) {
        m_lastError = query.lastError().text();
        qDebug() << "getAllRecipes error:" << m_lastError;
        return list;
    }

    while (query.next()) {
        Recipe r;
        mapRecipeRow(query, r);
        list.append(r);
    }
    return list;
}

QList<Recipe> DatabaseManager::getFavorites()
{
    QList<Recipe> list;
    QSqlQuery query;
    if (!query.exec(QString(kSelectFields)
                    + QStringLiteral(" WHERE is_favorite = true ORDER BY recipe_id DESC"))) {
        m_lastError = query.lastError().text();
        return list;
    }

    while (query.next()) {
        Recipe r;
        mapRecipeRow(query, r);
        list.append(r);
    }
    return list;
}

Recipe DatabaseManager::getRecipeById(int id)
{
    Recipe r;
    QSqlQuery query;
    query.prepare(QString(kSelectFields) + QStringLiteral(" WHERE recipe_id = :id"));
    query.bindValue(QStringLiteral(":id"), id);

    if (!query.exec()) {
        m_lastError = query.lastError().text();
        return r;
    }

    if (query.next())
        mapRecipeRow(query, r);
    return r;
}
