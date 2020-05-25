#include "cellbutton.h"
#include "mainwindow.h"
#include <QString>
#include <QDebug>
#include <QKeyEvent>

using namespace std;

extern CellButton *cell[9][9];
vector<CellButton*> selectedCells[17];

vector<Button> but;

CellButton::CellButton(QWidget *parent)
    : QPushButton(parent)
{
    setStyleSheet("border-width: 2px;"
                  "border-style: solid;");
    setFont(QFont("Consolas",24));
    setCheckable(false);
    setAutoExclusive(false);
    isSelected=false;
    isChangeable=true;
    isQuested = false;
    show();
}

void deleteColorValue(CellButton *button, QString param) {
    int fromColor = button->styleSheet().indexOf(param), toColor;
    if(fromColor!=-1) {
        toColor = button->styleSheet().indexOf(";",fromColor);
        button->setStyleSheet(button->styleSheet().remove(fromColor,toColor-fromColor+1));
    }
}

void resetBackgroundColor() {

    for(int i=0;i<9;i++) {
        for(int j=0;j<9;j++) {
            deleteColorValue(cell[i][j], "background-color");
            deleteColorValue(cell[i][j], "border-color");
            if (((i>=3&&i<6)||(j>=3&&j<6))&&!((i>=3&&i<6)&&(j>=3&&j<6))) {
                cell[i][j]->setStyleSheet((cell[i][j]->styleSheet()).append("background-color: #F4F3D3;"));
            } else {
                cell[i][j]->setStyleSheet((cell[i][j]->styleSheet()).append("background-color: #FFFFFF;"));
            }
        }
    }
}

Point findIndexButton(CellButton *button) {
    for (int x = 0; x < 9; x++) {
        for (int y = 0; y < 9; y++) {
            if (cell[x][y] == button) {
                return Point{x,y};
            }
        }
    }
    return Point{0,0};
}

void findIndex(CellButton *button) {
    selectedCells->clear();
    resetBackgroundColor();
    Point point = findIndexButton(button);
    int indexX = point.x;
    int indexY = point.y;

    for (int x = 0; x < 9; x++) {
        if (cell[x][indexY] != button){
            selectedCells->push_back(cell[x][indexY]);
        }
    }
    for (int y = 0; y < 9; y++) {
        selectedCells->push_back(cell[indexX][y]);
    }
}

void changeColorActions(CellButton *button, CellButton *currentButton) {

    QString stringParam = button == currentButton ? "border-color" : "background-color";
    deleteColorValue(button, stringParam);
    //add color
    QString stringParamWithValue = button == currentButton ? "border-color: #FF0F0F;" : "background-color: #F7C4C4;";
    button->setStyleSheet(button->styleSheet().append(stringParamWithValue));

    deleteColorValue(button, "border-width");
    QString borderWidthParam = button == currentButton ? "border-width: 5px;" : "border-width: 2px;";
    button->setStyleSheet(button->styleSheet().append(borderWidthParam));
}

void selectedActions(CellButton *button) {
    if(!button->isSelected) {
        changeColorActions(button, button);
        button->isSelected=true;
        button->setFocus();
    } else {
        emit button->quest();
    }
}

void CellButton::selected() {
    findIndex(this);
    selectedActions(this);
    for(unsigned i = 0; i < selectedCells->size(); i++) {
        changeColorActions(selectedCells->at(i), this);
    }
    return;
}
void CellButton::quest() {
    if(isChangeable && !isQuested) {
        setText(text().append("?"));
        isQuested = true;
    } else if(isChangeable && isQuested) {
        QString str = text();
        str.chop(1);
        setText(str);
        isQuested = false;
    }
    return;
}

void deselectColorActions(CellButton *button) {
    deleteColorValue(button, "border-color");
    deleteColorValue(button, "border-width");
    button->setStyleSheet(button->styleSheet().append("border-width: 2px;"));
}

void deselectedActions(CellButton *button) {
    deselectColorActions(button);
}

void CellButton::deselected() {
    this->isSelected=false;
    for (unsigned i = 0; i < selectedCells->size(); i++) {
        if (selectedCells->at(i) == this) {
            return;
        }
    }
    deselectedActions(this);
    return;
}

void CellButton::keyPressEvent(QKeyEvent *e) {
    if(isSelected) {
        but.push_back(Button{findIndexButton(this), this->text()});
        switch (e->key()) {
        case Qt::Key_1:
            if(isChangeable) setText("1");
            if(isQuested) setText(text().append("?"));
            break;
        case Qt::Key_2:
            if(isChangeable) setText("2");
            if(isQuested) setText(text().append("?"));
            break;
        case Qt::Key_3:
            if(isChangeable) setText("3");
            if(isQuested) setText(text().append("?"));
            break;
        case Qt::Key_4:
            if(isChangeable) setText("4");
            if(isQuested) setText(text().append("?"));
            break;
        case Qt::Key_5:
            if(isChangeable) setText("5");
            if(isQuested) setText(text().append("?"));
            break;
        case Qt::Key_6:
            if(isChangeable) setText("6");
            if(isQuested) setText(text().append("?"));
            break;
        case Qt::Key_7:
            if(isChangeable) setText("7");
            if(isQuested) setText(text().append("?"));
            break;
        case Qt::Key_8:
            if(isChangeable) setText("8");
            if(isQuested) setText(text().append("?"));
            break;
        case Qt::Key_9:
            if(isChangeable) setText("9");
            if(isQuested) setText(text().append("?"));
            break;
        case Qt::Key_0:
        case Qt::Key_Backspace:
            if(isChangeable) setText("");
            if(isQuested) isQuested = false;
            break;
        }
    }
    return;
}

void CellButton::setChangeable(bool can) {
    if(can) isChangeable=true;
    else isChangeable=false;
    return;
}
bool CellButton::beingSelected() const {
    return isSelected;
}
bool CellButton::beingQuested() const {
    return isQuested;
}
