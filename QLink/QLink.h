// QLink.h
// header file for QLink.cpp

#pragma once

#include <QtWidgets/QMainWindow>
#include <QPushButton>
#include <QEvent>
#include <QTimer>
#include <QPainter>
#include <QFileDialog>
#include <QMessageBox>
#include <QThread>
#include "ui_QLink.h"
#include "QLinkIcon.h"
#include "QLinkCharactor.h"
#include "QLinkButton.h"
#include "QLinkMainTimer.h"
#include "QLinkMenuBar.h"
#include "QLinkInit.h"
#include "QLinkProp.h"
#include "QLinkManual.h"
#include "QLinkGameover.h"
#include "QLinkAboutBox.h"
#include "QLinkPainter.h"
#include "QLinkMultiPlayerWin.h"
#include "QLinkThread.h"
#include "QLinkInitPage.h"

class QLinkButton;

// main window framework, offering the API of the main window
class QLink : public QMainWindow
{
    Q_OBJECT

private:
    static const int _MAX_M = 10, _MAX_N = 15;
    static const int _TOTAL_ICON_NUM = 11;
    const double _button_size_ratio = 3. / 5;       // originally 4. / 5
    const double _shown_width_ratio = 7. / 5;       // originally 11. / 10

    // kid-button width and kid height-width ratios, one of them determined by the other and the size of the image
    const double _kid_button_width_ratio = 1. / 3;
    const double _kid_height_width_ratio = 21. / 25;

    // _find_kid_ratio is used to expand the searching area
    const double _find_kid_ratio = (_shown_width_ratio + 1) / 2;

    // set the speed of the kid
    const double _original_kid_speed = 1.1;
    const double _speed_up_kid_speed = 1.5;

public:
    QLink(QWidget* parent = nullptr);
    ~QLink();

    const double _game_width_ratio = 4.0 / 5;

    int getButtonWidth() const;

    friend class QLinkMenuBar;
    friend class QLinkMenu;
    friend class QLinkInit;
    friend class QLinkProp;
    friend class QLinkGameover;
    friend class QLinkCharactor;
    friend class QLinkTest;

    // basic operation: create a game and end a game
    void startGame(int M = 5, int N = 10, int _used_kinds = 5);
    void startGame(int M, int N, const int data[][_MAX_N + 1]);
    void clearGame();
    void stopGame();

private:
    Ui::QLinkClass ui;
    QLinkIcon icon;

    // an initialization page for the game, but the code is very ugly
    bool _game_initialized = false;
    QLinkInitPage _init_class;

    void setInitialPage();
    void prepareGame();
    void initializeKid();

    // data is used to create buttons and show them only, 
    // _simulated_data is the one which is used for operating
    QLinkButton*** data = nullptr;
    QPair<int, int>** _prev_button = nullptr;
    static const int INF = 0x3f3f3f3f;
    int** _simulated_data = nullptr;
    int** _cur_turn_count = nullptr;
    int M = 5, N = 5;

    static const int _kid_num = 2;
    int score[_kid_num] {};
    void setScoreText();

    // _main_timer is used to show the time left for the game
    // _main_painter is used to draw the layout of the main window
    QLinkMainTimer* _main_timer = nullptr;
    QLinkPainter* _main_painter = nullptr;

    // QLinkThread, to provide a way to paint the page
    QTimer* _painter_timer = nullptr;
    QPixmap _painter_map;
    QThread* thread = nullptr;
    void emitPaintingSignal();
    void initializeThread();

    // QLinkMenuBar prepares a menubar for this game
    QLinkMenuBar* menu = nullptr;
    
    // QLinkInit is a son-window of the main window
    QLinkInit* _init_window = nullptr;

    // _game_stopped is used to judge whether the game is paused now
    bool _game_stopped = false;

    // QLinkManual is a son-window of the main window to give instructions
    QLinkManual* manual = nullptr;

    // _left_buttons is used to judge whether the player wins or not
    int _left_buttons = 0;

    // QLinkGameover is a app-modal-window to inform the end of the game
    QLinkGameover* _gameover_window = nullptr;
    void createGameoverWindow(int result);

    // QLinkMultiPlayerWin is a app-modal-window to provide winners for multi-game
    QLinkMultiPlayerWin* _multi_window = nullptr;
    void createMultiWindow();
    
    // QLinkAboutBox is a app-modal-window to provide info about this program
    QLinkAboutBox* _about_window = nullptr;
    void createAboutBox();

    // some operations to move the main charactor
    QLinkCharactor* kid[_kid_num] {};
    bool _in_game = false;
    void initializeGame(int _M, int _N, int _used_kinds);
    void setKidPosRandomly();

    // operations about switching between different game modes
    int _game_mode = 0;
    void switchGameMode();
    void updateGameMode();

    void settleMultiGame();

    // some basic operations, mainly focusing on updating the size of each control
    //     and deploying functions of other classes
    void setBackground();
    void resizeButtons();
    void drawInfoLine();
    void paintButtonSurround();

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

    void keyPressEvent(QKeyEvent* event) override;
    void keyPressSetMove(int _kid_no, int _cur_key, bool isRepeated);

    void keyReleaseEvent(QKeyEvent* event) override;
    void keyReleaseSetStill(int _kid_no, int _cur_key);

    void closeEvent(QCloseEvent* event) override;

    bool eventFilter(QObject* watched, QEvent* event);

private:
    void buttonPressed(QPair<int, int>& _pressed_btn, QPair<int, int>& _pressing_btn, int _kid_no);

    /******************  Below are QLinkProp main functions  *****************/

    // QLinkProp provides methods to exhibit a prop
    QLinkProp* prop = nullptr;
    QPair<int, int> _first_highlight_btn { -1, -1 }, 
        _second_highlight_btn { -1, -1 };

    // _layout_version is used to avoid linking affected by shuffle
    int _layout_version = 0;
    QVector<int> _layout_version_of_linking;

    // record the current dizzy state of each charactor
    bool dizzy[_kid_num] {};
    bool freeze[_kid_num] {};

    void correctKidPos();
    void checkProp();
    void checkSolution();

    /******************  Below are QLinkFile main functions  *****************/

    struct _FILE_STRUCTURE {
        // save the mode of the game
        int mode = 0;

        // save the basic info of the current game
        int _cur_data[_MAX_M + 1][_MAX_N + 1]{};
        int _left_time = 0, score[_kid_num]{};
        int M = 0, N = 0;

        // save the info about the positions of kids
        QPair<double, double> _kid_pos[_kid_num];

        // save the info of the icons to show, ensuring the two games are same
        int _used_icon_num = 0;
        int _icon_order[_TOTAL_ICON_NUM]{};

        // save the width of the kid to ensure the position of kids won't change due to the size of the window
        int _width_of_kid = 0;

        // save the hash-value produced by the function to ensure the file is correctly selected
        long long _hash_value = -1;
    };

    void saveToFile();
    void loadFromFile();
    void saveDataToStructure(_FILE_STRUCTURE& content) const;
    void loadDataFromStructure(const _FILE_STRUCTURE& content);
    long long getHashValue(const _FILE_STRUCTURE& content) const;

    /*******************  Below are QLink main algorithms  *******************/

    bool _need_update = true;
    enum Direction { UP, LEFT, DOWN, RIGHT, NONE };
    QLinkPainter* _link_painter = nullptr;

    QPair<int, int> _cur_pressed_button[_kid_num];

    // _link_lines is used to save the path of linking, _link_first_last is used to 
    //     save the start point and the last point of a specific path
    QList<QPoint> _link_lines;
    QList<QPair<int, int>> _link_first_last;

    QPair<int, int> findKid(const QLinkCharactor* _kid_, double _width_ratio = 1.) const;
    QPair<int, int> findPos(const QPoint& _point_, double _width_ratio = 1.) const;

    // defined by the rules
    int _max_allow_turn = 2;

    // dfs, see https://www.luogu.com.cn/problem/P1649
    void linkTwoButtons(const QPair<int, int>&, const QPair<int, int>&, int);
    bool dfs(int, int, int, Direction, int, const QPair<int, int>&);
    void getPath(const QPair<int, int>& _end_btn);
    void prepareNextLink(QTimer* _temp_timer, int _kid_no);
    void showPath();
    void resetDfsState();

    /********************  Below are QLinkTest functions  ********************/

public:
    void initializeTest();
    int linkTwoButtonsTest(const QPair<int, int>& btn_1, const QPair<int, int>& btn_2);

signals:
    void linkState(int);
    void getPixmap(int width, int height, double _pen_width, QList<QPair<QList<QPoint>, QColor>> list);

private slots:
    void setLinkState(int _state_code);
    void pauseGame();
    void setPixmap(QPixmap map, QRegion region);
};
