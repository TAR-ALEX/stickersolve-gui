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

class ContainerWidget : public QWidget {
private:
    estd::raw_ptr<QWidget> centralWidget;

public:
    ContainerWidget(QWidget* parent = nullptr) : QWidget(parent) { QWidget::setContentsMargins(0, 0, 0, 0); }
    ContainerWidget(estd::raw_ptr<QWidget> widget, QWidget* parent = nullptr) : ContainerWidget(parent) {
        setWidget(widget);
    }
    void resizeEvent(QResizeEvent* event) {
        centralWidget->move(0, 0);
        centralWidget->resize(this->width(), this->height());
    }
    void setWidget(estd::raw_ptr<QWidget> w) {
        centralWidget = w;
        centralWidget->setParent(this);
    }
};

class EQMarginWidget : public QWidget {
private:
    estd::raw_ptr<QHBoxLayout> layout;
    estd::raw_ptr<QWidget> widget;

public:
    EQMarginWidget(estd::raw_ptr<QWidget> widget, QWidget* parent = nullptr) : QWidget(parent) {
        layout = new QHBoxLayout();
        this->setLayout(layout.get());
        setWidget(widget);
    }

    EQMarginWidget(int margins, QWidget* parent = nullptr) : QWidget(parent) {
        layout = new QHBoxLayout();
        this->setLayout(layout.get());
        layout->setContentsMargins(margins, margins, margins, margins);
    }
    EQMarginWidget(estd::raw_ptr<QWidget> widget, int margins, QWidget* parent = nullptr) : QWidget(parent) {
        layout = new QHBoxLayout();
        this->setLayout(layout.get());
        layout->setContentsMargins(margins, margins, margins, margins);
        setWidget(widget);
    }


    void setWidget(estd::raw_ptr<QWidget> w) {
        // if(widget) delete widget.get();
        widget = w;
        layout->addWidget(widget.get());
    }
};

template <class LayoutT>
class EQLayoutWidget : public QWidget {
public:
    estd::raw_ptr<LayoutT> layout;

    EQLayoutWidget(QWidget* parent = nullptr) : QWidget(parent) {
        QWidget::setContentsMargins(0, 0, 0, 0);
        setLayout(new LayoutT(this));
    }

    EQLayoutWidget(estd::raw_ptr<LayoutT> l, QWidget* parent = nullptr) {
        QWidget::setContentsMargins(0, 0, 0, 0);
        setLayout(l);
    }

    EQLayoutWidget(std::vector<QWidget*> toAdd) {
        QWidget::setContentsMargins(0, 0, 0, 0);
        setLayout(new LayoutT(this));
        for (auto& w : toAdd) { layout->addWidget(w); }
    }

    void setLayout(estd::raw_ptr<LayoutT> l) {
        layout = l;
        layout->setParent(this);
        layout->setContentsMargins(0, 0, 0, 0);
        QWidget::setLayout(l.get());
    }
    template <typename... Args>
    decltype(auto) addWidget(Args&&... params) {
        layout->addWidget(std::forward<Args>(params)...);
    }

    template <typename... Args>
    decltype(auto) addLayout(Args&&... params) {
        layout->addLayout(std::forward<Args>(params)...);
    }
};