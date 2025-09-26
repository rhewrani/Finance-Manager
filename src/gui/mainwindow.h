#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "core/manager.h"
#include "configwindow.h"
#include "settingswindow.h"
#include "dialogs/info.h"


QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void closeEvent(QCloseEvent *event) override;

    // Splitting the functions allows the program to also just update single elements without having to update all (even if just one element was changed)
    void MainPage_updateDataFrontendAll(const QMap<int, QJsonObject> &in, const QMap<int, QJsonObject> &ex, bool updatePr);
    void MainPage_updateDataFrontendIncome(const QMap<int, QJsonObject> &in, bool resize = true);
    void MainPage_updateDataFrontendExpenses(const QMap<int, QJsonObject> &ex, bool resize = true);
    void MainPage_updateDataFrontendProfit(int sum);
    void MainPage_updateDataFrontendIncSum(int incSum);
    void MainPage_updateDataFrontendExpSum(int expSum);

private slots:

    void on_BTN_OPEN_INC_clicked();

    void on_BTN_OPEN_EXP_clicked();

    void on_actionOpen_configuration_triggered();

    void on_CB_MTH_checkStateChanged(const Qt::CheckState &arg1);

    void on_actionInfo_triggered();

    void on_actionOpen_settings_triggered();

    void on_actionSave_all_triggered();

    void on_actionLoad_backup_triggered();

private:
    Ui::MainWindow *ui;
    Manager *manager;
    Configwindow *configwindow;
    Info *info;
    Settingswindow *settingswindow;


    void Init();
    void InitUI();

    bool showSumType = 0; // 0 = all, 1 = strict with only monthly expenses shown in profit
    bool messageBoxAfterSaveRestartRequired = false;

};
#endif // MAINWINDOW_H
