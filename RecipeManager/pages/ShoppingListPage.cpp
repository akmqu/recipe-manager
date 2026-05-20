#include "ShoppingListPage.h"
#include "ui_ShoppingListPage.h"
#include "database/DatabaseManager.h"
#include <QDebug>

ShoppingListPage::ShoppingListPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ShoppingListPage)
{
    ui->setupUi(this);

    connect(ui->pushButton_DeleteBought, &QPushButton::clicked,
            this, &ShoppingListPage::on_pushButton_DeleteBought_clicked);

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
    const QList<ShoppingItem> items = DatabaseManager::instance().getAllShoppingItems();
    for (const ShoppingItem &item : items)
        addCheckbox(item.id, item.name, item.isBought);
}

void ShoppingListPage::on_pushButton_Add_clicked()
{
    const QString name = ui->lineEdit_AddProduct->text().trimmed();
    if (name.isEmpty())
        return;

    ShoppingItem item;
    item.name     = name;
    item.isBought = false;

    if (!DatabaseManager::instance().addShoppingItem(item)) {
        qWarning() << "addShoppingItem failed:" << DatabaseManager::instance().lastError();
        return;
    }

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

    QLayout *layout = ui->verticalLayout_Bought;
    for (int i = layout->count() - 1; i >= 0; --i) {
        QLayoutItem *li = layout->takeAt(i);
        if (!li) continue;
        if (QCheckBox *cb = qobject_cast<QCheckBox *>(li->widget())) {
            m_itemIds.remove(cb);
            delete cb;
        }
        delete li;
    }

}

// Checkbox toggled: move between sections + persist to DB
void ShoppingListPage::onItemToggled(QCheckBox *cb, bool checked)
{
    const int dbId = m_itemIds.value(cb, -1);
    if (dbId > 0)
        DatabaseManager::instance().setShoppingItemBought(dbId, checked);

    QFont f = cb->font();
    f.setStrikeOut(checked);
    cb->setFont(f);

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

    QFont f = cb->font();
    f.setPointSize(11);
    f.setStrikeOut(bought);
    cb->setFont(f);

    m_itemIds[cb] = dbId;

    {
        QSignalBlocker blocker(cb);
        cb->setChecked(bought);
    }

    connect(cb, &QCheckBox::toggled, this, [this, cb](bool state) {
        onItemToggled(cb, state);
    });

    if (bought)
        ui->verticalLayout_Bought->addWidget(cb);
    else
        ui->verticalLayout_ToBuy->addWidget(cb);

}

