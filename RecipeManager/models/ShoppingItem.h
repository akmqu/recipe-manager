#ifndef SHOPPINGITEM_H
#define SHOPPINGITEM_H

#include <QString>

struct ShoppingItem {
    int     id      = 0;
    QString name;
    bool    isBought = false;
};

#endif // SHOPPINGITEM_H