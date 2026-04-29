#include "AddRecipePage.h"
#include "ui_AddRecipePage.h"
#include <QMessageBox>
#include <QStyledItemDelegate>

AddRecipePage::AddRecipePage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::AddRecipePage)
{
    ui->setupUi(this);
    
    setAttribute(Qt::WA_StyledBackground, true);

    ui->comboBox_Category->setItemDelegate(new QStyledItemDelegate(this));
    ui->comboBox_Difficulty->setItemDelegate(new QStyledItemDelegate(this));
}

AddRecipePage::~AddRecipePage()
{
    delete ui;
}

// ─────────────────────────────────────────
//  Public
// ─────────────────────────────────────────

void AddRecipePage::clearForm()
{
    m_editId = -1;
    ui->lineEdit_Name->clear();
    ui->lineEdit_PrepTime->clear();
    ui->lineEdit_CookTime->clear();
    ui->plainTextEdit_Ingredients->clear();
    ui->plainTextEdit_Notes->clear();
    ui->comboBox_Category->setCurrentIndex(0);
    ui->comboBox_Difficulty->setCurrentIndex(0);
}

void AddRecipePage::loadForEdit(const Recipe& recipe)
{
    m_editId = recipe.id;
    ui->lineEdit_Name->setText(recipe.name);
    ui->lineEdit_PrepTime->setText(QString::number(recipe.prepTime));
    ui->lineEdit_CookTime->setText(QString::number(recipe.cookTime));
    ui->plainTextEdit_Ingredients->setPlainText(recipe.ingredients);
    ui->plainTextEdit_Notes->setPlainText(recipe.notes);

    const int catIdx = ui->comboBox_Category->findText(recipe.category);
    if (catIdx >= 0) ui->comboBox_Category->setCurrentIndex(catIdx);

    const int difIdx = ui->comboBox_Difficulty->findText(recipe.difficulty);
    if (difIdx >= 0) ui->comboBox_Difficulty->setCurrentIndex(difIdx);
}

// ─────────────────────────────────────────
//  Private slots
// ─────────────────────────────────────────

void AddRecipePage::on_pushButton_clicked()
{
    const Recipe r = collectFormData();
    if (!validate(r)) return;

    // Page does NOT write to DB — MainWindow owns that responsibility
    emit recipeSaved(r);
    clearForm();
}

void AddRecipePage::on_pushButton_Cancel_clicked()
{
    clearForm();
    emit cancelled();
}

// ─────────────────────────────────────────
//  Private helpers
// ─────────────────────────────────────────

Recipe AddRecipePage::collectFormData() const
{
    Recipe r;
    r.id          = m_editId;
    r.name        = ui->lineEdit_Name->text().trimmed();
    r.prepTime    = ui->lineEdit_PrepTime->text().toInt();
    r.cookTime    = ui->lineEdit_CookTime->text().toInt();
    r.ingredients = ui->plainTextEdit_Ingredients->toPlainText().trimmed();
    r.notes       = ui->plainTextEdit_Notes->toPlainText().trimmed();
    r.category    = ui->comboBox_Category->currentText();
    r.difficulty  = ui->comboBox_Difficulty->currentText();
    return r;
}

bool AddRecipePage::validate(const Recipe& r) const
{
    if (r.name.isEmpty()) {
        QMessageBox::warning(const_cast<AddRecipePage*>(this),
            "Uwaga", "Podaj nazwę przepisu!");
        return false;
    }
    return true;
}