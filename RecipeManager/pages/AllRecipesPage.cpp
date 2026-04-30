#include "AllRecipesPage.h"
#include "widgets/RecipeGridBrowser.h"
#include <QVBoxLayout>

AllRecipesPage::AllRecipesPage(QWidget *parent)
    : QWidget(parent)
    , m_browser(new RecipeGridBrowser(this))
{
    auto *lay = new QVBoxLayout(this);
    lay->setContentsMargins(0, 0, 0, 0);
    lay->setSpacing(0);
    lay->addWidget(m_browser);

    m_browser->setPageHeader(
        QStringLiteral("Wszystkie przepisy"),
        QStringLiteral("Przeglądaj, gotuj i zapisuj ulubione przepisy"));
    m_browser->setAddButtonVisible(true);

    connect(m_browser, &RecipeGridBrowser::cardClicked,
            this, &AllRecipesPage::cardClicked);
    connect(m_browser, &RecipeGridBrowser::favoriteClicked,
            this, &AllRecipesPage::favoriteClicked);
    connect(m_browser, &RecipeGridBrowser::addClicked,
            this, &AllRecipesPage::addClicked);
}

void AllRecipesPage::refreshRecipes(const QList<Recipe> &recipes)
{
    m_browser->refreshRecipes(recipes);
}
