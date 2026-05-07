#include "AddRecipePage.h"
#include "ui_AddRecipePage.h"
#include "utils/RecipeImageStorage.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QShowEvent>
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

void AddRecipePage::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    refreshImagePreview();
}

void AddRecipePage::clearForm()
{
    m_editId = -1;
    m_editFavorite = false;
    m_selectedImagePath.clear();

    ui->lineEdit_Name->clear();
    ui->lineEdit_PrepTime->clear();
    ui->lineEdit_CookTime->clear();
    ui->plainTextEdit_Ingredients->clear();
    ui->plainTextEdit_Notes->clear();
    ui->comboBox_Category->setCurrentIndex(0);
    ui->comboBox_Difficulty->setCurrentIndex(0);

    ui->label_PageTitle->setText(QStringLiteral("Dodaj nowy przepis"));
    ui->label_ImagePreview->clear();
    ui->label_ImagePreview->setText(QStringLiteral("Brak zdjęcia"));
}

void AddRecipePage::loadForEdit(const Recipe &recipe)
{
    m_editId = recipe.id;
    m_editFavorite = recipe.isFavorite;
    m_selectedImagePath = recipe.imagePath.trimmed();

    ui->lineEdit_Name->setText(recipe.name);
    ui->lineEdit_PrepTime->setText(QString::number(recipe.prepTime));
    ui->lineEdit_CookTime->setText(QString::number(recipe.cookTime));
    QString ingredients = recipe.ingredients;
    QString notes = recipe.notes;
    ui->plainTextEdit_Ingredients->setPlainText(ingredients.replace("\\n", "\n"));
    ui->plainTextEdit_Notes->setPlainText(notes.replace("\\n", "\n"));

    const int catIdx = ui->comboBox_Category->findText(recipe.category);
    if (catIdx >= 0)
        ui->comboBox_Category->setCurrentIndex(catIdx);

    const int difIdx = ui->comboBox_Difficulty->findText(recipe.difficulty);
    if (difIdx >= 0)
        ui->comboBox_Difficulty->setCurrentIndex(difIdx);

    ui->label_PageTitle->setText(QStringLiteral("Edytuj przepis"));
    refreshImagePreview();
}

void AddRecipePage::refreshImagePreview()
{
    RecipeImageStorage::updatePreviewLabel(ui->label_ImagePreview, m_selectedImagePath);
}

void AddRecipePage::on_pushButton_SelectImage_clicked()
{
    const QString path = QFileDialog::getOpenFileName(
        this,
        QStringLiteral("Wybierz zdjęcie"),
        QString(),
        QStringLiteral("Obrazy (*.png *.jpg *.jpeg *.webp *.bmp);;Wszystkie pliki (*)"));
    if (path.isEmpty())
        return;

    m_selectedImagePath = path;
    refreshImagePreview();
}

void AddRecipePage::on_pushButton_ClearImage_clicked()
{
    m_selectedImagePath.clear();
    ui->label_ImagePreview->clear();
    ui->label_ImagePreview->setText(QStringLiteral("Brak zdjęcia"));
}

void AddRecipePage::on_pushButton_clicked()
{
    const Recipe r = collectFormData();
    if (!validate(r))
        return;

    emit recipeSaved(r);
}

void AddRecipePage::on_pushButton_Cancel_clicked()
{
    const int returnToDetailsId = (m_editId > 0) ? m_editId : 0;
    clearForm();
    emit cancelled(returnToDetailsId);
}

Recipe AddRecipePage::collectFormData() const
{
    Recipe r;
    r.id = m_editId;
    r.name = ui->lineEdit_Name->text().trimmed();
    r.prepTime = ui->lineEdit_PrepTime->text().toInt();
    r.cookTime = ui->lineEdit_CookTime->text().toInt();
    r.ingredients = ui->plainTextEdit_Ingredients->toPlainText().trimmed();
    r.notes = ui->plainTextEdit_Notes->toPlainText().trimmed();
    r.category = ui->comboBox_Category->currentText();
    r.difficulty = ui->comboBox_Difficulty->currentText();
    r.isFavorite = (m_editId >= 0) ? m_editFavorite : false;
    r.imagePath = m_selectedImagePath.trimmed();
    return r;
}

bool AddRecipePage::validate(const Recipe &r) const
{
    if (r.name.isEmpty()) {
        QMessageBox::warning(const_cast<AddRecipePage *>(this),
                             QStringLiteral("Uwaga"),
                             QStringLiteral("Podaj nazwę przepisu!"));
        return false;
    }
    return true;
}
