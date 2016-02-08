#include "docmargin.h"

#include <QCursor>
#include <QMouseEvent>

qreal Pointo(qreal unit, const QString unita) {
    qreal ri = 0;
    if (unita == "cm") {
        ri = POINT_TO_CM(unit);
    } else if (unita == "pt" || unita == "px") {
        ri = unit;
        return ri;
    } else if (unita == "mm") {
        ri = POINT_TO_MM(unit);
    } else if (unita == "dm") {
        ri = POINT_TO_DM(unit);
    } else if (unita == "inch") {
        ri = POINT_TO_INCH(unit);
    } else if (unita == "pi") {
        ri = POINT_TO_PI(unit);
    } else if (unita == "dd") {
        ri = POINT_TO_DD(unit);
    } else if (unita == "cc") {
        ri = POINT_TO_CC(unit);
    } else {
        ri = 10;
    }
    return ri;
}

qreal FopInt(const QString datain) {
    QString ctmp = datain;
    const QString data = ctmp.replace(" ", "").trimmed();
    //////////qDebug() << "### request unit data->" << datain << " size->" << datain.size();
    qreal points = 0;
    if (data.size() < 1) {
        return points;
    }
    if (datain == "0") {
        return points;
    }
    if (data.endsWith("pt") || data.endsWith("px")) {
        points = data.left(data.length() - 2).toDouble();
        return points;
    } else if (data.endsWith("cm")) {
        double value = data.left(data.length() - 2).toDouble();
        points = CM_TO_POINT(value);
    } else if (data.endsWith("em")) {
        points = data.left(data.length() - 2).toDouble();
    } else if (data.endsWith("mm")) {
        double value = data.left(data.length() - 2).toDouble();
        points = MM_TO_POINT(value);
    } else if (data.endsWith("dm")) {
        double value = data.left(data.length() - 2).toDouble();
        points = DM_TO_POINT(value);
    } else if (data.endsWith("in")) {
        double value = data.left(data.length() - 2).toDouble();
        points = INCH_TO_POINT(value);
    } else if (data.endsWith("inch")) {
        double value = data.left(data.length() - 4).toDouble();
        points = INCH_TO_POINT(value);
    } else if (data.endsWith("pi")) {
        double value = data.left(data.length() - 4).toDouble();
        points = PI_TO_POINT(value);
    } else if (data.endsWith("dd")) {
        double value = data.left(data.length() - 4).toDouble();
        points = DD_TO_POINT(value);
    } else if (data.endsWith("cc")) {
        double value = data.left(data.length() - 4).toDouble();
        points = CC_TO_POINT(value);
    } else {
        points = 0;
    }


    return points;

}

DocMargin::DocMargin(QWidget *parent)
: QWidget(parent), units("mm"),
dimfontsize(8),
Cursor_1_X(MM_TO_POINT(15)),
Cursor_2_X(CM_TO_POINT(8)),FAKTORX(10),
actual_x(15), capturecursordiff(44) {
    ColText = Qt::black;
    MaximumCollisionAllowed = 180;
    setMaximumHeight(36);
    const qreal startleft = 10;
    const qreal startright = startleft + CM_TO_POINT(17);
    Cursor_1_X = startleft;
    Cursor_2_X = startright;
}

bool DocMargin::event(QEvent * e) {
    QMouseEvent *ke;

    //// MaximumCollisionAllowed
    bool movecursor = true;
    int limit_left = 10;
    int limitcenter1 = (this->width() / 2) - (MaximumCollisionAllowed / 2);
    int limitcenter2 = (this->width() / 2) + (MaximumCollisionAllowed / 2);
    int limit_right = this->width() - 15;

    if (e->type() == QEvent::MouseMove ||
            e->type() == QEvent::HoverEnter ||
            e->type() == QEvent::MouseMove ||
            e->type() == QEvent::MouseButtonPress) {
        ke = static_cast<QMouseEvent *> (e);
        if (ke) {
            QPoint now = ke->pos();
            const int Rsite = now.rx();
            if (Rsite > limitcenter2 && Rsite < limit_right) {
                if (AreaCursor_2.contains(ke->pos()) && movecursor) {
                    Cursor_2_X = Rsite;
                    setCursor(Qt::SplitHCursor);
                    update();
                } else {
                    ///QApplication::restoreOverrideCursor();
                    unsetCursor();
                }

            }
            if (Rsite > limit_left && Rsite < limitcenter1) {
                if (AreaCursor_1.contains(ke->pos()) && movecursor) {
                    Cursor_1_X = now.rx();
                    setCursor(Qt::SplitHCursor);
                    update();
                } else {
                    /////QApplication::restoreOverrideCursor();
                    unsetCursor();
                }
            }

            e->setAccepted(true);
            //// qDebug() << "### cursor pair " << Cursor_1_X << "," << Cursor_2_X;
            emit CursorMove(Cursor_1_X,Cursor_2_X);
        }
    } else if (e->type() == QEvent::MouseButtonDblClick) {
        ke = static_cast<QMouseEvent *> (e);
        if (ke) {
            e->setAccepted(true);
        }
    }
    return QWidget::event(e);
}

void DocMargin::resizeEvent(QResizeEvent *) {
    const int large = width();
    FAKTORX = width() / _FIX_;
    const qreal startleft = 10;
    const qreal startright = large - 40;   /// startleft + CM_TO_POINT(17);
    Cursor_1_X = startleft;
    Cursor_2_X = startright;
}


QSize DocMargin::minimumSizeHint() const {
    return QSize(100, 45);
}

QSize DocMargin::sizeHint() const {
    return QSize(700, 45);
}

void DocMargin::paintEvent(QPaintEvent *) {
    paintScale();
    paintCursor();
    ////return QWidget::paintEvent();
}

void DocMargin::paintScale() {
    const int large = width();
    const int xhi = height() / 5;

    QPainter painter(this);
    painter.setWindow(0, 0, large, 22);
    //////painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(QColor(245, 245, 245,255));
    painter.drawRect(rect());
    painter.setBrush(Qt::NoBrush);
    painter.setPen(QPen(Qt::darkGray, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter.drawLine(0, 1, large, 1);
   //// const uint linessumme_0 = large / lineseparator_0;
    ////const uint linessumme_1 = large / lineseparator_1;
    int PressUnit = -1;
    painter.setPen(QPen(Qt::darkGray,1));
    /////painter.setPen(QPen(ColText, 0.8, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    for (int i = 0; i <= _FIX_; i++) {
        PressUnit++;

        ////QString strnu = QString("%1").arg(i);
        const int isopos = FAKTORX * i;
        painter.drawLine(isopos, 1, isopos,xhi * 3);
        /////painter.drawRect(linepl);
        qDebug() << "### cursor pen " << i << "," << isopos;

        //// painter.drawLine(isopos,0,isopos,55);


    }
    /*
    painter.setPen(QPen(ColText, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    QFont valFont("Arial", dimfontsize, QFont::Normal);
    painter.setFont(valFont);
    for (int i = 0; i <= linessumme_0; i++) {
        const int LeftPointer0 = i * lineseparator_0 - 0.5;
        painter.drawLine(LeftPointer0, 5, LeftPointer0, 10);
        if (i > 0) {
            QString val = QString("%1").arg(i);
            QRectF valR(LeftPointer0 - (lineseparator_0 / 2), 11, lineseparator_0, dimfontsize + 2);
            painter.drawText(valR, Qt::AlignCenter, val);
        }
    }
*/
    painter.setPen(QPen(Qt::darkGray, 1, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter.drawLine(0, 21, large, 21);

    painter.end();
}

void DocMargin::paintCursor() {
    QPainter painter(this); ///QRect ( int x, int y, int width, int height )
    const int large = width();
    painter.setWindow(0, 0, large, 22);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(Qt::red);


    QRectF cursor(Cursor_1_X, 12, 5, 18);
    painter.setPen(QPen(Qt::red, 1.5));
    painter.drawLine(Cursor_1_X, 1, Cursor_1_X, 21);
    painter.drawLine(Cursor_2_X, 1, Cursor_2_X, 21);

    painter.setPen(QPen(Qt::darkRed, 1));
    QRectF rectangle_0(Cursor_1_X - 5, 18, 10, 15);
    QRectF rectangle_1(Cursor_1_X - 5, -10, 10, 15);

    QRectF brectangle_0(Cursor_2_X - 5, 18, 10, 15);
    QRectF brectangle_1(Cursor_2_X - 5, -10, 10, 15);
    bool showcapturearea = true;
    int rec_center = capturecursordiff / 2;

    AreaCursor_2 = QRectF(Cursor_2_X - rec_center, 0, capturecursordiff, maximumHeight());
    AreaCursor_1 = QRectF(Cursor_1_X - rec_center, 0, capturecursordiff, maximumHeight());

    if (showcapturearea) {
        painter.setPen(QPen(Qt::red,0));
        painter.setBrush(QColor(220, 220, 220,155));
        painter.drawRect(AreaCursor_2);
        painter.drawRect(AreaCursor_1);
    }
    painter.setPen(QPen(Qt::red, 1.5));
    painter.setBrush(Qt::red);

    painter.drawEllipse(rectangle_0);
    painter.drawEllipse(rectangle_1);

    painter.drawEllipse(brectangle_0);
    painter.drawEllipse(brectangle_1);

}

DocMargin::~DocMargin() {

}
