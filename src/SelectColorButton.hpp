#pragma once

#include "AspectRatioWidget.hpp"
#include "VisualCubicPuzzles.hpp"
#include <QtCore/QObject>
#include <QtGui/QPainter>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QtWidgets>

class SelectColorButton : public AspectRatioWidget {
protected:
    rptr<PolygonButton> polyBtn;

public:
    jptr<QColor> color;
    SelectColorButton(jptr<QColor> c, QWidget* parent = 0) : AspectRatioWidget(1, 1, parent) {
        polyBtn = new PolygonButton();
        polyBtn->color = c;
        this->color = c;
        setWidget(polyBtn);
        setMinimumSize({5, 5});
        polyBtn->polygon = QRectF{QPointF{0, 0}, QPointF{1, 1}};
        polyBtn->onClick = [&](auto self, auto mouseButtons) {
            if (mouseButtons == Qt::MouseButton::RightButton) this->changeColor();
            this->onClick(this, mouseButtons);
        };
    }

    void changeColor() {
        QColor newColor = QColorDialog::getColor(polyBtn->color.value(), parentWidget());
        if (newColor.isValid()) polyBtn->color.value() = newColor;
        // cout << "color=" << polyBtn->color->name().toStdString() << "\n";
    }

    std::function<void(SelectColorButton*, Qt::MouseButtons)> onClick = [](auto, auto) {};
};