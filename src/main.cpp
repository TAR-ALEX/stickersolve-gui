#include "AspectRatioWidget.hpp"
#include "SelectColorButton.hpp"
#include "SelectColorPanel.hpp"
#include "VisualCubicPuzzles.hpp"
#include <QtCore/QObject>
#include <QtGui/QPainter>
#include <QtWidgets/QApplication>
#include <QtWidgets/QColorDialog>
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


int main(int argc, char** argv) {
    QApplication app(argc, argv);

    QMainWindow mw;
    QWidget wid;
    QHBoxLayout mainLayout(&wid);
    QVBoxLayout puzzleColumn(&wid);
    QVBoxLayout solutionsColumn(&wid);
    wid.setLayout(&mainLayout);
    mw.setCentralWidget(&wid);
    mainLayout.addLayout(&puzzleColumn);
    mainLayout.addLayout(&solutionsColumn);

    QTextBrowser text;
    text.setMaximumWidth(500);
    solutionsColumn.addWidget(&text);

    //SelectColorCube SelectColorPanel{
    SelectColorCube colorPanel{
        {
            QColor{0, 255, 0},
            QColor{255, 255, 0},
            QColor{255, 0, 0},
            QColor{255, 255, 255},
            QColor{255, 144, 0},
            QColor{0, 100, 255},
            QColor{127, 127, 127}
        },
        &mw};
    QPushButton button2("save", &mw);
    AspectRatioWidget aspect(10.5, 7.5, &mw);
    PuzzleCube cube(&aspect);

    aspect.setWidget(&cube);

    cube.addSubWidget(&colorPanel);

    // puzzleColumn.addWidget(&colorPanel);
    puzzleColumn.addWidget(&button2);
    puzzleColumn.addWidget(&aspect);
    mw.show();
    colorPanel.onClick = [&](auto self, auto mouseButtons) {
        cout << "clicked(checked = " << mouseButtons << ");\n";
        cube.activeColor = self->color;
    };

    QObject::connect(&button2, &QPushButton::clicked, [&](bool checked) {
        QFile file("yourFile.png");
        file.open(QIODevice::WriteOnly);
        cube.grab().save(&file, "PNG");
    });



    return app.exec();
}
