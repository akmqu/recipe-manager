#ifndef ALLRECIPESPAGE_H
#define ALLRECIPESPAGE_H

#include "widgets/RecipeGridBrowser.h" 

class AllRecipesPage : public RecipeGridBrowser
{
    Q_OBJECT

public:
    explicit AllRecipesPage(QWidget *parent = nullptr);
};

#endif 