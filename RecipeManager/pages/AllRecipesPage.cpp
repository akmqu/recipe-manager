#include "AllRecipesPage.h"

AllRecipesPage::AllRecipesPage(QWidget *parent)
    : RecipeGridBrowser(parent)
{
    setPageHeader(
        QStringLiteral("Wszystkie przepisy"),
        QStringLiteral("Przeglądaj, gotuj i zapisuj ulubione przepisy"));
    setAddButtonVisible(true);
}