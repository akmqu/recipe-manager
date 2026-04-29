#ifndef ALLRECIPESPAGE_H
#define ALLRECIPESPAGE_H

#include <QWidget>
#include <QGridLayout>
#include <QTimer>
#include "models/Recipe.h"
#include "widgets/recipecard.h"

namespace Ui { class AllRecipesPage; }

class AllRecipesPage : public QWidget
{
    Q_OBJECT

public:
    explicit AllRecipesPage(QWidget *parent = nullptr);
    ~AllRecipesPage();

    // Called by MainWindow after fetching from DB
    void refreshRecipes(const QList<Recipe>& recipes);

signals:
    void cardClicked(int id);
    void favoriteClicked(int id, bool isFavorite);
    void addClicked();

protected:
    void resizeEvent(QResizeEvent* event) override;

private slots:
    void on_lineEdit_Search_textChanged(const QString& text);
    void filterByCategory(const QString& category);
    void on_pushButton_Add_clicked();

private:
    Ui::AllRecipesPage *ui;

    QGridLayout* m_gridLayout    = nullptr;
    QTimer*      m_resizeTimer   = nullptr;

    QList<Recipe> m_allRecipes;
    QList<Recipe> m_filteredRecipes;
    QString       m_currentCategory = "Wszystkie";

    void applyFilters();
    void loadCards(const QList<Recipe>& recipes);
    int calculateColumns() const;
    void setupCategoryButtons();
};

#endif // ALLRECIPESPAGE_H