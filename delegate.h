#ifndef DELEGATE_H
#define DELEGATE_H

#include <QStyledItemDelegate>
#include <QSize>
#include <QPixmap>
#include <QApplication>
#include <QPainter>

#define TWIDTH 128
#define THEIGHT 192

class Delegate : public QStyledItemDelegate
{
    using QStyledItemDelegate::QStyledItemDelegate;
public:
    void paint(QPainter *painter,
               const QStyleOptionViewItem &option,
               const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};

#endif // DELEGATE_H
