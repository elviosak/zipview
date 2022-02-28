#include "delegate.h"

void Delegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    painter->save();
    auto pix = index.data(Qt::UserRole + 1).value<QPixmap>();
    QApplication::style()->drawPrimitive(QStyle::PE_PanelItemViewItem, &option, painter);
    QApplication::style()->drawPrimitive(QStyle::PE_PanelItemViewRow, &option, painter);
    QApplication::style()->drawItemPixmap(
                painter
                , option.rect
                , Qt::AlignmentFlag::AlignCenter
                , pix
    );
    painter->restore();
}

QSize Delegate::sizeHint(const QStyleOptionViewItem &option,const QModelIndex &index) const
{
    return QSize(TWIDTH, THEIGHT);
}
