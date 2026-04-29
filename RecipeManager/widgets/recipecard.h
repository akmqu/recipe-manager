#ifndef RECIPECARD_H
#define RECIPECARD_H

#include <QWidget>
#include "models/Recipe.h"

namespace Ui { class RecipeCard; }

class RecipeCard : public QWidget
{
    Q_OBJECT

public:
    explicit RecipeCard(const Recipe& recipe, QWidget *parent = nullptr);
    ~RecipeCard();

    int recipeId() const { return m_id; }

signals:
    void cardClicked(int id);
    void favoriteClicked(int id, bool isFavorite);

protected:
    void mousePressEvent(QMouseEvent*) override;

private:
    Ui::RecipeCard *ui;
    int  m_id;
    bool m_isFavorite;

    void updateFavoriteButton();
};

#endif // RECIPECARD_H