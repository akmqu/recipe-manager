#include "FavoritesPage.h"
#include "widgets/RecipeGridBrowser.h"
#include <QVBoxLayout>

FavoritesPage::FavoritesPage(QWidget *parent)
    : QWidget(parent)
    , m_browser(new RecipeGridBrowser(this))
{
    auto *lay = new QVBoxLayout(this);
    lay->setContentsMargins(0, 0, 0, 0);
    lay->setSpacing(0);
    lay->addWidget(m_browser);

    m_browser->setPageHeader(
        QStringLiteral("Ulubione"),
        QStringLiteral("Ulubione przepisy zawsze pod ręką"));
    m_browser->setAddButtonVisible(false);

    connect(m_browser, &RecipeGridBrowser::cardClicked,
            this, &FavoritesPage::cardClicked);
    connect(m_browser, &RecipeGridBrowser::favoriteClicked,
            this, &FavoritesPage::favoriteClicked);
}

void FavoritesPage::refreshRecipes(const QList<Recipe> &recipes)
{
    QList<Recipe> favorites;
    favorites.reserve(recipes.size());
    for (const Recipe &r : recipes) {
        if (r.isFavorite)
            favorites.append(r);
    }
    m_browser->refreshRecipes(favorites);
}
