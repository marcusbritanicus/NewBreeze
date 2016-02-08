#ifndef DOCMARGIN_H
#define DOCMARGIN_H

#include <QWidget>
#include <QPainter>
#include <QPen>
#include <QFont>
#include <QRectF>
#include <QColor>
#include <QMouseEvent>
#include <QDebug>
#include <QtGlobal>


#define _FIX_ 17


#define POINT_TO_CM(cm) ((cm)/28.3465058)
#define POINT_TO_MM(mm) ((mm)/2.83465058)     ////////  0.352777778
#define POINT_TO_DM(dm) ((dm)/283.465058)
#define POINT_TO_INCH(inch) ((inch)/72.0)
#define POINT_TO_PI(pi) ((pi)/12)
#define POINT_TO_DD(dd) ((dd)/154.08124)
#define POINT_TO_CC(cc) ((cc)/12.840103)

#define MM_TO_POINT(mm) ((mm)*2.83465058)
#define CM_TO_POINT(cm) ((cm)*28.3465058)     ///// 28.346456693
#define DM_TO_POINT(dm) ((dm)*283.465058)
#define INCH_TO_POINT(inch) ((inch)*72.0)
#define PI_TO_POINT(pi) ((pi)*12)
#define DD_TO_POINT(dd) ((dd)*154.08124)
#define CC_TO_POINT(cc) ((cc)*12.840103)





qreal Pointo(qreal unit, const QString unita);
qreal FopInt(const QString datain);

class DocMargin : public QWidget {
    Q_OBJECT

public:
    DocMargin(QWidget *parent = 0);
    QSize sizeHint() const;
    QSize minimumSizeHint() const;
    void initmargin(qreal l , qreal r) {
        Cursor_1_X = l;
        Cursor_2_X = width() - r;
        update();
    }
    ~DocMargin();
    qreal Cursor_1_X = 1.0;
    qreal Cursor_2_X = 1.0;
signals:
    void CursorMove(qreal, qreal);
protected:
    void resizeEvent(QResizeEvent *event);
    bool event(QEvent * e);
    void paintEvent(QPaintEvent *);
    void paintScale();
    void paintCursor();
    qreal FAKTORX;
    QString units;
    QColor ColText;
    QRectF AreaCursor_1;
    QRectF AreaCursor_2;
    qreal actual_x;
    int MaximumCollisionAllowed;
    int dimfontsize = 8;
    int capturecursordiff = 0;
    int lastMove = 0;



};

#endif // DOCMARGIN_H
