#ifndef RECIPEGRIDBROWSER_H
#define RECIPEGRIDBROWSER_H

#include <QWidget>
#include <QGridLayout>
#include <QTimer>
#include "models/Recipe.h"
#include "recipecard.h"

namespace Ui { class RecipeGridBrowser; }

class RecipeGridBrowser : public QWidget
{
    Q_OBJECT

public:
    explicit RecipeGridBrowser(QWidget *parent = nullptr);
    ~RecipeGridBrowser();

    void setPageHeader(const QString &title, const QString &subtitle);
    void setAddButtonVisible(bool visible);

    void refreshRecipes(const QList<Recipe> &recipes);

signals:
    void cardClicked(int id);
    void favoriteClicked(int id, bool isFavorite);
    void addClicked();

protected:
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void on_lineEdit_Search_textChanged(const QString &text);
    void filterByCategory(const QString &category);
    void on_pushButton_Add_clicked();

private:
    Ui::RecipeGridBrowser *ui;

    QGridLayout *m_gridLayout = nullptr;
    QTimer *m_resizeTimer = nullptr;

    QList<Recipe> m_allRecipes;
    QList<Recipe> m_filteredRecipes;
    QString m_currentCategory = QStringLiteral("Wszystkie");

    void applyFilters();
    void applySort(QList<Recipe> &recipes) const;
    void loadCards(const QList<Recipe> &recipes);
    int calculateColumns() const;
    void setupCategoryButtons();
    void setupSortCombo();
};

#endif // RECIPEGRIDBROWSER_H
