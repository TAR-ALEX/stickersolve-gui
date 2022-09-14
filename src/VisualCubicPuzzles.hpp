#pragma once

#include "AspectRatioWidget.hpp"
#include <QtCore/QObject>
#include <QtGui/QPainter>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QtWidgets>
#include <estd/ptr.hpp>
#include <functional>
#include <iostream>
#include <vector>

using namespace estd::shortnames;
using namespace std;

class PolygonButton : public QWidget {
private:
    QPolygonF scaledPolygon() {
        QPolygonF result;
        for (auto vert : polygon) {
            QPointF p = {vert.x() * width(), vert.y() * height()};
            result.push_back(p);
        }
        return result;
    }

public:
    using QWidget::QWidget;
    QPolygonF polygon;
    QColor color{127, 127, 127};
    std::function<void(PolygonButton* self, Qt::MouseButtons)> onClick = [](auto, auto) {};

    virtual void mousePressEvent(QMouseEvent* event) {
        if (scaledPolygon().containsPoint(event->pos(), Qt::OddEvenFill)) {
            onClick(this, event->button());
            update();
            event->accept();
        } else {
            event->setAccepted(false);
            event->ignore();
        }
    }

    void paintEvent(QPaintEvent* p) {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setBrush(color);
        painter.setPen(QPen(Qt::black, 2));
        painter.drawPolygon(scaledPolygon());
    }
};

class PuzzleTiled : public QWidget {
protected:
    double aspectWH = 2.0;

public:
    PuzzleTiled(QWidget* parent) : QWidget(parent) {}

    std::vector<rptr<PolygonButton>> tiles;
    QColor activeColor{0, 0, 0};
    QColor defaultColor{127, 127, 127};

    std::vector<QColor> getColors() {
        std::vector<QColor> result;
        for (auto& tile : tiles) { result.push_back(tile->color); }
        return result;
    }

    void paintEvent(QPaintEvent* p) {
        for (auto tile : tiles) {
            tile->setFixedWidth(width());
            tile->setFixedHeight(height());
        }
    }
    virtual void mousePressEvent(QMouseEvent* event) {
        for (auto& tile : tiles) {
            tile->mousePressEvent(event);
            if (event->isAccepted()) break;
        }
    }

    void normalize() {
        double maxWidth = 0;
        double maxHeight = 0;
        double minWidth = 0;
        double minHeight = 0;
        for (auto& poly : tiles) {
            for (auto& p : poly->polygon) {
                if (p.x() < minWidth) minWidth = p.x();
                if (p.y() < minHeight) minHeight = p.y();
                if (p.y() > maxHeight) maxHeight = p.y();
                if (p.x() > maxWidth) maxWidth = p.x();
            }
        }

        double scaleX = maxWidth - minWidth;
        double scaleY = maxHeight - minHeight;

        for (auto& poly : tiles) {
            for (auto& p : poly->polygon) {
                p.setX((p.x() - minWidth) / scaleX);
                p.setY((p.y() - minHeight) / scaleY);
            }
        }
    }

    // QPixmap asPixmap() {
    //     QRect rectangle{QPoint{0, 0}, QPoint{1000, 1000}};
    //     QPixmap pixmap(rectangle.size());
    //     render(&pixmap, QPoint(), QRegion(rectangle));
    //     setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
    //     setMinimumSize(0, 0);
    //     return pixmap;
    // }
};

template <int order = 3>
class PuzzleCube : public PuzzleTiled {
private:
    void addGrid(double xStart, double yStart, double scaleX, double scaleY, double skewX, double skewY) {
        std::vector<rptr<PolygonButton>> tmp;

        for (int y = 0; y < order; y++) {
            for (int x = 0; x < order; x++) {
                rptr<PolygonButton> tile = new PolygonButton(this);

                tile->polygon = QPolygonF{
                    {
                        {xStart + x + (skewX * y), yStart + y + (skewY * x)},
                        {xStart + 1 + x + (skewX * y), yStart + y + (skewY * (x + 1))},
                        {xStart + 1 + x + (skewX * (y + 1)), yStart + 1 + y + (skewY * (x + 1))},
                        {xStart + x + (skewX * (y + 1)), yStart + 1 + y + (skewY * x)},
                    },
                };
                tmp.push_back(tile);
            }
        }
        for (auto tile : tmp) {
            for (auto& p : tile->polygon) {
                p.setX(xStart + (p.x() - xStart) * scaleX * 3.0 / order);
                p.setY(yStart + (p.y() - yStart) * scaleY * 3.0 / order);
            }
            tiles.push_back(tile);
        }
    }

    rptr<QWidget> subWidget;

public:
    PuzzleCube(QWidget* parent) : PuzzleTiled(parent) {
        addGrid(1.5, 0, 1, 0.5, -0.5, 0);
        addGrid(0, 1.5, 1, 1, 0, 0);
        addGrid(3, 1.5, 0.5, 1, 0, -0.5);

        addGrid(4.5, 0, 1, 1, 0, 0);
        addGrid(-3, 1.5, 1, 1, 0, 0);
        addGrid(0, 4.5, 1, 1, 0, 0);

        this->aspectWH = 10.5 / 7.5;

        normalize();

        int i = 0;
        for (auto tile : tiles) {
            tile->onClick = [=](auto self, auto mouseButtons) {
                cout << "tiles[" << i << "].onClick\n";
                if (mouseButtons == Qt::MouseButton::LeftButton) {
                    self->color = activeColor;
                } else {
                    self->color = defaultColor;
                }
            };
            i++;
        }
    }

    void resizeEvent(QResizeEvent* event) {
        PuzzleTiled::resizeEvent(event);
        double width = this->width();
        double height = this->height();

        double scaledW = width/2.5;
        double scaledH = height/2.5;

        double offsetX = width - scaledW;
        double offsetY = height - scaledH;

        if (subWidget) {
            subWidget->resize(scaledW, scaledH);
            subWidget->move(offsetX, offsetY);
            subWidget->update();
        }
    }

    void addSubWidget(rptr<QWidget> w) {
        subWidget = w;
        subWidget->setParent(this);
    }
};