/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

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

//    Visited = true;
    if (Visited) setPen(QPen(Qt::red, 2));
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
    QGraphicsPolygonItem::paint( painter, style);
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

