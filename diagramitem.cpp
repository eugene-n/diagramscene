#include "diagramitem.h"
#include "arrow.h"

#include <QGraphicsScene>
#include <QGraphicsSceneContextMenuEvent>
#include <QMenu>
#include <QPainter>
#include <QtDebug>

DiagramItem::DiagramItem(QMenu *contextMenu, quint16 allNodes,
             QGraphicsItem *parent)
    : QGraphicsPolygonItem(parent) //QGraphicsEllipseItem(parent)
{
    myContextMenu = contextMenu;
    nNode = allNodes;

//    Было 100
//    myPolygon << QPointF(-20, -20) << QPointF(20, -20)
//              << QPointF(20, 20) << QPointF(-20, 20)
//              << QPointF(-20, -20);
    myPolygon << QPointF(-15, -15) << QPointF(15, -15)
              << QPointF(15, 15) << QPointF(-15, 15)
              << QPointF(-15, -15);

    setPolygon(myPolygon);
    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
}

void DiagramItem::removeArrow(Arrow *arrow)
{
    int index = arrows.indexOf(arrow);

    if (index != -1)
        arrows.removeAt(index);
}

void DiagramItem::removeArrows()
{
    foreach (Arrow *arrow, arrows) {
        arrow->startItem()->removeArrow(arrow);
        arrow->endItem()->removeArrow(arrow);
        scene()->removeItem(arrow);
        delete arrow;
    }
}

void DiagramItem::addArrow(Arrow *arrow)
{
    arrows.append(arrow);
}

QPixmap DiagramItem::image() const
{
    QPixmap pixmap(250, 250);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    painter.setPen(QPen(Qt::black, 8));
    painter.translate(125, 125);
    painter.drawPolyline(myPolygon);

    return pixmap;
}



void DiagramItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    scene()->clearSelection();
    setSelected(true);
    myContextMenu->exec(event->screenPos());
}

QVariant DiagramItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == QGraphicsItem::ItemPositionChange) {
        foreach (Arrow *arrow, arrows) {
            arrow->updatePosition();
        }
    }

    return value;
}

// Return a number converted into letters
// as in A, B, C, ..., AA, AB, AC, ..., BA, BB, BC, ...
QString DiagramItem::NumberToLetters(quint16 number)
{
    QString result = "";
    quint16 n = number;
    do
    {
        quint16 letterNum = number % 26;
        n /= 26;
        number = (number - 26)/26;
        QChar ch = QChar('A' + letterNum);
        result = ch + result;
    } while (n > 0);
    return result;
}

QString DiagramItem::nameNode()
{
    return NumberToLetters(nNode);
}

void DiagramItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *style, QWidget *)
{
    if (Visited) setPen(QPen(Qt::red, 2));
    QGraphicsPolygonItem::paint( painter, style);
    painter->setPen(QPen(Qt::black, 2));
    painter->drawText(-5, 4, nameNode());
}


//    class RectItem : public QObject, public QGraphicsRectItem
//    {
//      Q_OBJECT
//      public:
//        RectItem (std::string text=0, QGraphicsItem *parent = 0);
//        void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
//          QWidget *widget = 0);

//    private:
//      std::string m_text;
//    };

//    RectItem::RectItem (std::string text, QGraphicsItem *parent)
//    : QGraphicsRectItem(100, 100, 100, 30, parent)
//    {
//      m_text = text;
//    }

//    void RectItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
//                            QWidget *widget)
//    {
//      QPointF pos = QPointF(rect().x() + 10, rect().y() + 20);
//      painter->drawText(pos, m_text.c_str());
//      painter->setPen(QPen(Qt::black, 1));
//      painter->drawRoundRect(rect());
//    }

