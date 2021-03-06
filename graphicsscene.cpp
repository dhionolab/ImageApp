#include "graphicsscene.h"
#include <QGraphicsView>
#include <QGraphicsSceneMouseEvent>

GraphicsScene::GraphicsScene(QObject *parent) : QGraphicsScene(parent)
{
    isPressed = false;
}

void GraphicsScene::addImage(QString fileName)
{
    pixmap.load(fileName);
    pixmapItem = addPixmap(pixmap);
    pixmapItem->setPos(0,0);

    views().first()->centerOn(pixmapItem);

    line1 = addLine(0,0,pixmap.width(),0,QPen(QBrush(QColor(Qt::black)),3));
    line2 = addLine(0,0,0,pixmap.height(),QPen(QBrush(QColor(Qt::black)),3));
}

void GraphicsScene::showTwoLine(QPointF p)
{
    line1->setPos(0,p.y());
    line2->setPos(p.x(),0);
    line1->show();
    line2->show();
}

void GraphicsScene::hideTwoLine(QPointF p)
{
    line1->hide();
    line2->hide();

    MyGraphicsRectItem *item = new MyGraphicsRectItem(QRectF(p.x(),p.y(),0,0));
    addItem(item);
    connect(item, &MyGraphicsRectItem::hover_enter,  [=](){current = item;});
    current = item;

    groupItems << item;
}

void GraphicsScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if(pixmap.isNull())
    {
        return;
    }
    if (mouseEvent->button() != Qt::LeftButton)
    {
        return;
    }

    if(step1)
    {
        hideTwoLine(mouseEvent->scenePos());
        step1 = false;
        step2 = true;
    }
    if(step3)
    {
        step2 = false;
        step3 = false;
        current->createGrabbers();
    }
    if(current != nullptr)
    {
        if(current->grabbersAreCreated())
        {
            if(current->checkCornerGrabbers() != -1)
            {
                isPressed = true;
            }
        }
    }
}

void GraphicsScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if(pixmap.isNull())
    {
        return;
    }
    if(step1)
    {
        showTwoLine(event->scenePos());
    }
    if(step2)
    {
        qreal x = current->rect().x();
        qreal y = current->rect().y();
        qreal nx = event->scenePos().x();
        qreal ny = event->scenePos().y();
        qreal w = nx - x;
        qreal h = ny - y;
        current->setRect(x, y, w, h);
        step3 = true;
    }
    if(isPressed)
    {
        qreal x1 = current->rect().x();
        qreal y1 = current->rect().y();

        qreal x2 = current->rect().bottomRight().x();
        qreal y2 = current->rect().bottomRight().y();

        qreal px = event->scenePos().x();
        qreal py = event->scenePos().y();

        if(px<0)
        {
            px = 0;
        }
        if(px>pixmap.width())
        {
            px = pixmap.width();
        }
        if(py<0)
        {
            py = 0;
        }
        if(py>pixmap.height())
        {
            py = pixmap.height();
        }

        switch(current->checkCornerGrabbers())
        {
        case CornerGrabber::bottomLeft:
            current->setRect(QRectF(QPointF(px<x2?px:x2,y1),QPointF(x2,py>y1?py:y1)));
            break;
        case CornerGrabber::topLeft:
            current->setRect(QRectF(QPointF(px<x2?px:x2,py<y2?py:y2),QPointF(x2,y2)));
            break;
        case CornerGrabber::topRight:
            current->setRect(QRectF(QPointF(x1,py<y2?py:y2),QPointF(px>x1?px:x1,y2)));
            break;
        case CornerGrabber::bottomRight:
            current->setRect(QRectF(QPointF(x1,y1),QPointF(px>x1?px:x1,py>y1?py:y1)));
            break;
        }
        current->setCornerPositions();
        current->update();
    }

    QGraphicsScene::mouseMoveEvent(event);
}

void GraphicsScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    if(pixmap.isNull())
    {
        return;
    }
    if (mouseEvent->button() != Qt::LeftButton)
    {
        return;
    }
    isPressed = false;
    QGraphicsScene::mouseReleaseEvent(mouseEvent);
}
