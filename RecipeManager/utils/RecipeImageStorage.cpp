#include "RecipeImageStorage.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QLabel>
#include <QPainter>
#include <QPainterPath>
#include <QPixmap>
#include <QSize>
#include <QStandardPaths>

namespace RecipeImageStorage {

QString storageRoot()
{
    const QString root = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)
        + QStringLiteral("/RecipeManager/images");//save in standart folder for app date
    QDir().mkpath(root);//create all folders what needed
    return root; //return correct path
}

//check if the image are already in folder or no(for not dublicat)
bool isManagedPath(const QString &absolutePath)
{
    if (absolutePath.isEmpty())
        return false;
    const QString clean = QFileInfo(absolutePath).canonicalFilePath();//clean path
    const QString root = QFileInfo(storageRoot()).canonicalFilePath();
    if (root.isEmpty() || clean.isEmpty())
        return false;
    return clean.startsWith(root);//check if the start of path is root
}

//path from user -> copy to root folder -> return new path
QString importForRecipe(int recipeId, const QString &sourcePath)
{
    if (sourcePath.trimmed().isEmpty())
        return {};

    const QFileInfo srcInfo(sourcePath);
    if (!srcInfo.isFile() || !srcInfo.exists())//check if it is file / if it is on pc
        return {};

    const QString cleanSrc = srcInfo.canonicalFilePath();
    if (isManagedPath(cleanSrc))
        return cleanSrc;

    const QString ext = srcInfo.suffix().isEmpty() ? QStringLiteral("jpg") : srcInfo.suffix(); //check if file has .jpg/.png if not set jpg as default
    const QString destPath = storageRoot() + QStringLiteral("/recipe_%1.%2").arg(recipeId).arg(ext);

    if (QFile::exists(destPath))//check if it is a file with the same name already
        QFile::remove(destPath);

    if (!QFile::copy(cleanSrc, destPath)) 
        return {};

    return QFileInfo(destPath).canonicalFilePath();
}





QPixmap coverRoundPixmap(const QPixmap &source, const QSize &targetSize, int cornerRadius)
{
    if (source.isNull() || targetSize.width() < 1 || targetSize.height() < 1)
        return {};

    QPixmap scaled = source.scaled(targetSize, Qt::KeepAspectRatioByExpanding, //keep proportions and fill whole area (400x200 -> 600x300 (if needed 300x300))
                                   Qt::SmoothTransformation);
    const int x = qMax(0, (scaled.width() - targetSize.width()) / 2); // (600 - 300) / 2 = 150(from right and 150 from left side cut)
    const int y = qMax(0, (scaled.height() - targetSize.height()) / 2);
    QPixmap cropped = scaled.copy(x, y, targetSize.width(), targetSize.height());

    QPixmap out(targetSize);
    out.fill(Qt::transparent);

    QPainter painter(&out);//create painter for drawing
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

    //paint image
    QPainterPath clip;
    const qreal r = qMin(cornerRadius, qMin(targetSize.width(), targetSize.height()) / 2); //qreal = double(for graphic)
    clip.addRoundedRect(0, 0, targetSize.width(), targetSize.height(), r, r);
    painter.setClipPath(clip); //allow drawing only inside rounded rect
    painter.drawPixmap(0, 0, cropped); //draw
    return out;
}

QPixmap coverTopRoundPixmap(const QPixmap &source, const QSize &targetSize, int topCornerRadius)
{
    if (source.isNull() || targetSize.width() < 1 || targetSize.height() < 1)
        return {};

    QPixmap scaled = source.scaled(targetSize, Qt::KeepAspectRatioByExpanding,
                                   Qt::SmoothTransformation);
    const int x = qMax(0, (scaled.width() - targetSize.width()) / 2);
    const int y = qMax(0, (scaled.height() - targetSize.height()) / 2);
    QPixmap cropped = scaled.copy(x, y, targetSize.width(), targetSize.height());

    QPixmap out(targetSize);
    out.fill(Qt::transparent);

    QPainter painter(&out);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

    const qreal r = qMin(topCornerRadius, qMin(targetSize.width(), targetSize.height()) / 2);
    QPainterPath clip;
    clip.moveTo(0, targetSize.height());
    clip.lineTo(0, r);
    clip.quadTo(0, 0, r, 0);
    clip.lineTo(targetSize.width() - r, 0);
    clip.quadTo(targetSize.width(), 0, targetSize.width(), r);
    clip.lineTo(targetSize.width(), targetSize.height());
    clip.closeSubpath();

    painter.setClipPath(clip);
    painter.drawPixmap(0, 0, cropped);
    return out;
}

void updatePreviewLabel(QLabel *label, const QString &imagePath, int cornerRadius)
{
    if (!label)
        return;

    if (imagePath.isEmpty() || !QFile::exists(imagePath)) {
        label->clear();
        label->setText(QStringLiteral("Brak zdjęcia"));
        return;
    }

    QPixmap pm(imagePath);
    if (pm.isNull()) {
        label->setText(QStringLiteral("Nie można wczytać pliku"));
        return;
    }

    label->setText({});
    QSize box = label->size();
    if (box.width() < 50 || box.height() < 50)
        box = label->minimumSize().isValid() ? label->minimumSize() : QSize(320, 200);
    label->setPixmap(coverRoundPixmap(pm, box, cornerRadius));
}

} // namespace RecipeImageStorage
