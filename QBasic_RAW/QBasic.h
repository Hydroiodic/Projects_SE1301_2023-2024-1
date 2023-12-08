#pragma once

#include <QMainWindow>
#include <QMessageBox>
#include "QBasicController.h"
#include "QBasicCmdParser.h"
#include "QBasicCode.h"
#include "QBasicExpression.h"
#include "QBasicVarList.h"
#include "QBasicHelp.h"
#include "QBasicInform.h"
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
    QBasicHelp* helper = nullptr;

private:
    QBasicVarList* variables_list = nullptr;
    QBasicExpression* expression = nullptr;

public slots:
    void inform(const QString& str) const;

private:
    Command parseCommand(const QString& str) const;
    void dispenseCommand(const QString& cmd_text);

private:
    QBasicCode code;

private:
    void executeCmd(int no);
    void executeInstCmd(const Command& cmd);

private:
    void initSlots();

private:
    bool is_input = false;
    QBasicVar* variable_to_input = nullptr;
    void inputAssignVariable(const QString& str);

private:
    bool is_running = false;
    void clearRunningState();

private slots:
    void cmdLineEditReturnPressed();
    void btnClearCodePressed();
    void btnRunCodePressed();
    void btnLoadCodePressed();

public slots:
    void appendOutputText(const QString& str);
    void setInputState(QBasicVar* var);

private:
    Ui::QBasic ui;
};
