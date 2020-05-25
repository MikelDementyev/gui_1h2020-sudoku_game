#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QFont>
#include <QPalette>
#include <QString>
#include <QFrame>
#include "cellbutton.h"
#include <QTimer>
#include <QCloseEvent>

#include "datamodeltable.h"

struct Point {
    int x,y;
};
struct Button {
    Point point;
    QString str;
};

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void randomGenerate(int total);
    void spawnCells();
    bool isInvalid(int a, int b, int su[][9]);
    bool isNotSolvable(int total);
    bool canBeFilled(int *blank[], int n, int total);
    void createBlankPointers(int *blank[], int amountOfBlank[], int total);
    void blankCalculate(int amountOfBlank[]);
    int smallest(int array[], int x);
    void clearPage1();
    void redNumberBlanks();
    void greenNumberCells();
    QFrame *coverCells();

private slots:
    void undo();
    void gameStart();
    void newGameStart();
    void back2Menu();
    void showAnswer();
    void checkAnswer();
    void blinkResult();
    void clearAnswer();
    void help();
    void back2Game();
    void score();

    void on_HelpBackButton_2_clicked();
    void closeEvent(QCloseEvent *event);
    void on_score_clicked();

private:
    int countCorrect = 0;
    int countWrong = 0;
    int constHints = 0;
    dataModelTable *ModelTable;
    Ui::MainWindow *ui;
    int sudoku[9][9], ansSudoku[9][9], userSudoku[9][9];
    bool isShowingAns;
    QFrame *cover;
    QTimer *blinkTimer;
    QPushButton *CustomFinish;
    int random_min = 28, random_max = 33;
};


#endif // MAINWINDOW_H
