#ifndef DIAGRAMSCENE_H
#define DIAGRAMSCENE_H

#include "diagramitem.h"
#include <QGraphicsScene>
#include <QList>

QT_BEGIN_NAMESPACE
class QGraphicsSceneMouseEvent;
class QMenu;
class QPointF;
class QGraphicsLineItem;
class QFont;
class QColor;
QT_END_NAMESPACE

class DiagramScene : public QGraphicsScene
{
    Q_OBJECT

public:
    enum Mode { InsertItem, InsertLine, InsertText, MoveItem, InsertDualLine};

    explicit DiagramScene(QMenu *itemMenu, QObject *parent = nullptr);
    QColor itemColor() const { return myItemColor; }
    QColor lineColor() const { return myLineColor; }
    void setLineColor(const QColor &color);
    void setItemColor(const QColor &color);
    void DeselectNodes();
    void DeselectBranches();
    void ResetNetwork();

public slots:
    void setMode(Mode mode);
    void setItemType(DiagramItem::DiagramType type);

signals:
    void itemInserted(DiagramItem *item);
    void itemSelected(QGraphicsItem *item);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent) override;

private:
    bool isItemChange(int type);

    DiagramItem::DiagramType myItemType;
    QMenu *myItemMenu;
    Mode myMode;
    bool leftButtonDown;
    QPointF startPoint;
    QGraphicsLineItem *line;
    QList<QList<DiagramItem*>*> GetConnectedComponents();

    QColor myItemColor;
    QColor myLineColor;
    QColor myLineRedColor;
    quint16 allNodes;// = 0; //Так тоже можно инициализировать
};

#endif // DIAGRAMSCENE_H
