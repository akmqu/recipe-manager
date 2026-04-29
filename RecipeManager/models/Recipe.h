#ifndef RECIPE_H
#define RECIPE_H

#include <QString>

struct Recipe {
    int id = 0;
    QString name;
    QString ingredients; 
    QString notes;
    QString category = "Nie podano";
    QString difficulty = "Nie podano";
    int prepTime = 0;
    int cookTime = 0;
    bool isFavorite = false;
    int rating = 0;
    QString imagePath = "";
};






#endif 