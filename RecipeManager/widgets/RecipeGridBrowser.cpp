#include "RecipeGridBrowser.h"
#include "ui_recipegridbrowser.h"
#include <QComboBox>
#include <QResizeEvent>
#include <QStyledItemDelegate>
#include <algorithm>

namespace {

int totalTime(const Recipe &r)
{
    return r.prepTime + r.cookTime;
}

int difficultyRank(const QString &difficulty)
{
    if (difficulty == QStringLiteral("Łatwy")) return 0;
    if (difficulty == QStringLiteral("Średni")) return 1;
    if (difficulty == QStringLiteral("Trudny")) return 2;
    return 3;
}

} // namespace


RecipeGridBrowser::RecipeGridBrowser(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::RecipeGridBrowser)
{
    ui->setupUi(this);

    m_gridLayout = qobject_cast<QGridLayout *>(ui->scrollAreaWidgetContents->layout());
    if (m_gridLayout)
        m_gridLayout->setAlignment(Qt::AlignTop | Qt::AlignLeft);

    m_resizeTimer = new QTimer(this);
    m_resizeTimer->setSingleShot(true);
    m_resizeTimer->setInterval(120);
    connect(m_resizeTimer, &QTimer::timeout, this, [this] {
        loadCards(m_filteredRecipes);
    });

    setupCategoryButtons();
    setupSortCombo();
}

RecipeGridBrowser::~RecipeGridBrowser()
{
    delete ui;
}

void RecipeGridBrowser::setPageHeader(const QString &title, const QString &subtitle)
{
    ui->label_PageTitle->setText(title);
    ui->label_PageTitle2->setText(subtitle);
}

void RecipeGridBrowser::setAddButtonVisible(bool visible)// for btn +
{
    ui->pushButton_Add->setVisible(visible);
}

void RecipeGridBrowser::refreshRecipes(const QList<Recipe> &recipes)
{
    m_allRecipes = recipes;
    applyFilters();
}

void RecipeGridBrowser::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    if (!m_filteredRecipes.isEmpty())
        m_resizeTimer->start();
}

void RecipeGridBrowser::on_lineEdit_Search_textChanged(const QString &)
{
    applyFilters();
}

void RecipeGridBrowser::filterByCategory(const QString &category)
{
    m_currentCategory = category;
    applyFilters();
}

void RecipeGridBrowser::setupCategoryButtons()
{
    const QList<QPushButton *> buttons = {
        ui->pushButton_All,
        ui->pushButton_Breakfast,
        ui->pushButton_Breakfast2,
        ui->pushButton_Deserts,
        ui->pushButton_Evening,
        ui->pushButton_Another,
    };

    for (QPushButton *button : buttons) {
        connect(button, &QPushButton::clicked,
                this, [this, button] { filterByCategory(button->text()); });
    }
}

void RecipeGridBrowser::applyFilters()
{
    QList<Recipe> result = m_allRecipes;

    if (m_currentCategory != QStringLiteral("Wszystkie")) {
        result.erase(
            std::remove_if(result.begin(), result.end(),
                           [this](const Recipe &r) {
                               return r.category != m_currentCategory;//true -> delete
                           }),
            result.end());
    }

    const QString text = ui->lineEdit_Search->text().trimmed();
    if (!text.isEmpty()) {
        result.erase(
            std::remove_if(result.begin(), result.end(),
                           [&text](const Recipe &r) {
                               return !r.name.contains(text, Qt::CaseInsensitive);//a A 
                           }),
            result.end());
    }

    applySort(result);

    m_filteredRecipes = result;
    loadCards(m_filteredRecipes);
}

void RecipeGridBrowser::setupSortCombo()
{
    ui->comboBox_Sort->setItemDelegate(new QStyledItemDelegate(this));

    connect(ui->comboBox_Sort, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, [this](int) { applyFilters(); });
}

void RecipeGridBrowser::applySort(QList<Recipe> &recipes) const
{
    const int mode = ui->comboBox_Sort->currentIndex();

    auto byIdDesc = [](const Recipe &a, const Recipe &b) { return a.id > b.id; };
    auto byIdAsc = [](const Recipe &a, const Recipe &b) { return a.id < b.id; };
    auto byTimeAsc = [](const Recipe &a, const Recipe &b) {
        return totalTime(a) < totalTime(b);
    };
    auto byTimeDesc = [](const Recipe &a, const Recipe &b) {
        return totalTime(a) > totalTime(b);
    };
    auto byNameAsc = [](const Recipe &a, const Recipe &b) {
        return QString::localeAwareCompare(a.name, b.name) < 0;
    };
    auto byNameDesc = [](const Recipe &a, const Recipe &b) {
        return QString::localeAwareCompare(a.name, b.name) > 0;
    };
    auto byDifficultyAsc = [](const Recipe &a, const Recipe &b) {
        return difficultyRank(a.difficulty) < difficultyRank(b.difficulty);
    };
    auto byDifficultyDesc = [](const Recipe &a, const Recipe &b) {
        return difficultyRank(a.difficulty) > difficultyRank(b.difficulty);
    };
    auto byRatingAsc = [](const Recipe &a, const Recipe &b) { return a.rating < b.rating; };
    auto byRatingDesc = [](const Recipe &a, const Recipe &b) { return a.rating > b.rating; };

    switch (mode) {
    case 0: std::sort(recipes.begin(), recipes.end(), byIdDesc); break;
    case 1: std::sort(recipes.begin(), recipes.end(), byIdAsc); break;
    case 2: std::sort(recipes.begin(), recipes.end(), byTimeAsc); break;
    case 3: std::sort(recipes.begin(), recipes.end(), byTimeDesc); break;
    case 4: std::sort(recipes.begin(), recipes.end(), byNameAsc); break;
    case 5: std::sort(recipes.begin(), recipes.end(), byNameDesc); break;
    case 6: std::sort(recipes.begin(), recipes.end(), byDifficultyAsc); break;
    case 7: std::sort(recipes.begin(), recipes.end(), byDifficultyDesc); break;
    case 8: std::sort(recipes.begin(), recipes.end(), byRatingAsc); break;
    case 9: std::sort(recipes.begin(), recipes.end(), byRatingDesc); break;
    default: break;
    }
}

int RecipeGridBrowser::calculateColumns() const
{
    int width = ui->scrollArea->viewport()->width();
    if (width < 100)
        width = 1000;
    return qMax(1, (width - 20) / (300 + 20));
}

void RecipeGridBrowser::loadCards(const QList<Recipe> &recipes)
{
    if (!m_gridLayout)
        return;

    while (QLayoutItem *item = m_gridLayout->takeAt(0)) { //delete from layout  from first
        QWidget *w = item->widget();// get the widget stored inside the layout
        if (w != nullptr) {
            w->deleteLater();
        }
        delete item;
    }

    const int columns = calculateColumns();
    int row = 0, col = 0;

    for (const Recipe &r : recipes) {
        auto *card = new RecipeCard(r, ui->scrollAreaWidgetContents);
        card->setFixedSize(300, 300);
        m_gridLayout->addWidget(card, row, col);

        connect(card, &RecipeCard::cardClicked, this, &RecipeGridBrowser::cardClicked);
        connect(card, &RecipeCard::favoriteClicked, this, &RecipeGridBrowser::favoriteClicked);

        if (++col >= columns) {
            col = 0;
            ++row;
        }
    }
}

void RecipeGridBrowser::on_pushButton_Add_clicked()
{
    emit addClicked();
}
