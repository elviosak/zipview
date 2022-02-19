#ifndef DELEGATE_H
#define DELEGATE_H

#include <QStyledItemDelegate>
#include <QSize>
#include <QPixmap>
#include <QApplication>
#include <QPainter>

//#include "view.h"

//#include <quazip/quazip.h>
//#include <quazip/quazipfile.h>

#define TWIDTH 128
#define THEIGHT 192

class Delegate : public QStyledItemDelegate
{
    Q_OBJECT
    using QStyledItemDelegate::QStyledItemDelegate;
public:
//    View* listView;
    void paint(QPainter *painter,
               const QStyleOptionViewItem &option,
               const QModelIndex &index) const override;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;
signals:
    void requestImage(const QModelIndex &index) const;
};

#endif // DELEGATE_H
