#include "AspectRatioWidget.hpp"
#include "NxNeditor.hpp"
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
    QProgressBar* progressBarTables = new QProgressBar();
    QProgressBar* progressBar = new QProgressBar();
    QPushButton* cancelButton = new QPushButton("cancel");
    solutionsColumn->addWidget(new EQLayoutWidget<QHBoxLayout>({progressBarTables, progressBar, cancelButton}));
    std::function<void(void)> onCancelBtn = []() {};
    QObject::connect(cancelButton, &QPushButton::clicked, [&](bool checked) { onCancelBtn(); });

    auto tlbx = new QTabWidget();
    auto solvedState = new NxNEditor<3>();
    auto scramble = new NxNEditor<3>(solvedState->idToColor);
    auto activeState = scramble;

    tlbx->setMinimumSize(300, 300);
    tlbx->addTab(new EQMarginWidget(scramble), "Initial State");
    tlbx->addTab(new EQMarginWidget(solvedState), "Final State");
    puzzleColumn->addWidget(tlbx);
    scramble->setState(Puzzle3x3());
    solvedState->setState(Puzzle3x3());

    puzzleColumn->addWidget(tlbx);

    auto configForm = new QFormLayout();

    auto allowedMoves = new QLineEdit("U U2 U' R R2 R' F F2 F' D D2 D' L L2 L' B B2 B'");
    auto applyMovesBtn = new QPushButton("apply");
    auto swapScramble = new QPushButton("swap");
    auto resetToSolved = new QPushButton("reset");
    auto applyMoves = new QLineEdit("");
    auto searchDepth = new QSpinBox();
    auto numSolutions = new QSpinBox();
    searchDepth->setRange(1, 999);
    numSolutions->setRange(-1, INT_MAX);
    searchDepth->setValue(14);
    numSolutions->setValue(-1);
    puzzleColumn->addLayout(configForm);
    configForm->addRow(QObject::tr("Apply Moves:"), applyMoves);
    configForm->addRow("", new EQLayoutWidget<QHBoxLayout>(std::vector<QWidget*>{swapScramble, resetToSolved, applyMovesBtn}));
    configForm->addRow(QObject::tr("Allowed Moves:"), allowedMoves);
    configForm->addRow(QObject::tr("Search Depth:"), searchDepth);
    configForm->addRow(QObject::tr("Num Solutions:"), numSolutions);
    auto solveBtn = new QPushButton("solve", wid);
    puzzleColumn->addWidget(solveBtn);

    mw->show();

    QObject::connect(applyMovesBtn, &QPushButton::clicked, [&](bool checked) {
        Puzzle3x3 tmp;
        tmp.state = activeState->getState();
        try {
            tmp.applyMoves(applyMoves->text().toStdString());
            activeState->setState(tmp);
            activeState->update();
        } catch (std::runtime_error& e) { QMessageBox::critical(0, "Error", e.what()); }
    });

    QObject::connect(tlbx, &QTabWidget::currentChanged, [&](int idx) {
        if (idx == 1) {
            activeState = solvedState;
            resetToSolved->hide();
        } else {
            activeState = scramble;
            resetToSolved->show();
        }
    });

    QObject::connect(resetToSolved, &QPushButton::clicked, [&](bool checked) {
        scramble->setState(solvedState->getState());
        scramble->update();
    });

    QObject::connect(swapScramble, &QPushButton::clicked, [&](bool checked) {
        State slv = solvedState->getState();
        State scr = scramble->getState();
        scramble->setState(slv);
        solvedState->setState(scr);
        scramble->update();
        solvedState->update();
    });

    Solver3x3 solver;
    QObject::connect(solveBtn, &QPushButton::clicked, [&](bool checked) {
        QMetaObject::invokeMethod(app.get(), [=] { text->clear(); });
        std::thread t([&]() {
            try {
                QMetaObject::invokeMethod(app.get(), [=] {
                    progressBar->setValue(0);
                    solveBtn->setDisabled(true);
                });
                // QFile file("yourFile.png");
                // file.open(QIODevice::WriteOnly);
                // scramble->cube->grab().save(&file, "PNG");
                Puzzle3x3 pzl{allowedMoves->text().toStdString()};
                pzl.state = scramble->getState();
                pzl.solvedState = solvedState->getState();
                std::cout << "myState: " << pzl.toString() << endl;

                solver.cfg->pruiningTablesPath = "./tables";
                solver.progressCallback = [&](int progress) {
                    QMetaObject::invokeMethod(app.get(), [=] { progressBar->setValue(progress); });
                };
                solver.tableProgressCallback = [&](int progress) {
                    QMetaObject::invokeMethod(app.get(), [=] { progressBarTables->setValue(progress); });
                };
                onCancelBtn = [&]() { solver.cancel(); };

                auto slnQ = solver.asyncSolveStrings(
                    pzl, searchDepth->value(), numSolutions->value() ? numSolutions->value() : -1
                );
                std::vector<std::string> res;

                while (slnQ->hasNext()) {
                    QString solutionString = QString((slnQ->pop()).c_str());
                    QMetaObject::invokeMethod(app.get(), [=] { text->append(solutionString); });
                }
                onCancelBtn = []() {};
                QMetaObject::invokeMethod(app.get(), [=] { solveBtn->setDisabled(false); });
            } catch (std::runtime_error& e) {
                QMetaObject::invokeMethod(app.get(), [&, e] {
                    QMessageBox::critical(0, "Error", e.what());
                    solveBtn->setDisabled(false);
                });
            }
        });
        t.detach();
    });
    return app->exec();
}
