#ifndef RECIPEDETAILSPAGE_H
#define RECIPEDETAILSPAGE_H

#include <QWidget>
#include "models/Recipe.h"

class QResizeEvent;
namespace Ui { class RecipeDetailsPage; }

class RecipeDetailsPage : public QWidget
{
    Q_OBJECT

public:
    explicit RecipeDetailsPage(QWidget *parent = nullptr);
    ~RecipeDetailsPage();

    void loadRecipe(const Recipe &recipe);

signals:
    void backClicked();
    void editRequested(int recipeId);
    void deleteRequested(int recipeId);

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    void updateHeroImage();

    Ui::RecipeDetailsPage *ui;
    int     m_recipeId      = 0;
    QString m_heroImagePath;

    static constexpr int kHeroCornerRadius = 16;
};

#endif // RECIPEDETAILSPAGE_H