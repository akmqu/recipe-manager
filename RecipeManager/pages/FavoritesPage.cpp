#include "FavoritesPage.h"

FavoritesPage::FavoritesPage(QWidget *parent)
    : RecipeGridBrowser(parent)
{
    setPageHeader(
        QStringLiteral("Ulubione"),
        QStringLiteral("Ulubione przepisy zawsze pod ręką"));
    setAddButtonVisible(false);
}

void FavoritesPage::refreshRecipes(const QList<Recipe> &recipes)
{
    QList<Recipe> favorites;
    favorites.reserve(recipes.size());
    for (const Recipe &r : recipes) {
        if (r.isFavorite)
            favorites.append(r);
    }
    RecipeGridBrowser::refreshRecipes(favorites);
}