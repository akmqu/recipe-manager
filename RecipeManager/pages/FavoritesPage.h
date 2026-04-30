#ifndef FAVORITESPAGE_H
#define FAVORITESPAGE_H

#include <QWidget>
#include "models/Recipe.h"

class RecipeGridBrowser;

class FavoritesPage : public QWidget
{
    Q_OBJECT

public:
    explicit FavoritesPage(QWidget *parent = nullptr);

    void refreshRecipes(const QList<Recipe> &recipes);

signals:
    void cardClicked(int id);
    void favoriteClicked(int id, bool isFavorite);

private:
    RecipeGridBrowser *m_browser = nullptr;
};

#endif // FAVORITESPAGE_H
