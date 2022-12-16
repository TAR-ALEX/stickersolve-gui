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
static std::map<V, K> reverseMap(const std::map<K, V>& m) {
    std::map<V, K> r;
    for (const auto& kv : m) r[kv.second] = kv.first;
    return r;
}


template <int order = 3>
class NxNEditor : public AspectRatioWidget {
private:
    void init() {
        cube = new PuzzleCube<order>(idToColor[-1], this);
        this->setWidget(cube);

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
            cube,
        };
        colorToId = reverseMap(idToColor);

        cube->activeColor = colorPanel->color;

        cube->addSubWidget(colorPanel);

        colorPanel->onClick = [&](auto self, auto mouseButtons) {
            cout << "clicked(checked = " << mouseButtons << ");\n";
            cube->activeColor = self->color;
        };
    }

public:
    PuzzleCube<order>* cube = nullptr;
    map<int, jptr<QColor>> idToColor = {
        {0, jptr<QColor>{0, 255, 0}},
        {1, jptr<QColor>{255, 255, 0}},
        {2, jptr<QColor>{255, 0, 0}},
        {3, jptr<QColor>{255, 255, 255}},
        {4, jptr<QColor>{255, 144, 0}},
        {5, jptr<QColor>{0, 100, 255}},
        {-1, jptr<QColor>{127, 127, 127}},
    };

    map<jptr<QColor>, int> colorToId;


    State colorsToState(std::vector<jptr<QColor>> qcolors) {
        State pzl;
        for (auto& c : qcolors) { pzl.push_back(colorToId[c]); }
        return pzl;
    }

    std::vector<jptr<QColor>> stateToColors(State s) {
        std::vector<jptr<QColor>> qcolors;
        for (auto& v : s) { qcolors.push_back(idToColor[v]); }
        return qcolors;
    }

    void setState(State s) { cube->setColors(stateToColors(s)); }

    State getState() { return colorsToState(cube->getColors()); }

    NxNEditor(QWidget* parent = nullptr) : AspectRatioWidget(10.5, 7.5, parent) { init(); }

    NxNEditor(map<int, jptr<QColor>> idToColor, QWidget* parent = nullptr) : AspectRatioWidget(10.5, 7.5, parent) {
        this->idToColor = idToColor;
        init();
    }
};