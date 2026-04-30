#ifndef RECIPEIMAGESTORAGE_H
#define RECIPEIMAGESTORAGE_H

#include <QPixmap>
#include <QSize>
#include <QString>

class QLabel;

namespace RecipeImageStorage {

QString storageRoot();

/** Returns true if path is inside our app image directory. */
bool isManagedPath(const QString &absolutePath);

/**
 * Copies a user-selected image into storage as recipe_<id>.<ext>.
 * If sourcePath is empty, returns QString() (caller clears DB path).
 * If source is already a managed file for this id, returns cleaned path without copying.
 */
QString importForRecipe(int recipeId, const QString &sourcePath);

/** Scale like CSS object-fit: cover, then clip to rounded rect. */
QPixmap coverRoundPixmap(const QPixmap &source, const QSize &targetSize, int cornerRadius);
QPixmap coverTopRoundPixmap(const QPixmap &source, const QSize &targetSize, int topCornerRadius);

void updatePreviewLabel(QLabel *label, const QString &imagePath, int cornerRadius = 12);

} // namespace RecipeImageStorage

#endif
