#pragma once

#include <QMainWindow>
#include "QBasicController.h"
#include "QBasicCmdParser.h"
#include "QBasicCode.h"
#include "QBasicExpression.h"
#include "exceptions.h"
#include "ui_QBasic.h"

using commands::QBasicCmdParser;
using commands::Command;

class QBasic : public QMainWindow
{
    Q_OBJECT

public:
    QBasic(QWidget* parent = nullptr);
    ~QBasic();

private:
    friend class QBasicController;
    QBasicController* controller = nullptr;
    QBasicCmdParser* parser = nullptr;

private:
    QBasicExpression* expression = nullptr;

private:
    void inform(const QString& str) const;

private:
    Command parseCommand(const QString& str) const;

private:
    QBasicCode code;

private:
    void executeCmd(int no);

private:
    void initSlots();

private slots:
    void on_cmdLineEdit_returnPressed();
    void on_btnClearCode_pressed();
    void on_btnRunCode_pressed();
    void on_btnLoadCode_pressed();

private:
    Ui::QBasic* ui;
};
