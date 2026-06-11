#ifndef RECIPEIMAGESTORAGE_H
#define RECIPEIMAGESTORAGE_H

#include <QPixmap>
#include <QSize>
#include <QString>

class QLabel;

namespace RecipeImageStorage {

QString storageRoot();

//returns true if path is
bool isManagedPath(const QString &absolutePath);

QString importForRecipe(int recipeId, const QString &sourcePath);

//cover, then clip to rounded rect
QPixmap coverRoundPixmap(const QPixmap &source, const QSize &targetSize, int cornerRadius);
QPixmap coverTopRoundPixmap(const QPixmap &source, const QSize &targetSize, int topCornerRadius);

void updatePreviewLabel(QLabel *label, const QString &imagePath, int cornerRadius = 12);

}

#endif
