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
    jptr<QColor> color;
    SelectColorPanel(std::vector<jptr<QColor>> colors, QWidget* p = 0) : QWidget(p), layout(this) {
        layout.setContentsMargins(0,0,0,0);
        int widthLimit = 0;
        int heightLimit = 0;
        for (auto& color : colors) {
            this->color = color;
            rptr<SelectColorButton> btn = new SelectColorButton(color, this);
            widthLimit += btn->minimumWidth();
            heightLimit += btn->minimumHeight();
            btn->onClick = [=](auto self, auto buttons) {
                this->color = self->color;
                cout << "color=" << self->color->name().toStdString() << "\n";
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
    jptr<QColor> color;
    std::vector<jptr<QColor>> availableColors;
    SelectColorCube(std::vector<jptr<QColor>> colors, QWidget* p = 0) : PuzzleCube(colors.back(), p) {
        this->color = colors.back();
        for (auto c : colors) { availableColors.push_back(c); } // create unique colors

        auto cube = this;

        if (colors.size() < 6) { throw std::runtime_error("SelectColorCube must have 6 or more colors"); };

        for (int i = 0; i < 6; i++) {
            cube->tiles[i]->color = colors[i];
            cube->tiles[i]->onClick = [=](auto self, auto mouse) {
                if (mouse == Qt::MouseButton::RightButton) {
                    self->color.value() = QColorDialog::getColor(self->color.value(), this);
                    self->update();
                }
                this->color = self->color;
                this->onClick(this, mouse);
            };
        }

        if (colors.size() <= 6) { return; }

        std::vector<jptr<QColor>> remaining{colors.begin() + 6, colors.end()};
        rptr<SelectColorPanel> colorPanel = new SelectColorPanel(remaining, this);
        colorPanel->onClick = [=](auto self, auto mouse) {
            this->color = self->color;
            this->onClick(this, mouse);
        };
        cube->addSubWidget(colorPanel);
        //layout.addWidget(colorPanel.get());
    }
};