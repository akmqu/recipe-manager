#include "AllRecipesPage.h"
#include "ui_AllRecipesPage.h"
#include <QResizeEvent>
#include <QDebug>
#include <algorithm>

AllRecipesPage::AllRecipesPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::AllRecipesPage)
{
    ui->setupUi(this);

    ui->scrollArea->setWidgetResizable(true);

    if (!ui->scrollAreaWidgetContents->layout()) {
        m_gridLayout = new QGridLayout(ui->scrollAreaWidgetContents);
        m_gridLayout->setSpacing(20);
        m_gridLayout->setContentsMargins(20, 20, 20, 20);
        m_gridLayout->setAlignment(Qt::AlignTop | Qt::AlignLeft);
        ui->scrollAreaWidgetContents->setLayout(m_gridLayout);
    } else {
        m_gridLayout = qobject_cast<QGridLayout*>(
            ui->scrollAreaWidgetContents->layout());
    }

    ui->scrollAreaWidgetContents->setSizePolicy(
        QSizePolicy::Expanding, QSizePolicy::Expanding);

    // Debounce resize events
    m_resizeTimer = new QTimer(this);
    m_resizeTimer->setSingleShot(true);
    m_resizeTimer->setInterval(120);
    connect(m_resizeTimer, &QTimer::timeout, this, [this]{
        loadCards(m_filteredRecipes);
    });

    setupCategoryButtons();
}

AllRecipesPage::~AllRecipesPage()
{
    delete ui;
}

// ─────────────────────────────────────────
//  Public
// ─────────────────────────────────────────

void AllRecipesPage::refreshRecipes(const QList<Recipe>& recipes)
{
    m_allRecipes = recipes;
    applyFilters();
}

// ─────────────────────────────────────────
//  Protected
// ─────────────────────────────────────────

void AllRecipesPage::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    if (!m_filteredRecipes.isEmpty())
        m_resizeTimer->start();
}

// ─────────────────────────────────────────
//  Private slots
// ─────────────────────────────────────────

void AllRecipesPage::on_lineEdit_Search_textChanged(const QString&)
{
    applyFilters();
}

void AllRecipesPage::filterByCategory(const QString& category)
{
    m_currentCategory = category;
    applyFilters();
}

// ─────────────────────────────────────────
//  Private helpers
// ─────────────────────────────────────────

void AllRecipesPage::setupCategoryButtons()
{
    const QList<QPair<QPushButton*, QString>> buttons = {
        { ui->pushButton_All,        "Wszystkie" },
        { ui->pushButton_Breakfast,  "Śniadania" },
        { ui->pushButton_Breakfast2, "Obiady"    },
        { ui->pushButton_Deserts,    "Desery"    },
        { ui->pushButton_Evening,    "Kolacje"   },
        { ui->pushButton_Another,    "Inne"      },
    };

    for (auto& pair : buttons) {
        connect(pair.first, &QPushButton::clicked,
                this, [this, cat = pair.second]{ filterByCategory(cat); });
    }
}

void AllRecipesPage::applyFilters()
{
    QList<Recipe> result = m_allRecipes;

    // Category filter
    if (m_currentCategory != "Wszystkie") {
        result.erase(
            std::remove_if(result.begin(), result.end(),
                [this](const Recipe& r){ return r.category != m_currentCategory; }),
            result.end());
    }

    // Search filter
    const QString text = ui->lineEdit_Search->text().trimmed();
    if (!text.isEmpty()) {
        result.erase(
            std::remove_if(result.begin(), result.end(),
                [&text](const Recipe& r){
                    return !r.name.contains(text, Qt::CaseInsensitive);
                }),
            result.end());
    }

    m_filteredRecipes = result;
    loadCards(m_filteredRecipes);
}

int AllRecipesPage::calculateColumns() const
{
    int width = ui->scrollArea->viewport()->width();
    if (width < 100) width = 800;
    return qMax(1, (width - 20) / (260 + 20));
}

void AllRecipesPage::loadCards(const QList<Recipe>& recipes)
{
    // Clear existing cards
    while (QLayoutItem* item = m_gridLayout->takeAt(0)) {
        if (QWidget* w = item->widget()) w->deleteLater();
        delete item;
    }

    const int columns = calculateColumns();
    int row = 0, col = 0;

    for (const Recipe& r : recipes) {
        auto* card = new RecipeCard(r, ui->scrollAreaWidgetContents);
        card->setFixedSize(260, 260);
        m_gridLayout->addWidget(card, row, col);

        connect(card, &RecipeCard::cardClicked,
                this, &AllRecipesPage::cardClicked);
        connect(card, &RecipeCard::favoriteClicked,
                this, &AllRecipesPage::favoriteClicked);

        if (++col >= columns) { col = 0; ++row; }
    }
}

void AllRecipesPage::on_pushButton_Add_clicked()
{
    emit addClicked();
}

