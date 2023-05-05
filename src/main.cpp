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
    auto cleanBtn = new QPushButton("clean");
    auto applyMoves = new QLineEdit("");
    auto searchDepth = new QSpinBox();
    auto numSolutions = new QSpinBox();
    searchDepth->setRange(1, 999);
    numSolutions->setRange(-1, INT_MAX);
    searchDepth->setValue(14);
    numSolutions->setValue(-1);
    puzzleColumn->addLayout(configForm);
    configForm->addRow(QObject::tr("Apply Moves:"), applyMoves);
    configForm->addRow(new EQLayoutWidget<QHBoxLayout>({cleanBtn, swapScramble, resetToSolved, applyMovesBtn}));
    configForm->addRow(QObject::tr("Allowed Moves:"), allowedMoves);
    configForm->addRow(QObject::tr("Search Depth:"), searchDepth);
    configForm->addRow(QObject::tr("Num Solutions:"), numSolutions);

    auto sortSelector = new QComboBox();
    sortSelector->addItems({"disabled", "length", "fingertricks"});
    configForm->addRow(QObject::tr("Sorting:"), sortSelector);

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
            // resetToSolved->hide();
        } else {
            activeState = scramble;
            // resetToSolved->show();
        }
    });

    QObject::connect(cleanBtn, &QPushButton::clicked, [&](bool checked) {
        State s = activeState->getState();
        for (auto& color : s) color = -1;
        activeState->setState(s);
        activeState->update();
    });

    QObject::connect(resetToSolved, &QPushButton::clicked, [&](bool checked) {
        if (activeState == scramble) {
            scramble->setState(solvedState->getState());
            scramble->update();
        } else {
            solvedState->setState(Puzzle3x3().solvedState);
            solvedState->update();
        }
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
        QMetaObject::invokeMethod(app.get(), [=] {
            text->clear();
            text->clearHistory();
        });
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
                std::vector<std::string> allSolutions;

                while (slnQ->hasNext()) {
                    std::string solution = slnQ->pop();
                    allSolutions.push_back(solution);
                    QString solutionString = QString(solution.c_str());
                    QMetaObject::invokeMethod(app.get(), [=] {
                        text->append(solutionString);
                        text->clearHistory();
                    });
                }
                if (sortSelector->currentText() != "disabled") {
                    std::string sortedSolutions = "";
                    if (sortSelector->currentText() == "fingertricks")
                        sortedSolutions = FingertrickSolutionSorter().sortString(allSolutions);
                    else if (sortSelector->currentText() == "length")
                        sortedSolutions = SolutionSorter().sortString(allSolutions);
                    QMetaObject::invokeMethod(app.get(), [=, &text] {
                        text->clear();
                        text->clearHistory();
                        text->append(QString(sortedSolutions.c_str()));
                    });
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
