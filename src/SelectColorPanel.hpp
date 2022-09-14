#pragma once

#include "SelectColorButton.hpp"
#include <QtCore/QObject>
#include <QtGui/QPainter>
#include <QtWidgets/QApplication>
#include <QtWidgets/QtWidgets>
#include <estd/ptr.hpp>
#include <exception>

using namespace estd::shortnames;

class SelectColorPanel : public QWidget {
private:
    QHBoxLayout layout;
    std::vector<rptr<SelectColorButton>> buttons;

public:
    std::function<void(SelectColorPanel*, Qt::MouseButtons)> onClick = [](auto, auto) {};
    QColor color;
    SelectColorPanel(std::vector<QColor> colors, QWidget* p = 0) : QWidget(p), layout(this) {
        int widthLimit = 0;
        int heightLimit = 0;
        for (auto& color : colors) {
            this->color = color;
            rptr<SelectColorButton> btn = new SelectColorButton(this);
            widthLimit += btn->minimumWidth();
            heightLimit += btn->minimumHeight();
            btn->setColor(color);
            btn->onClick = [=](auto self, auto buttons) {
                this->color = self->getColor();
                cout << "color=" << self->getColor().name().toStdString() << "\n";
                onClick(this, buttons);
            };
            buttons.emplace_back(btn.get());
            layout.addWidget(btn.get());
        }
        setMinimumSize(widthLimit, heightLimit);
    }
};

class SelectColorCube : public PuzzleCube<1> {
public:
    std::function<void(SelectColorCube* self, Qt::MouseButtons)> onClick = [](auto, auto) {};
    QColor color;
    SelectColorCube(std::vector<QColor> colors, QWidget* p = 0) : PuzzleCube(p) {
        auto cube = this;

        if (colors.size() < 6) { throw std::runtime_error("SelectColorCube must have 6 or more colors"); };

        for (int i = 0; i < 6; i++) {
            cube->tiles[i]->color = colors[i];
            cube->tiles[i]->onClick = [=](auto self, auto mouse) {
                if (mouse == Qt::MouseButton::RightButton) {
                    self->color = QColorDialog::getColor(self->color, this);
                    self->update();
                }
                this->color = self->color;
                this->onClick(this, mouse);
            };
        }

        if (colors.size() <= 6) { return; }

        std::vector<QColor> remaining{colors.begin() + 6, colors.end()};
        rptr<SelectColorPanel> colorPanel = new SelectColorPanel(remaining, this);
        colorPanel->onClick = [=](auto self, auto mouse) {
            this->color = self->color;
            this->onClick(this, mouse);
        };
        cube->addSubWidget(colorPanel);
        //layout.addWidget(colorPanel.get());
    }
};