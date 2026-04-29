#ifndef ADDRECIPEPAGE_H
#define ADDRECIPEPAGE_H

#include <QWidget>
#include "models/Recipe.h"

namespace Ui { class AddRecipePage; }

class AddRecipePage : public QWidget
{
    Q_OBJECT

public:
    explicit AddRecipePage(QWidget *parent = nullptr);
    ~AddRecipePage();

    void clearForm();
    void loadForEdit(const Recipe& recipe);

signals:
    void recipeSaved(const Recipe& recipe);
    void cancelled();

private slots:
    void on_pushButton_clicked();
    void on_pushButton_Cancel_clicked();

private:
    Ui::AddRecipePage *ui;
    int m_editId = -1;  // -1 = new recipe, >=0 = edit mode

    Recipe collectFormData() const;
    bool   validate(const Recipe& recipe) const;
};

#endif // ADDRECIPEPAGE_H