#include "diagramscene.h"
#include "arrow.h"

#include <QTextCursor>
#include <QGraphicsSceneMouseEvent>

DiagramScene::DiagramScene(QMenu *itemMenu, QObject *parent)
    : QGraphicsScene(parent)
{
    myItemMenu = itemMenu;
    myMode = MoveItem;
    myItemType = DiagramItem::Step;
    line = nullptr;
    myItemColor = Qt::white;
    myLineColor = Qt::black;
    myLineRedColor = Qt::red;
    allNodes = 0;
}

void DiagramScene::setLineColor(const QColor &color)
{
    myLineColor = color;
    if (isItemChange(Arrow::Type)) {
        Arrow *item = qgraphicsitem_cast<Arrow *>(selectedItems().first());
        item->setColor(myLineColor);
        update();
    }
}

void DiagramScene::setItemColor(const QColor &color)
{
    myItemColor = color;
    if (isItemChange(DiagramItem::Type)) {
        DiagramItem *item = qgraphicsitem_cast<DiagramItem *>(selectedItems().first());
        item->setBrush(myItemColor);
    }
}

void DiagramScene::setMode(Mode mode)
{
    myMode = mode;
}

void DiagramScene::setItemType(DiagramItem::DiagramType type)
{
    myItemType = type;
}

void DiagramScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if (mouseEvent->button() != Qt::LeftButton)
        return;

    DiagramItem *item;
    switch (myMode) {
        case InsertItem:            
            item = new DiagramItem(myItemMenu, allNodes++);
            item->setBrush(myItemColor);
//            item->setPen(QPen(myLineRedColor));
            addItem(item);
            item->setPos(mouseEvent->scenePos());
//            emit itemInserted(item); //Это переключает на стрелку
            break;

        case InsertLine:
        case InsertDualLine:
            line = new QGraphicsLineItem(QLineF(mouseEvent->scenePos(),
                                        mouseEvent->scenePos()));
            line->setPen(QPen(myLineColor, 2));
            addItem(line);
            break;
    default:
        ;
    }
    QGraphicsScene::mousePressEvent(mouseEvent);
}

void DiagramScene::mouseMoveEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if ((myMode == InsertLine || myMode == InsertDualLine) && line != nullptr) {
        QLineF newLine(line->line().p1(), mouseEvent->scenePos());
        line->setLine(newLine);
    } else if (myMode == MoveItem) {
        QGraphicsScene::mouseMoveEvent(mouseEvent);
    }
}

void DiagramScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if (line != nullptr && (myMode == InsertLine || myMode == InsertDualLine)) {
        QList<QGraphicsItem *> startItems = items(line->line().p1());
        if (startItems.count() && startItems.first() == line)
            startItems.removeFirst();
        QList<QGraphicsItem *> endItems = items(line->line().p2());
        if (endItems.count() && endItems.first() == line)
            endItems.removeFirst();

        removeItem(line);
        delete line;

        if (startItems.count() > 0 && endItems.count() > 0 &&
            startItems.first()->type() == DiagramItem::Type &&
            endItems.first()->type() == DiagramItem::Type &&
            startItems.first() != endItems.first()) {
            DiagramItem *startItem = qgraphicsitem_cast<DiagramItem *>(startItems.first());
            DiagramItem *endItem = qgraphicsitem_cast<DiagramItem *>(endItems.first());

            Arrow *arrow = new Arrow(startItem, endItem);
            arrow->setColor(myLineColor);
            startItem->addArrow(arrow);
            endItem->addArrow(arrow);
            arrow->setZValue(-1000.0);
            addItem(arrow);
            arrow->updatePosition();

            //Двунаправленная связь - добавляется обратка
            if( myMode == InsertDualLine)
            {
                    arrow = new Arrow(endItem, startItem);
                    arrow->setColor(myLineColor);
                    startItem->addArrow(arrow);
                    endItem->addArrow(arrow);
                    arrow->setZValue(-1000.0);
                    addItem(arrow);
                    arrow->updatePosition();
            }
        }
    }
    line = nullptr;
    QGraphicsScene::mouseReleaseEvent(mouseEvent);
}

bool DiagramScene::isItemChange(int type)
{
    foreach (QGraphicsItem *item, selectedItems()) {
        if (item->type() == type)
            return true;
    }
    return false;
}

//private void connectedComponentsToolStripButton_Click(object sender, EventArgs e)
//{
//    // Find the connected components.
//    List<List<NetworkNode>> components =
//        TheNetwork.GetConnectedComponents();

//    // Display the components.
//    string txt = "";
//    for (int i = 0; i < components.Count; i++)
//    {
//        List<NetworkNode> component = components[i];
//        txt += ". Component " + i + ": ";
//        foreach (NetworkNode componentNode in component)
//            txt += " " + componentNode.ToString();
//    }
//    txt = txt.Substring(2);
//    toolStripStatusLabel1.Text = txt;
//    Console.WriteLine(txt);

//    // Redraw the network.
//    networkPictureBox.Refresh();
//}

//public List<List<NetworkNode>> GetConnectedComponents()
// {
//     // Reset the network.
//     ResetNetwork();

//     // Keep track of the number of nodes visited.
//     int numVisited = 0;

//     // Make the result list of lists.
//     List<List<NetworkNode>> components = new List<List<NetworkNode>>();

//     // Repeat until all nodes are in a connected component.
//     while (numVisited < AllNodes.Count)
//     {
//         // Find a node that hasn't been visited.
//         NetworkNode startNode = null;
//         foreach (NetworkNode node in AllNodes)
//             if (!node.Visited)
//             {
//                 startNode = node;
//                 break;
//             }

//         // Make sure we found one.
//         Debug.Assert(startNode != null);

//         // Add the start node to the stack.
//         Stack<NetworkNode> stack = new Stack<NetworkNode>();
//         stack.Push(startNode);
//         startNode.Visited = true;
//         numVisited++;

//         // Add the node to a new connected component.
//         List<NetworkNode> component = new List<NetworkNode>();
//         components.Add(component);
//         component.Add(startNode);

//         // Process the stack until it's empty.
//         while (stack.Count > 0)
//         {
//             // Get the next node from the stack.
//             NetworkNode node = stack.Pop();

//             // Process the node's links.
//             foreach (NetworkLink link in node.Links)
//             {
//                 // Only use the link if the destination
//                 // node hasn't been visited.
//                 NetworkNode toNode = link.Nodes[1];
//                 if (!toNode.Visited)
//                 {
//                     // Mark the node as visited.
//                     toNode.Visited = true;

//                     // Mark the link as part of the tree.
//                     link.Visited = true;
//                     numVisited++;

//                     // Add the node to the current connected component.
//                     component.Add(toNode);

//                     // Push the node onto the stack.
//                     stack.Push(toNode);
//                 }
//             }
//         }
//     }

//     // Return the components.
//     return components;
// }

//// Reset the network.
//private void ResetNetwork()
//{
//    // Deselect all nodes and branches.
//    DeselectNodes();
//    DeselectBranches();

//    // Clear the nodes' Text properties.
//    foreach (NetworkNode node in AllNodes)
//    {
//        node.FromLink = null;
//        node.FromNode = null;
//        node.Text = null;
//    }
//}

// Deselect all nodes.
void DiagramScene::DeselectNodes()
{
//    foreach (NetworkNode node in AllNodes)
//        node.Visited = false;
}

// Deselect all nodes.
void DiagramScene::DeselectBranches()
{
//    foreach (NetworkNode node in AllNodes)
//        foreach (NetworkLink link in node.Links)
//            link.Visited = false;
}


