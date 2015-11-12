/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
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

#include "renderarea.h"

#include <QPainter>
#include <QPaintEvent>

RenderArea::RenderArea(QWidget *parent)
    : QWidget(parent)
{
    this->setAttribute(Qt::WA_OpaquePaintEvent);
    QFont newFont = font();
    newFont.setPixelSize(12);
    setFont(newFont);

    QFontMetrics fontMetrics(newFont);
    xBoundingRect = fontMetrics.boundingRect(tr("x"));
    yBoundingRect = fontMetrics.boundingRect(tr("y"));
}

void RenderArea::setOperations(const QList<Operation> &operations)
{
    this->operations = operations;
    update();
}

void RenderArea::appendOperation(Operation op) {
    operations.append(op);
    update();
}

void RenderArea::setShape(const QPainterPath &shape)
{
    this->shape = shape;
    update();
}

void RenderArea::setXY(float x, float y) {
    this->x.append(x);
    this->y.append(y);
}

void RenderArea::clear() {
    operations.clear();
    x.clear();
    y.clear();
}

QSize RenderArea::minimumSizeHint() const
{
    return QSize(182, 182);
}

QSize RenderArea::sizeHint() const
{
    return QSize(600, 400);
}

void RenderArea::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    painter.setRenderHint(QPainter::Antialiasing);
    if (operations.empty())
        painter.fillRect(event->rect(), QBrush(Qt::black));
    painter.translate(300, 160);

    transformPainter(painter);
    drawShape(painter);
    drawCoordinates(painter, Qt::red);
}

void RenderArea::drawCoordinates(QPainter &painter, Qt::GlobalColor pen)
{
    painter.setPen(pen);

    painter.drawLine(0, 0, 50, 0);
    painter.drawLine(48, -2, 50, 0);
    painter.drawLine(48, 2, 50, 0);
    painter.drawText(60 - xBoundingRect.width() / 2,
                     0 + xBoundingRect.height() / 2, tr("x"));

    painter.drawLine(0, 0, 0, 50);
    painter.drawLine(-2, 48, 0, 50);
    painter.drawLine(2, 48, 0, 50);
    painter.drawText(0 - yBoundingRect.width() / 2,
                     60 + yBoundingRect.height() / 2, tr("y"));
}

void RenderArea::drawShape(QPainter &painter)
{
    painter.setPen(Qt::green);
    painter.drawPath(shape);
}

void RenderArea::transformPainter(QPainter &painter)
{
    QTransform matrix;
    for (int i = 0; i < operations.size(); ++i) {
        switch (operations[i]) {
        case ReflectX:
            matrix *= QTransform(1, 0, 0, 0, -1, 0, 0, 0, 1);
            break;
        case ReflectY:
            matrix *= QTransform(-1, 0, 0, 0, 1, 0, 0, 0, 1);
            break;
        case ReflectXY:
            matrix *= QTransform(-1, 0, 0, 0, -1, 0, 0, 0, 1);
            break;
        case Translate:
            matrix.translate(x[i], y[i]);
            break;
        case Scale:
            matrix.scale(x[i], y[i]);
            break;
        case Rotate:
            matrix.rotate(x[i]);
            break;
        case NoTransformation:
        default:
            ;
        }
    }
    painter.setWorldTransform(matrix, true);
}
