#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QCoreApplication>
#include <QItemSelectionModel>
#include <ctime>
#include <QDebug>
#include <QFile>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QAbstractItemView>

using namespace std;

CellButton *cell[9][9];
int tempSudoku[9][9];
extern vector <Button> but;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    blinkTimer=NULL;
    CustomFinish=NULL;
    cover=NULL;
    isShowingAns=false;    
    ui->Pages->setCurrentIndex(0);

    connect(ui->QuitButton,SIGNAL(clicked()),qApp,SLOT(quit()));
    connect(ui->StartButton,SIGNAL(clicked()),this,SLOT(gameStart()));
    connect(ui->AnswerButton,SIGNAL(clicked()),this,SLOT(showAnswer()));
    connect(ui->CheckButton,SIGNAL(clicked()),this,SLOT(checkAnswer()));
    connect(ui->MenuButton,SIGNAL(clicked()),this,SLOT(back2Menu()));
    connect(ui->NewGameButton,SIGNAL(clicked()),this,SLOT(newGameStart()));
    connect(ui->ClearButton,SIGNAL(clicked()),this,SLOT(clearAnswer()));
    connect(ui->HelpButton,SIGNAL(clicked()),this,SLOT(help()));
    connect(ui->HelpBackButton,SIGNAL(clicked()),this,SLOT(back2Game()));
    connect(ui->Undo, SIGNAL(clicked()), this, SLOT(undo()));
    connect(ui->ScoreBackButton, SIGNAL(clicked()), this, SLOT(back2Game()));
    connect(ui->Score, SIGNAL(clicked()), this, SLOT(score()));
    QFile Newfile("./score.txt");
    if (!(Newfile.exists())&&Newfile.open(QIODevice::WriteOnly))
    {
        Newfile.write("best 5 2 2 second 3 3 3 worse 1 5 8 player 3 5 10 finik 1 1 0");
        Newfile.close();
    }
}

void MainWindow::score() {
    QStandardItemModel *model = new QStandardItemModel;
    QStandardItem *item;
    QStringList horizontalHeader;
    horizontalHeader.append("Name");
    horizontalHeader.append("correct answer");
    horizontalHeader.append("wrong answer");
    horizontalHeader.append("hints");
    QString str="";
    QFile file("./score.txt");
    if ((file.exists())&&(file.open(QIODevice::ReadOnly))) {
       str=file.readAll();
       file.close();
    }
    QStringList myStringList = str.split(' ');
    myStringList.append(ui->lineEdit->text());
    myStringList.append(str.setNum(countCorrect));
    myStringList.append(str.setNum(countWrong));
    myStringList.append(str.setNum(constHints));
    //Заголовки строк
    QStringList verticalHeader;
    int sizeTable = (myStringList.length())/4;
    for (int i = 1; i <= sizeTable; i++) {
      verticalHeader.append(str.setNum(i));
    }
    model->setHorizontalHeaderLabels(horizontalHeader);
    model->setVerticalHeaderLabels(verticalHeader);
    int len = 0;
    for (int i = 0; i < sizeTable; i++) {
        for (int j = 0; j < 4; j++) {
            item = new QStandardItem(myStringList[len]);
            item->setEditable(false);
            model->setItem(i, j, item);
            len++;
        }
    }
    ui->scoreTableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->scoreTableView->setModel(model);
    ui->scoreTableView->resizeRowsToContents();
    ui->scoreTableView->resizeColumnsToContents();

    QStandardItemModel *modelList = new QStandardItemModel;
    QStringList tmpList;
    int numDel = myStringList[1].toInt();
    int numDelIndex = 1;
    len = 5;
    for (int j = 0; j < 3; j++) {
        for (int i = 1; i < sizeTable; i++) {
            if (!myStringList[len].isNull()) {
                int tmpInt = myStringList[len].toInt();
                if (tmpInt>numDel) {
                    numDel = tmpInt;
                    numDelIndex = len;
                }
                len += 4;
            }
        }
        sizeTable--;
        tmpList.append(myStringList[numDelIndex-1]);
        for (int i = 0; i < 4; i ++)
          myStringList.removeAt(numDelIndex-1);
        numDel = myStringList[1].toInt();
        len = 5;
        numDelIndex = 1;
    }
    for (int j = 0; j < tmpList.length(); j++) {
        item = new QStandardItem(tmpList[j]);
        item->setEditable(false);
        modelList->setItem(j, 0, item);
    }
    ui->ScoreListView->setModel(modelList);
    ui->Pages->setCurrentIndex(2);
    ui->centralWidget->setFocus();
    return;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::undo() {
    if(but.size()>0) {
        Button curbut = but.back();
        but.pop_back();
        cell[curbut.point.x][curbut.point.y]->setText(curbut.str);
    }
}

void MainWindow::gameStart() {
    ui->Pages->setCurrentIndex(1);
    srand(time(NULL));
    randomGenerate(random_min+rand()%(random_max-random_min+1));
    spawnCells();
    redNumberBlanks();
    return;
}

void MainWindow::newGameStart() {
    clearPage1();
    ui->centralWidget->setFocus();
    srand(time(NULL));
    randomGenerate(random_min+rand()%(random_max-random_min+1));
    spawnCells();
    redNumberBlanks();
    return;
}

void MainWindow::showAnswer() {
    constHints++;
    int i, j;
    static int x, y;
    QString str;
    ui->centralWidget->setFocus();
    if(!isShowingAns) {
        cover = coverCells();
        for(i=0;i<9;++i) {
            for(j=0;j<9;++j) {
                str = cell[i][j]->text();
                if(str.endsWith("?")) str.chop(1);
                if(str!="") {
                    userSudoku[i][j]=str.toInt();
                } else userSudoku[i][j]=0;
            }
        }
        for(i=0;i<9;++i) {
            for(j=0;j<9;++j) {
                cell[i][j]->setText(str.setNum(ansSudoku[i][j]));
            }
        }
        for(i=0;i<9;++i) {
            for(j=0;j<9;++j) {
                if(cell[i][j]->beingSelected()) {
                    x=i;
                    y=j;
                    cell[i][j]->deselected();
                    break;
                }
            }
            if(j!=9) break;
        }
        if(j==9) {
            x=87;
        }
        ui->CheckButton->hide();
        ui->NewGameButton->hide();
        ui->MenuButton->hide();
        ui->Result->hide();
        ui->HelpButton->hide();
        ui->ClearButton->hide();
        ui->AnswerButton->setText("Resume");
        isShowingAns = true;
    } else {
        for(i=0;i<9;i++) {
            for(j=0;j<9;j++) {
                if(userSudoku[i][j]!=0) {
                    cell[i][j]->setText(str.setNum(userSudoku[i][j]));
                } else cell[i][j]->setText("");
                if(cell[i][j]->beingQuested()) cell[i][j]->setText(cell[i][j]->text().append("?"));
            }
        }
        ui->CheckButton->show();
        ui->NewGameButton->show();
        ui->MenuButton->show();
        ui->Result->show();
        ui->HelpButton->show();
        ui->ClearButton->show();
        ui->AnswerButton->setText("ShowAnswer");
        if(x!=87) cell[x][y]->selected();
        if(cover!=NULL) delete cover;
        cover=NULL;
        isShowingAns = false;
    }
}

void MainWindow::checkAnswer() {
    int i, j;
    QString str;
    bool doesUserWin = true;
    for(i=0;i<9;++i) {
        for(j=0;j<9;++j) {
            str = cell[i][j]->text();
            if(str.endsWith("?")) str.chop(1);
            if(str!="") {
                userSudoku[i][j]=str.toInt();
            } else userSudoku[i][j]=0;
        }
    }
    for(i=0;i<9;++i) {
        for(j=0;j<9;++j) {
            if(userSudoku[i][j]==0||isInvalid(i,j,userSudoku)) {
                doesUserWin = false;
                break;
            }
        }
        if(!doesUserWin) break;
    }
    if(doesUserWin) {
        countCorrect++;
        greenNumberCells();
        ui->centralWidget->setFocus();
        ui->Result->setStyleSheet("color: #76E023;");
        ui->Result->setText("You win!");
        cover = coverCells();
        ui->AnswerButton->hide();
        ui->CheckButton->hide();
        ui->HelpButton->hide();
        ui->ClearButton->hide();
        blinkTimer = new QTimer(this);
        connect(blinkTimer, SIGNAL(timeout()), this, SLOT(blinkResult()));
        blinkTimer->start(250);
        for(i=0;i<9;++i) for(j=0;j<9;++j) cell[i][j]->deselected();
    } else {
        countWrong++;
        for(i=0;i<9;++i) for(j=0;j<9;++j) if(cell[i][j]->beingSelected()) cell[i][j]->setFocus();
        ui->Result->setStyleSheet("color: red;");
        ui->Result->setText("Incorrect!");
        disconnect(ui->CheckButton,0,0,0);
        blinkTimer = new QTimer(this);
        connect(blinkTimer, SIGNAL(timeout()), this, SLOT(blinkResult()));
        blinkTimer->start(250);
    }
}

void MainWindow::help() {
    ui->Pages->setCurrentIndex(3);
    ui->centralWidget->setFocus();
    return;
}

void MainWindow::back2Game() {
    ui->Pages->setCurrentIndex(1);
    for(int i=0;i<9;++i) for(int j=0;j<9;++j) if(cell[i][j]->beingSelected()) cell[i][j]->setFocus();
    return;
}

void MainWindow::blinkResult() {
    static int x=0;
    if(x>5) {
        if(blinkTimer!=NULL) delete blinkTimer;
        blinkTimer=NULL;
        x=0;
        connect(ui->CheckButton,SIGNAL(clicked(bool)),this,SLOT(checkAnswer()));
        return;
    }
    if(x%2==0) ui->Result->hide();
    else ui->Result->show();
    ++x;
    return;
}

QFrame* MainWindow::coverCells() {
    QFrame* frame = new QFrame(ui->page_1);
    frame->setGeometry(QRect(QPoint(100,100),QSize(450,450)));
    frame->setStyleSheet("background-color: transparent");
    frame->show();
    return frame;
}

void MainWindow::randomGenerate(int total) {
    int i, j, k, num;
    do {
        for(i=0;i<9;i++) for(j=0;j<9;j++) sudoku[i][j]=0;
        for(num=0;num<total;++num) {
            do {
                i=rand()%9;
                j=rand()%9;
            } while(sudoku[i][j]!=0);
            sudoku[i][j]=rand()%9+1;
            for(k=0;k<9;k++) {
                sudoku[i][j]=(sudoku[i][j]==9)?1:sudoku[i][j]+1;
                if(!isInvalid(i, j, sudoku)) break;
            }
            if(k==9) break;
        }
        if(k==9) continue;
        for(i=0;i<9;i++) for(j=0;j<9;j++) ansSudoku[i][j]=sudoku[i][j];
        for(i=0;i<9;i++) for(j=0;j<9;j++) tempSudoku[i][j]=sudoku[i][j];
    } while(k==9||isNotSolvable(total));
    for(i=0;i<9;i++) for(j=0;j<9;j++) userSudoku[i][j]=sudoku[i][j];
    return;
}

void MainWindow::redNumberBlanks() {
    for(int i=0;i<9;++i)
        for(int j=0;j<9;++j)
            if(sudoku[i][j]==0) cell[i][j]->setStyleSheet((cell[i][j]->styleSheet()).append("color: #FF0F0F;"));
    return;
}

void MainWindow::greenNumberCells() {
    for(int i=0;i<9;++i)
        for(int j=0;j<9;++j)
            cell[i][j]->setStyleSheet((cell[i][j]->styleSheet()).append("color: #76E023;"));
    return;
}

void MainWindow::spawnCells() {
    int i, j, a, b;
    QString str;
    for(i=0;i<9;i++) {
        for(j=0;j<9;j++) {
            cell[i][j] = new CellButton(ui->sudokuBox);
            if(sudoku[i][j]!=0) {
                cell[i][j]->setText(str.setNum(sudoku[i][j]));
            } else {
                cell[i][j]->setText("");
            }
            if (((i>=3&&i<6)||(j>=3&&j<6))&&!((i>=3&&i<6)&&(j>=3&&j<6))) {
                cell[i][j]->setStyleSheet((cell[i][j]->styleSheet()).append("background-color: #F4F3D3;"));
            }
            else {
                cell[i][j]->setStyleSheet((cell[i][j]->styleSheet()).append("background-color: #FFFFFF;"));
            }
            cell[i][j]->setGeometry(QRect(QPoint(2+50*i,2+50*j),QSize(50,50)));
        }
    }

    for(i=0;i<9;i++) {
        for(j=0;j<9;j++) {
            if(sudoku[i][j]==0) {
                connect(cell[i][j],SIGNAL(clicked(bool)),cell[i][j],SLOT(selected()));
            } else {
                cell[i][j]->setChangeable(false);
            }
            for(a=0;a<9;a++) {
                for(b=0;b<9;b++) {
                    if(!(i==a&&j==b)) {
                        connect(cell[i][j],SIGNAL(clicked(bool)),cell[a][b],SLOT(deselected()));
                    }
                }
            }
        }
    }
    return;
}

void MainWindow::back2Menu() {
    clearPage1();
    ui->Pages->setCurrentIndex(0);
    ui->centralWidget->setFocus();
}

void MainWindow::clearPage1() {
    for (int i=0;i<9;++i) {
        for(int j=0;j<9;++j) {
            if(cell[i][j]!=NULL) delete cell[i][j];
            cell[i][j]=NULL;
            sudoku[i][j]=ansSudoku[i][j]=userSudoku[i][j]=tempSudoku[i][j]=0;
        }
    }
    isShowingAns=false;
    if(cover!=NULL) delete cover;
    ui->Result->setText("");
    ui->MenuButton->show();
    ui->NewGameButton->show();
    ui->CheckButton->show();
    ui->AnswerButton->show();
}

void MainWindow::clearAnswer() {
    for(int i=0;i<9;++i) {
        for(int j=0;j<9;++j) {
            if(sudoku[i][j]==0) cell[i][j]->setText("");
            if(cell[i][j]->beingSelected()) cell[i][j]->deselected();
            if(cell[i][j]->beingQuested()) cell[i][j]->quest();
        }
    }
    ui->centralWidget->setFocus();
    return;
}

bool MainWindow::isInvalid(int a, int b, int su[][9]) {
    int i, j, x, y;
    for(i=0;i<9;i++) {
        if(i!=a && su[a][b]==su[i][b]) return true;
        else if(i!=b && su[a][b]==su[a][i]) return true;
    }
    for(i=0;i<3;i++) {
        for(j=0;j<3;j++) {
            x=a/3*3+i;
            y=b/3*3+j;
            if(x!=a && y!=b && su[a][b]==su[x][y]) return true;
        }
    }
    return false;
}

bool MainWindow::isNotSolvable(int total) {
    int amountOfBlank[27];
    int *blank[81-total];
    createBlankPointers(blank, amountOfBlank, total);
    return !canBeFilled(blank, 0, total);
}

bool MainWindow::canBeFilled(int *blank[], int n, int total) {
    int a, b, *address=&ansSudoku[0][0];
    if(n==81-total) return true;
    for(int num=1;num<=9;num++) {
        *blank[n]=num;
        a=(blank[n]-address)/9;
        b=(blank[n]-address)%9;
        if(!isInvalid(a, b, ansSudoku))
            if(canBeFilled(blank, n+1, total)) return true;
    }
    *blank[n]=10;
    return false;
}

void MainWindow::createBlankPointers(int *blank[], int amountOfBlank[], int total) {
    int i, x, y, n=0, k;
    while(n<81-total) {
        blankCalculate(amountOfBlank);
        k=smallest(amountOfBlank,27);
        if(k<9) {
            for(i=0;i<9;++i) {
                if(ansSudoku[k][i]==0) {
                    blank[n]=&ansSudoku[k][i];
                    *blank[n]=10;
                    ++n;
                }
            }
        } else if(k<18) {
            for(i=0;i<9;++i) {
                if(ansSudoku[i][k-9]==0) {
                    blank[n]=&ansSudoku[i][k-9];
                    *blank[n]=10;
                    ++n;
                }
            }
        } else if(k<21) {
            for(x=3*(k-18);x<3*(k-18)+3;++x) {
                for(y=0;y<3;++y) {
                    if(ansSudoku[x][y]==0) {
                        blank[n]=&ansSudoku[x][y];
                        *blank[n]=10;
                        ++n;
                    }
                }
            }
        } else if(k<24) {
            for(x=3*(k-21);x<3*(k-21)+3;++x) {
                for(y=3;y<6;++y) {
                    if(ansSudoku[x][y]==0) {
                        blank[n]=&ansSudoku[x][y];
                        *blank[n]=10;
                        ++n;
                    }
                }
            }
        } else {
            for(x=3*(k-24);x<3*(k-24)+3;++x) {
                for(y=6;y<9;++y) {
                    if(ansSudoku[x][y]==0) {
                        blank[n]=&ansSudoku[x][y];
                        *blank[n]=10;
                        ++n;
                    }
                }
            }
        }
    }
}

void MainWindow::blankCalculate(int amountOfBlank[]) {
    int i, j, x, y;
    for(i=0;i<27;i++) {
        amountOfBlank[i]=0;
        if(i<9) {
            for(j=0;j<9;j++) {
                if(ansSudoku[i][j]==0) amountOfBlank[i]++;
            }
        } else if(i<18) {
            for(j=0;j<9;j++) {
                if(ansSudoku[j][i-9]==0) amountOfBlank[i]++;
            }
        } else if(i<21){
            for(x=3*(i-18);x<3*(i-18)+3;x++) {
                for(y=0;y<3;y++) {
                    if(ansSudoku[x][y]==0) amountOfBlank[i]++;
                }
            }
        } else if(i<24){
            for(x=3*(i-21);x<3*(i-21)+3;x++) {
                for(y=3;y<6;y++) {
                    if(ansSudoku[x][y]==0) amountOfBlank[i]++;
                }
            }
        } else {
            for(x=3*(i-24);x<3*(i-24)+3;x++) {
                for(y=6;y<9;y++) {
                    if(ansSudoku[x][y]==0) amountOfBlank[i]++;
                }
            }
        }
    }
    return;
}

int MainWindow::smallest(int array[], int x) {
    int index=0, value=9;
    for(int i=0;i<x;i++) {
        if(array[i]>0 && array[i]<value) {
            value = array[i];
            index = i;
        }
    }
    return index;
}

void MainWindow::on_HelpBackButton_2_clicked() {}
void MainWindow::on_score_clicked() {}


void MainWindow::closeEvent(QCloseEvent *event)
{

    QString str;
    QFile file("./score.txt");
    if ((file.exists())&&(file.open(QIODevice::ReadOnly))) {
       str=file.readAll();
       file.close();
    }
    str.remove(str.length()-2,str.length());
    QStringList myStringList = str.split(' ');
    QString ch = " ";
    if (file.open(QIODevice::Append)) {
       file.write(ch.toUtf8());
       file.write(ui->lineEdit->text().toUtf8());
       file.write(ch.toUtf8());
       file.write(str.setNum(countCorrect).toUtf8());
       file.write(ch.toUtf8());
       file.write(str.setNum(countWrong).toUtf8());
       file.write(ch.toUtf8());
       file.write(str.setNum(constHints).toUtf8());
    }
    file.close();
}
