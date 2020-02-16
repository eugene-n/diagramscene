#include "arrow.h"
#include "diagramitem.h"
#include "diagramscene.h"
#include "mainwindow.h"

#include <QtWidgets>

MainWindow::MainWindow()
{
    createActions();
    createToolBox();
    createMenus();

    scene = new DiagramScene(itemMenu, this);
    scene->setSceneRect(QRectF(0, 0, 5000, 5000));
    connect(scene, SIGNAL(itemInserted(DiagramItem*)),
            this, SLOT(itemInserted(DiagramItem*)));

    createToolbars();

    QHBoxLayout *layout = new QHBoxLayout;
    view = new QGraphicsView(scene);
    view->setRenderHint(QPainter::Antialiasing);
    layout->addWidget(view);

    QWidget *widget = new QWidget;
    widget->setLayout(layout);

    setCentralWidget(widget);
    setWindowTitle(tr("NetAlg"));
    setUnifiedTitleAndToolBarOnMac(true);
}

void MainWindow::deleteItem()
{
    foreach (QGraphicsItem *item, scene->selectedItems()) {
        if (item->type() == Arrow::Type) {
            scene->removeItem(item);
            Arrow *arrow = qgraphicsitem_cast<Arrow *>(item);
            arrow->startItem()->removeArrow(arrow);
            arrow->endItem()->removeArrow(arrow);
            delete item;
        }
    }

    foreach (QGraphicsItem *item, scene->selectedItems()) {
         if (item->type() == DiagramItem::Type)
             qgraphicsitem_cast<DiagramItem *>(item)->removeArrows();
         scene->removeItem(item);
         delete item;
     }
}

void MainWindow::testaction()
//void MainWindow::listCreate()
{
//    foreach (QGraphicsItem *item, scene->items(Qt::AscendingOrder)) //Сортировка в порядке возрастания
//    {
//        DiagramItem *node = qgraphicsitem_cast<DiagramItem *>(item);
//        if (node)
//        {
//            QString str = node->nameNode();
//            qDebug() << str;
//        }
//    }
//    qDebug() << "listCreate";
    scene->DeselectNodes();
    scene->DeselectBranches();
    scene->update();
}

void MainWindow::pointerGroupClicked(int)
{
    scene->setMode(DiagramScene::Mode(pointerTypeGroup->checkedId()));
}

void MainWindow::itemInserted(DiagramItem *)
{
    pointerTypeGroup->button(int(DiagramScene::MoveItem))->setChecked(true);
    scene->setMode(DiagramScene::Mode(pointerTypeGroup->checkedId()));
}

void MainWindow::about()
{
    QMessageBox::about(this, tr("About NetAlg"),
                       tr("The <b>NetAlg</b>"));
}

void MainWindow::listCreate()
//void MainWindow::testaction()
{
    foreach (QGraphicsItem *item, scene->items(Qt::AscendingOrder))
    {
        DiagramItem *node = qgraphicsitem_cast<DiagramItem *>(item);
        if (node)
            node->Visited = true;
    }
    foreach (QGraphicsItem *item, scene->items(Qt::AscendingOrder))
    {
        Arrow *node = qgraphicsitem_cast<Arrow *>(item);
        if (node)
            node->Visited = true;
    }
    scene->update();
}

void MainWindow::createToolBox()
{
    buttonGroup = new QButtonGroup(this);
    buttonGroup->setExclusive(false);
}

void MainWindow::createActions()
{
    deleteAction = new QAction(QIcon(":/images/delete.png"), tr("&Delete"), this);
    deleteAction->setShortcut(tr("Delete"));
    deleteAction->setStatusTip(tr("Delete item from diagram"));
    connect(deleteAction, SIGNAL(triggered()), this, SLOT(deleteItem()));


    listAction = new QAction(QIcon(":/images/pencil.png"), tr("&ListNodes"), this);
    listAction->setShortcut(tr("Ctrl+L"));
    listAction->setStatusTip(tr("Создание списков связных узлов"));
    connect(listAction, SIGNAL(triggered()), this, SLOT(listCreate()));

    testAction = new QAction(QIcon(":/images/background3.png"), tr("&Test"), this);
    testAction->setShortcut(tr("Ctrl+L"));
    testAction->setStatusTip(tr("Создание списков связных узлов"));
    connect(testAction, SIGNAL(triggered()), this, SLOT(testaction()));

    exitAction = new QAction(tr("E&xit"), this);
    exitAction->setShortcuts(QKeySequence::Quit);
    exitAction->setStatusTip(tr("Quit Scenediagram example"));
    connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));

    aboutAction = new QAction(tr("A&bout"), this);
    aboutAction->setShortcut(tr("F1"));
    connect(aboutAction, SIGNAL(triggered()), this, SLOT(about()));
}
void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(exitAction);

    itemMenu = menuBar()->addMenu(tr("&Item"));
    itemMenu->addAction(deleteAction);
//    itemMenu->addSeparator();
//    itemMenu->addAction(listAction);

    aboutMenu = menuBar()->addMenu(tr("&Help"));
    aboutMenu->addAction(aboutAction);
}

void MainWindow::createToolbars()
{
    QToolButton *pointerButton = new QToolButton;
    pointerButton->setCheckable(true);
    pointerButton->setChecked(true);
    pointerButton->setIcon(QIcon(":/images/pointer.png"));
    QToolButton *itemPointerButton = new QToolButton;
    itemPointerButton->setCheckable(true);
    itemPointerButton->setIcon(QIcon(":/images/addItem.png"));
    QToolButton *linePointerButton = new QToolButton;
    linePointerButton->setCheckable(true);
    linePointerButton->setIcon(QIcon(":/images/arrow1.png"));
    QToolButton *linePointerButtonDual = new QToolButton;
    linePointerButtonDual->setCheckable(true);
    linePointerButtonDual->setIcon(QIcon(":/images/arrow2.png"));

    pointerTypeGroup = new QButtonGroup(this);
    pointerTypeGroup->addButton(pointerButton, int(DiagramScene::MoveItem));
    pointerTypeGroup->addButton(itemPointerButton, int(DiagramScene::InsertItem));
    pointerTypeGroup->addButton(linePointerButton, int(DiagramScene::InsertLine));
    pointerTypeGroup->addButton(linePointerButtonDual, int(DiagramScene::InsertDualLine));
    connect(pointerTypeGroup, SIGNAL(buttonClicked(int)),
            this, SLOT(pointerGroupClicked(int)));

    pointerToolbar = addToolBar(tr("Pointer type"));
    pointerToolbar->addWidget(pointerButton);
    pointerToolbar->addWidget(itemPointerButton);
    pointerToolbar->addWidget(linePointerButton);
    pointerToolbar->addWidget(linePointerButtonDual);

    editToolBar = addToolBar(tr("Edit"));
    editToolBar->addAction(deleteAction);
    editToolBar = addToolBar(tr("List"));
    editToolBar->addAction(listAction);
    editToolBar->addAction(testAction);
}

QWidget *MainWindow::createBackgroundCellWidget(const QString &text, const QString &image)
{
    QToolButton *button = new QToolButton;
    button->setText(text);
    button->setIcon(QIcon(image));
    button->setIconSize(QSize(50, 50));
    button->setCheckable(true);
    backgroundButtonGroup->addButton(button);

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(button, 0, 0, Qt::AlignHCenter);
    layout->addWidget(new QLabel(text), 1, 0, Qt::AlignCenter);

    QWidget *widget = new QWidget;
    widget->setLayout(layout);

    return widget;
}

