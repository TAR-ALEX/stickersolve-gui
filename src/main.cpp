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
#include <estd/thread_pool.hpp>
#include <functional>
#include <iostream>
#include <map>
#include <stickersolve/puzzles/Solver3x3.h>
#include <thread>
#include <vector>

using namespace estd::shortnames;
using namespace std;

/// Given a map from keys to values, creates a new map from values to keys
template <typename K, typename V>
static map<V, K> reverseMap(const map<K, V>& m) {
    map<V, K> r;
    for (const auto& kv : m) r[kv.second] = kv.first;
    return r;
}

map<int, jptr<QColor>> idToColor = {
    {0, jptr<QColor>{0, 255, 0}},
    {1, jptr<QColor>{255, 255, 0}},
    {2, jptr<QColor>{255, 0, 0}},
    {3, jptr<QColor>{255, 255, 255}},
    {4, jptr<QColor>{255, 144, 0}},
    {5, jptr<QColor>{0, 100, 255}},
    {-1, jptr<QColor>{127, 127, 127}},
};

map<jptr<QColor>, int> colorToId = reverseMap(idToColor);


int main(int argc, char** argv) {
    cptr<QApplication> app = new QApplication(argc, argv);

    cptr<QMainWindow> mw = new QMainWindow();
    auto wid = new QWidget();
    auto mainLayout = new QHBoxLayout(wid);
    auto puzzleColumn = new QVBoxLayout(wid);
    auto solutionsColumn = new QVBoxLayout(wid);

    wid->setLayout(mainLayout);
    mw->setCentralWidget(wid);
    mainLayout->addLayout(puzzleColumn);
    mainLayout->addLayout(solutionsColumn);

    auto text = new QTextBrowser();
    text->setMaximumWidth(500);
    solutionsColumn->addWidget(text);

    //SelectColorCube SelectColorPanel{
    auto colorPanel = new SelectColorCube{
        {
            idToColor[0],
            idToColor[1],
            idToColor[2],
            idToColor[3],
            idToColor[4],
            idToColor[5],
            idToColor[-1],
        },
        wid,
    };
    auto button2 = new QPushButton("save", wid);
    auto cube = new PuzzleCube(colorPanel->availableColors.back(), wid);
    cube->activeColor = colorPanel->color;

    cube->addSubWidget(colorPanel);

    puzzleColumn->addWidget(button2);
    // puzzleColumn->addWidget(cube);
    puzzleColumn->addWidget(new AspectRatioWidget(cube, 10.5, 7.5));

    mw->show();
    colorPanel->onClick = [&](auto self, auto mouseButtons) {
        cout << "clicked(checked = " << mouseButtons << ");\n";
        cube->activeColor = self->color;
    };

    QObject::connect(button2, &QPushButton::clicked, [&](bool checked) {
        QMetaObject::invokeMethod(app.get(), [=] { text->clear(); });
        std::thread t([&]() {
            QFile file("yourFile.png");
            file.open(QIODevice::WriteOnly);
            cube->grab().save(&file, "PNG");
            auto qcolors = cube->getColors();
            State pzl;
            for (auto& c : qcolors) { pzl.push_back(colorToId[c]); }

            Solver3x3 solver("U U2 U' R R2 R' F F2 F' D D2 D' L L2 L' B B2 B'");
            solver.cfg->targetThreads = 16;
            solver.pruningTableMasked.load();
            solver.pruningTableClassic.load();
            solver.redundancyTable.load();

            // cout << "solver.pruningTableClassic.getStats()";
            // cout << "\n--------------------\n";
            // cout << solver.pruningTableClassic.getStats();
            // cout << "solver.pruningTable.getStats()";
            // cout << "\n--------------------\n";
            // cout << solver.pruningTableMasked.getStats();
            // cout << "\n--------------------\n";
            // cout << "solver.redundancyTable.getStats()";
            // cout << "\n--------------------\n";
            // cout << solver.redundancyTable.getStats();
            // cout << "\n--------------------\n";

            auto slnQ = solver.asyncSolveStrings(pzl, 15, -1);

            while (slnQ->hasNext()) {
                QString solutionString = QString((slnQ->pop()).c_str());
                QMetaObject::invokeMethod(app.get(), [=] { text->append(solutionString); });
            }
            
            solver.pruningTableMasked.unload();
            solver.pruningTableClassic.unload();
            solver.redundancyTable.unload();

        });
        t.detach();
    });
    return app->exec();
}
