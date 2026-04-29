#ifndef SHOPPINGLISTPAGE_H
#define SHOPPINGLISTPAGE_H

#include <QWidget>

namespace Ui { class ShoppingListPage; }

class ShoppingListPage : public QWidget
{
    Q_OBJECT

public:
    explicit ShoppingListPage(QWidget *parent = nullptr);
    ~ShoppingListPage();

private:
    Ui::ShoppingListPage *ui;
};

#endif // SHOPPINGLISTPAGE_H