#ifndef ADDRECIPEPAGE_H
#define ADDRECIPEPAGE_H

#include <QWidget>
#include "models/Recipe.h"

class QShowEvent;

namespace Ui { class AddRecipePage; }

class AddRecipePage : public QWidget
{
    Q_OBJECT

public:
    explicit AddRecipePage(QWidget *parent = nullptr);
    ~AddRecipePage();

    void clearForm();
    void loadForEdit(const Recipe &recipe);

protected:
    void showEvent(QShowEvent *event) override;

signals:
    void recipeSaved(const Recipe &recipe);
    // 0 = user was adding a new recipe; >0 = was editing that recipe id. 
    void cancelled(int previousEditRecipeId);

private slots:
    void on_pushButton_clicked();
    void on_pushButton_Cancel_clicked();
    void on_pushButton_SelectImage_clicked();
    void on_pushButton_ClearImage_clicked();

private:
    Ui::AddRecipePage *ui;
    int m_editId = -1;
    bool m_editFavorite = false;
    QString m_selectedImagePath;

    void refreshImagePreview();

    Recipe collectFormData() const;
    bool validate(const Recipe &recipe) const;
};

#endif 
