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

#include "window.h"

#include <cmath>
#include <QComboBox>
#include <QLineEdit>
#include <QPolygonF>
#include <QGridLayout>
#include <QPushButton>
#include <QFormLayout>

#define PI 3.14159265

Window::Window()
{
    shapeComboBox = new QComboBox;
    shapeComboBox->addItem(tr("Polígono"));
    shapeComboBox->addItem(tr("Arco"));
    shapeComboBox->addItem(tr("Cubo"));
    shapeComboBox->addItem(tr("Prisma triangular"));
    shapeComboBox->addItem(tr("Prisma rectangular"));
    shapeComboBox->addItem(tr("Cono"));

    transformationTextBoxes[0] = new QLineEdit;
    transformationTextBoxes[1] = new QLineEdit;
    shapeTextBox = new QLineEdit;

    QGridLayout *layout = new QGridLayout;
    layout->addWidget(shapeComboBox, 1, 0);

    shapeButton = new QPushButton("Dibujar figura");
    connect(shapeButton, SIGNAL(clicked(bool)), this, SLOT(shapeChanged()));

    QFormLayout *formLayout1 = new QFormLayout;
    formLayout1->addRow(tr("Lados / Ángulo:"), shapeTextBox);
    formLayout1->addRow(shapeButton);
    layout->addLayout(formLayout1, 2, 0);

    QFormLayout *formLayout2 = new QFormLayout;
    formLayout2->addRow(tr("X / Ángulo:"), transformationTextBoxes[0]);
    formLayout2->addRow(tr("Y:"), transformationTextBoxes[1]);
    layout->addLayout(formLayout2, 2, 1);

    transformationButton = new QPushButton("Aplicar transformación");
    connect(transformationButton, SIGNAL(clicked(bool)), this, SLOT(operationChanged()));
    layout->addWidget(transformationButton, 3, 1);

    clearButton = new QPushButton("Reset");
    connect(clearButton, SIGNAL(clicked(bool)), this, SLOT(borrarPantalla()));
    layout->addWidget(clearButton, 3, 0);

    transformedRenderArea = new RenderArea;
    operationComboBox = new QComboBox;
    operationComboBox->addItem(tr("Sin transformación"));
    operationComboBox->addItem(tr("Rotación"));
    operationComboBox->addItem(tr("Escalación"));
    operationComboBox->addItem(tr("Traslación"));
    operationComboBox->addItem(tr("Reflexión en X"));
    operationComboBox->addItem(tr("Reflexión en Y"));
    operationComboBox->addItem(tr("Reflexión en XY"));

    layout->addWidget(transformedRenderArea, 0, 0, 1, 2);
    layout->addWidget(operationComboBox, 1, 1);

    setLayout(layout);
    setupShapes();
    shapeSelected(0);

    setWindowTitle(tr("Transformations"));
}

void Window::setupShapes()
{
    QPainterPath polygon;
    QPainterPath arc;

    QPainterPath cube;
    int puntos1[8], puntos2[8];
    cube.addPolygon(generarPoligono(4, 0, 0, puntos1));
    cube.closeSubpath();
    cube.addPolygon(generarPoligono(4, 40, 0, puntos2));
    cube.closeSubpath();
    for (int i = 0; i < 4; ++i) {
        cube.moveTo(puntos1[2*i], puntos1[2*i+1]);
        cube.lineTo(puntos2[2*i], puntos2[2*i+1]);
    }

    QPainterPath prismaTriangular;
    prismaTriangular.addPolygon(generarPoligono(3, 0, 0, puntos1));
    prismaTriangular.closeSubpath();
    prismaTriangular.addPolygon(generarPoligono(3, 50, 20, puntos2));
    prismaTriangular.closeSubpath();
    for (int i = 0; i < 3; ++i) {
        prismaTriangular.moveTo(puntos1[2*i], puntos1[2*i+1]);
        prismaTriangular.lineTo(puntos2[2*i], puntos2[2*i+1]);
    }

    QPainterPath prismaRectangular;
    prismaRectangular.addPolygon(generarPoligono(4, 0, 0, puntos1));
    prismaRectangular.closeSubpath();
    prismaRectangular.addPolygon(generarPoligono(4, 70, 15, puntos2));
    prismaRectangular.closeSubpath();
    for (int i = 0; i < 4; ++i) {
        prismaRectangular.moveTo(puntos1[2*i], puntos1[2*i+1]);
        prismaRectangular.lineTo(puntos2[2*i], puntos2[2*i+1]);
    }

    QPainterPath cono;
    cono.addEllipse(QPointF(0.0, 50.0), 50.0, 10.0);
    cono.moveTo(-50.0, 50.0);
    cono.lineTo(0.0, -20.0);
    cono.moveTo(50.0, 50.0);
    cono.lineTo(0.0, -20.0);

    shapes.append(polygon);
    shapes.append(arc);
    shapes.append(cube);
    shapes.append(prismaTriangular);
    shapes.append(prismaRectangular);
    shapes.append(cono);
}

QPolygonF Window::generarPoligono(int n, int centroX, int centroY, int puntos[]) {
    QPolygonF poly;
    double radio = 50;
    int puntoX, puntoY;
    double angulo = (2.0 * PI) / n;
    for (int i = 0; i < n; ++i) {
        puntoX = centroX + radio * sin(i * angulo);
        puntoY = centroY - radio * cos(i * angulo);
        if (puntos) {
            puntos[2*i] = puntoX;
            puntos[2*i+1] = puntoY;
        }
        poly << QPointF(puntoX, puntoY);
    }
    return poly;
}

void Window::operationChanged()
{
    static const Operation operationTable[] = {
        NoTransformation, Rotate, Scale, Translate, ReflectX, ReflectY, ReflectXY
    };

    int index = operationComboBox->currentIndex();
    transformedRenderArea->setXY(transformationTextBoxes[0]->text().toFloat(), transformationTextBoxes[1]->text().toFloat());
    transformedRenderArea->appendOperation(operationTable[index]);
}

void Window::shapeSelected(int index)
{
    QPainterPath shape;
    if (index == 0) {
        shape.addPolygon(generarPoligono(shapeTextBox->text().toInt(), 0, 0));
        shape.closeSubpath();
    }
    else if (index == 1) {
        shape.moveTo(50.0, 0.0);
        shape.arcTo(-50, -50, 100, 100, 0, shapeTextBox->text().toInt());
    }
    else
        shape = shapes[index];

    transformedRenderArea->setShape(shape);
}

void Window::shapeChanged() {
    int index = shapeComboBox->currentIndex();
    shapeSelected(index);
}

void Window::borrarPantalla() {
    QPainterPath shape;
    transformedRenderArea->clear();
    transformedRenderArea->setShape(shape);
}
