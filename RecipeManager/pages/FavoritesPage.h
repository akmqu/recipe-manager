#ifndef FAVORITESPAGE_H
#define FAVORITESPAGE_H

#include "widgets/RecipeGridBrowser.h"

class FavoritesPage : public RecipeGridBrowser
{
    Q_OBJECT

public:
    explicit FavoritesPage(QWidget *parent = nullptr);
    
    void refreshRecipes(const QList<Recipe> &recipes);
};

#endif // FAVORITESPAGE_H