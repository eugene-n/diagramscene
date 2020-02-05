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

#include "diagramscene.h"
#include "arrow.h"

#include <QTextCursor>
#include <QGraphicsSceneMouseEvent>


//! [0]
DiagramScene::DiagramScene(QMenu *itemMenu, QObject *parent)
    : QGraphicsScene(parent)
{
    myItemMenu = itemMenu;
    myMode = MoveItem;
    myItemType = DiagramItem::Step;
    line = nullptr;
    //textItem = nullptr;
    myItemColor = Qt::white;
    //myTextColor = Qt::black;
    myLineColor = Qt::black;
    myLineRedColor = Qt::red;
    allNodes = 0;
}
//! [0]

//! [1]
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
            item = new DiagramItem(myItemType, myItemMenu, allNodes++);
            item->setBrush(myItemColor);
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
        case InsertNode:
            item = new DiagramItem(myItemType, myItemMenu, allNodes++);
            item->setBrush(myItemColor);
            addItem(item);
            item->setPos(mouseEvent->scenePos());
//            emit itemInserted(item); //Это переключает на стрелку
            break;
//        case CreateList:
//            item = new DiagramItem(myItemType, myItemMenu, allNodes++);
//            item->setBrush(myItemColor);
//            addItem(item);
//            item->setPos(mouseEvent->scenePos());
//            emit itemInserted(item); //Это переключает на стрелку
//            break;
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

//// Deselect all nodes.
//public void DeselectNodes()
//{
//    foreach (NetworkNode node in AllNodes)
//        node.Visited = false;
//}

//// Deselect all nodes.
//public void DeselectBranches()
//{
//    foreach (NetworkNode node in AllNodes)
//        foreach (NetworkLink link in node.Links)
//            link.Visited = false;
//}
