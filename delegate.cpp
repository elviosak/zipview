#include "delegate.h"

#include <QBitmap>
#include <QDebug>


void Delegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    auto pix = index.data(Qt::DecorationRole).value<QPixmap>();
    if (pix.isNull()) {
        emit requestImage(index);
        return;
    }

//    QPixmap pix;
//    pix.loadFromData(index.data(Qt::UserRole + 5).toByteArray());

    painter->save();
    pix = pix.scaled(option.rect.size(), Qt::KeepAspectRatio, Qt::TransformationMode::SmoothTransformation);
    QApplication::style()->drawPrimitive(QStyle::PE_PanelItemViewItem, &option, painter);
    QApplication::style()->drawPrimitive(QStyle::PE_PanelItemViewRow, &option, painter);
    QApplication::style()->drawItemPixmap(
                painter
                , option.rect
                , Qt::AlignmentFlag::AlignAbsolute
                , pix
    );
    painter->restore();
}

QSize Delegate::sizeHint(const QStyleOptionViewItem &option,const QModelIndex &index) const
{
    auto size = index.data(Qt::SizeHintRole).value<QSize>();
    return size.scaled(option.rect.width(), 0, Qt::KeepAspectRatioByExpanding);//.grownBy(QMargins(0, 4, 0, 0));
}
