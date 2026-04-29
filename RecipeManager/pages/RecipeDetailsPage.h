#ifndef RECIPEDETAILSPAGE_H
#define RECIPEDETAILSPAGE_H

#include <QWidget>
#include "models/Recipe.h"

namespace Ui { class RecipeDetailsPage; }

class RecipeDetailsPage : public QWidget
{
    Q_OBJECT

public:
    explicit RecipeDetailsPage(QWidget *parent = nullptr);
    ~RecipeDetailsPage();

    void loadRecipe(const Recipe& recipe);

signals:
    void backClicked();

private:
    Ui::RecipeDetailsPage *ui;
};

#endif // RECIPEDETAILSPAGE_H