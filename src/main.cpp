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
#include "NxNeditor.hpp"

// #include "../vendor/src/pruning/pruning.cpp"
// #include "../vendor/src/pruning/pruningTree.cpp"
// #include "../vendor/src/pruning/redundancy.cpp"

// #include "../vendor/src/solver/puzzle.cpp"
// #include "../vendor/src/solver/puzzleSolver.cpp"
// #include "../vendor/src/solver/puzzleState.cpp"

using namespace estd::shortnames;
using namespace std;

int main(int argc, char** argv) {
    cptr<QApplication> app = new QApplication(argc, argv);
    cptr<QMainWindow> mw = new QMainWindow();
    
    auto wid = new EQLayoutWidget<QHBoxLayout>();
    auto puzzleColumn = new QVBoxLayout();
    auto solutionsColumn = new QVBoxLayout();

    mw->setCentralWidget(new EQMarginWidget(wid));
    wid->addLayout(puzzleColumn);
    wid->addLayout(solutionsColumn);

    auto text = new QTextBrowser();
    // text->setMaximumWidth(500);
    solutionsColumn->addWidget(text);
    solutionsColumn->addWidget(new EQLayoutWidget<QHBoxLayout>({new QProgressBar(), new QPushButton("cancel")}));

    auto tlbx = new QTabWidget();
    auto solvedState = new NxNEditor<3>();
    auto scramble = new NxNEditor<3>(solvedState->idToColor);
    
    tlbx->setMinimumSize(300,300);
    tlbx->addTab(new EQMarginWidget(scramble), "Scramble");
    tlbx->addTab(new EQMarginWidget(solvedState), "Solved State");
    puzzleColumn->addWidget(tlbx);
    scramble->setState(Puzzle3x3());
    solvedState->setState(Puzzle3x3());

    puzzleColumn->addWidget(tlbx);

    auto configForm = new QFormLayout();

    auto allowedMoves = new QLineEdit("U U2 U' R R2 R' F F2 F' D D2 D' L L2 L' B B2 B'");
    auto applyMovesBtn = new QPushButton("apply");
    auto applyMoves = new QLineEdit("");
    auto searchDepth = new QSpinBox();
    auto numSolutions = new QSpinBox();
    searchDepth->setValue(14);
    numSolutions->setValue(-1);
    puzzleColumn->addLayout(configForm);
    configForm->addRow(QObject::tr("Apply Moves:"), new EQLayoutWidget<QHBoxLayout>({applyMoves, applyMovesBtn}));
    configForm->addRow(QObject::tr("Allowed Moves:"), allowedMoves);
    configForm->addRow(QObject::tr("Search Depth:"), searchDepth);
    configForm->addRow(QObject::tr("Number of Solutions:"), numSolutions);
    auto solveBtn = new QPushButton("solve", wid);
    puzzleColumn->addWidget(solveBtn);

    mw->show();

    QObject::connect(applyMovesBtn, &QPushButton::clicked, [&](bool checked) {
        Puzzle3x3 tmp;
        tmp.state = scramble->getState();
        tmp.applyMoves(applyMoves->text().toStdString());
        scramble->setState(tmp);
        scramble->update();
    });

    QObject::connect(solveBtn, &QPushButton::clicked, [&](bool checked) {
        QMetaObject::invokeMethod(app.get(), [=] { text->clear(); });
        std::thread t([&]() {
            QFile file("yourFile.png");
            file.open(QIODevice::WriteOnly);
            scramble->cube->grab().save(&file, "PNG");
            State pzl = scramble->getState();

            Solver3x3 solver(allowedMoves->text().toStdString());
            solver.cfg->pruiningTablesPath = "./tables";

            auto slnQ = solver.asyncSolveStrings(pzl, searchDepth->value(), numSolutions->value() ? numSolutions->value() : -1);
            std::vector<std::string> res;

            while (slnQ->hasNext()) {
                QString solutionString = QString((slnQ->pop()).c_str());
                QMetaObject::invokeMethod(app.get(), [=] { text->append(solutionString); });
            }
        });
        t.detach();
    });
    return app->exec();
}
