#include "ShoppingListPage.h"
#include "ui_ShoppingListPage.h"
#include "database/DatabaseManager.h"
#include <QDebug>

ShoppingListPage::ShoppingListPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ShoppingListPage)
{
    ui->setupUi(this);

    connect(ui->lineEdit_AddProduct, &QLineEdit::returnPressed,
            this, &ShoppingListPage::on_pushButton_Add_clicked);

    loadFromDatabase();
}

ShoppingListPage::~ShoppingListPage()
{
    delete ui;
}

void ShoppingListPage::loadFromDatabase()
{
    // get all saved shopping items and show them as checkboxes
    const QList<ShoppingItem> items = DatabaseManager::instance().getAllShoppingItems();
    for (const ShoppingItem &item : items) {
        addCheckbox(item.id, item.name, item.isBought);
    }
}

void ShoppingListPage::on_pushButton_Add_clicked()
{
    const QString name = ui->lineEdit_AddProduct->text().trimmed();
    if (name.isEmpty()) {
        return;
    }

    ShoppingItem item;
    item.name     = name;
    item.isBought = false;

    if (!DatabaseManager::instance().addShoppingItem(item)) {
        qWarning() << "addShoppingItem failed:" << DatabaseManager::instance().lastError();
        return;
    }

    // add product to ui 
    addCheckbox(item.id, item.name, false);
    ui->lineEdit_AddProduct->clear();
}

void ShoppingListPage::on_pushButton_DeleteBought_clicked()
{
    if (!DatabaseManager::instance().deleteBoughtShoppingItems()) {
        qWarning() << "deleteBoughtShoppingItems failed:"
                   << DatabaseManager::instance().lastError();
        return;
    }

    // remove bought products from ui
    QLayout *layout = ui->verticalLayout_Bought;
    for (int i = layout->count() - 1; i >= 0; --i) {
        QLayoutItem *li = layout->takeAt(i);
        if (!li) {
            continue;
        }
        if (QCheckBox *cb = qobject_cast<QCheckBox *>(li->widget())) {
            delete cb;
        }
        delete li;
    }

}

void ShoppingListPage::onItemToggled(QCheckBox *cb, bool checked)
{
    // read database id stored in this checkbox
    const int dbId = cb->property("dbId").toInt();
    //bought/not bought state
    if (dbId > 0 && !DatabaseManager::instance().setShoppingItemBought(dbId, checked)) {
        qWarning() << "setShoppingItemBought failed:"
                   << DatabaseManager::instance().lastError();

        QSignalBlocker blocker(cb);
        cb->setChecked(!checked);
        return;
    }

    QFont f = cb->font();
    f.setStrikeOut(checked);
    cb->setFont(f);

    // move checkbox between sections
    if (checked) {
        ui->verticalLayout_ToBuy->removeWidget(cb);
        ui->verticalLayout_Bought->addWidget(cb);
    } else {
        ui->verticalLayout_Bought->removeWidget(cb);
        ui->verticalLayout_ToBuy->addWidget(cb);
    }

}

void ShoppingListPage::addCheckbox(int dbId, const QString &name, bool bought)
{
    QCheckBox *cb = new QCheckBox(name, this);
    cb->setProperty("dbId", dbId); // store database id directly in checkbox

    QFont f = cb->font();
    f.setPointSize(11);
    f.setStrikeOut(bought);
    cb->setFont(f);

    // set initial bought/not bought state
    cb->setChecked(bought);

    // react when user marks product 
    connect(cb, &QCheckBox::toggled, this, [this, cb](bool state) {
        onItemToggled(cb, state);
    });

    if (bought) {
        ui->verticalLayout_Bought->addWidget(cb);
    } else {
        ui->verticalLayout_ToBuy->addWidget(cb);
    }

}
