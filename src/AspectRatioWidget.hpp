#pragma once

#include <QtCore/QObject>
#include <QtGui/QPainter>
#include <QtWidgets/QApplication>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QtWidgets>
#include <estd/ptr.hpp>

class AspectRatioWidget : public QWidget {
private:
    double aspectWH = 2.0;
    estd::raw_ptr<QWidget> centralWidget;
public:
    AspectRatioWidget(double width, double height, QWidget* parent = nullptr) : QWidget(parent) {
        aspectWH = width / height;
    }
    AspectRatioWidget(estd::raw_ptr<QWidget> widget, double width, double height, QWidget* parent = nullptr) :
        AspectRatioWidget(width, height, parent) {
        setWidget(widget);
    }
    void resizeEvent(QResizeEvent* event) {
        int width = this->width();
        int height = this->height();

        int xScale = width;
        int yScale = height;

        double aspectCurrent = double(xScale) / double(yScale);

        if (aspectCurrent > aspectWH) {
            xScale *= aspectWH / aspectCurrent;
        } else {
            yScale *= aspectCurrent / aspectWH;
        }

        int xMove = (width - xScale) / 2;
        int yMove = (height - yScale) / 2;


        centralWidget->move(xMove, yMove);
        centralWidget->resize(xScale, yScale);
    }
    void setWidget(estd::raw_ptr<QWidget> w) {
        centralWidget = w;
        centralWidget->setParent(this);
    }
};