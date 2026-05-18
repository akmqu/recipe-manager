#ifndef SHOPPINGLISTPAGE_H
#define SHOPPINGLISTPAGE_H

#include <QWidget>
#include <QCheckBox>
#include <QMap>

namespace Ui { class ShoppingListPage; }

class ShoppingListPage : public QWidget
{
    Q_OBJECT

public:
    explicit ShoppingListPage(QWidget *parent = nullptr);
    ~ShoppingListPage();

private slots:
    void on_pushButton_Add_clicked();
    void on_pushButton_DeleteBought_clicked();

private:
    Ui::ShoppingListPage *ui;

    QMap<QCheckBox *, int> m_itemIds;

    void loadFromDatabase();
    void addCheckbox(int dbId, const QString &name, bool bought);
    void onItemToggled(QCheckBox *cb, bool checked);
    void updateBoughtCount();
};

#endif // SHOPPINGLISTPAGE_H