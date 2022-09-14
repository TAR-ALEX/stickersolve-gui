#pragma once

#include "VisualCubicPuzzles.hpp"
#include <QtCore/QObject>
#include <QtGui/QPainter>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QtWidgets>

class SelectColorButton : public PolygonButton {
public:
    SelectColorButton(QWidget* parent = 0);
    SelectColorButton(QColor c, QWidget* parent = 0) : SelectColorButton(parent) { setColor(c); }

    void setColor(const QColor& color);
    const QColor& getColor() const;

    void updateColor();
    void changeColor();

    std::function<void(SelectColorButton*, Qt::MouseButtons)> onClick = [](auto, auto) {};
};

SelectColorButton::SelectColorButton(QWidget* parent) : PolygonButton(parent) {
    setMinimumSize({20, 20});
    // setMaximumSize({20, 20});
    polygon = QRectF{QPointF{0, 0}, QPointF{1, 1}};
    PolygonButton::onClick = [&](auto self, auto mouseButtons) {
        if (mouseButtons == Qt::MouseButton::RightButton) this->changeColor();
        this->onClick(this, mouseButtons);
    };
    setColor(QColor{127, 127, 127});
}

void SelectColorButton::updateColor() {
    setStyleSheet("background-color: " + color.name() + "; border: none;");
    update();
}

void SelectColorButton::changeColor() {
    QColor newColor = QColorDialog::getColor(color, parentWidget());
    if (newColor != color) { setColor(newColor); }
}

void SelectColorButton::setColor(const QColor& color) {
    this->color = color;
    updateColor();
}

const QColor& SelectColorButton::getColor() const { return color; }
