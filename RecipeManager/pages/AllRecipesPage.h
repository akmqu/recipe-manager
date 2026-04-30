#ifndef ALLRECIPESPAGE_H
#define ALLRECIPESPAGE_H

#include <QWidget>
#include "models/Recipe.h"

class RecipeGridBrowser;

class AllRecipesPage : public QWidget
{
    Q_OBJECT

public:
    explicit AllRecipesPage(QWidget *parent = nullptr);

    void refreshRecipes(const QList<Recipe> &recipes);

signals:
    void cardClicked(int id);
    void favoriteClicked(int id, bool isFavorite);
    void addClicked();

private:
    RecipeGridBrowser *m_browser = nullptr;
};

#endif // ALLRECIPESPAGE_H
