#include "RecipeGridBrowser.h"
#include "ui_recipegridbrowser.h"
#include <QResizeEvent>
#include <QDebug>
#include <algorithm>

RecipeGridBrowser::RecipeGridBrowser(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::RecipeGridBrowser)
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
        m_gridLayout = qobject_cast<QGridLayout *>(
            ui->scrollAreaWidgetContents->layout());
    }

    ui->scrollAreaWidgetContents->setSizePolicy(
        QSizePolicy::Expanding, QSizePolicy::Expanding);

    m_resizeTimer = new QTimer(this);
    m_resizeTimer->setSingleShot(true);
    m_resizeTimer->setInterval(120);
    connect(m_resizeTimer, &QTimer::timeout, this, [this] {
        loadCards(m_filteredRecipes);
    });

    setupCategoryButtons();
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

void RecipeGridBrowser::setAddButtonVisible(bool visible)
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
    const QList<QPair<QPushButton *, QString>> buttons = {
        { ui->pushButton_All, QStringLiteral("Wszystkie") },
        { ui->pushButton_Breakfast, QStringLiteral("Śniadania") },
        { ui->pushButton_Breakfast2, QStringLiteral("Obiady") },
        { ui->pushButton_Deserts, QStringLiteral("Desery") },
        { ui->pushButton_Evening, QStringLiteral("Kolacje") },
        { ui->pushButton_Another, QStringLiteral("Inne") },
    };

    for (auto &pair : buttons) {
        connect(pair.first, &QPushButton::clicked,
                this, [this, cat = pair.second] { filterByCategory(cat); });
    }
}

void RecipeGridBrowser::applyFilters()
{
    QList<Recipe> result = m_allRecipes;

    if (m_currentCategory != QStringLiteral("Wszystkie")) {
        result.erase(
            std::remove_if(result.begin(), result.end(),
                           [this](const Recipe &r) {
                               return r.category != m_currentCategory;
                           }),
            result.end());
    }

    const QString text = ui->lineEdit_Search->text().trimmed();
    if (!text.isEmpty()) {
        result.erase(
            std::remove_if(result.begin(), result.end(),
                           [&text](const Recipe &r) {
                               return !r.name.contains(text, Qt::CaseInsensitive);
                           }),
            result.end());
    }

    m_filteredRecipes = result;
    loadCards(m_filteredRecipes);
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
    while (QLayoutItem *item = m_gridLayout->takeAt(0)) {
        if (QWidget *w = item->widget())
            w->deleteLater();
        delete item;
    }

    const int columns = calculateColumns();
    int row = 0, col = 0;

    for (const Recipe &r : recipes) {
        auto *card = new RecipeCard(r, ui->scrollAreaWidgetContents);
        card->setFixedSize(300, 300);
        m_gridLayout->addWidget(card, row, col);

        connect(card, &RecipeCard::cardClicked,
                this, &RecipeGridBrowser::cardClicked);
        connect(card, &RecipeCard::favoriteClicked,
                this, &RecipeGridBrowser::favoriteClicked);

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
