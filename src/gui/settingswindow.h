#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include "core/manager.h"
#include <QDialog>

class Manager;

namespace Ui {
class Settingswindow;
}

class Settingswindow : public QDialog
{
    Q_OBJECT

public:
    explicit Settingswindow(Manager *manager, QWidget *parent = nullptr);
    ~Settingswindow();

    void closeEvent(QCloseEvent *event) override;

    void sw_show();

signals:
    void signal_updatedSettings(bool state);

private slots:
    void on_TBTN_DIR_clicked();

    void on_CMBX_LNG_currentIndexChanged(int index);

    void on_CMBX_CUR_currentIndexChanged(int index);

    void on_CMBX_DATE_currentIndexChanged(int index);

    void on_CB_CD_checkStateChanged(const Qt::CheckState &arg1);

    void on_CB_EAS_checkStateChanged(const Qt::CheckState &arg1);

    void on_CB_BOAE_checkStateChanged(const Qt::CheckState &arg1);

    void on_SPBX_BU_valueChanged(int arg1);

    void on_CB_EL_checkStateChanged(const Qt::CheckState &arg1);

    void on_BTN_SAVE_clicked();

    void on_LN_FLPT_textChanged(const QString &arg1);

    void on_BTN_RES_clicked();

    void on_BTN_ER_clicked();

private:
    Ui::Settingswindow *ui;
    Manager *manager;

    void Init();
    void InitUI();

    void sw_setDataWindow();
    void checkVariables();

    Manager::appSettings& settings;
    Manager::appSettings settingsEdit = {     // EDIT VERSION: TEMPORARELY STORES CHANGED SETTINGS - USED TO CHECK WHETHER DATA HAS BEEN CHANGED //
        .bAutoSave = true,
        .bEnableLogging = false,
        .bConfirmDelete = true,
        .bBackupOnExit = true,
        .intLanguage = 0,
        .intCurrency = 0,
        .intDateFormat = 0,
        .intMaxBackups = 5,
        .strDataPath = ""
    };

    bool restartRequired = false;
    bool settingsChanged = false;
    bool saveButtonUsed = false;

};

#endif // SETTINGSWINDOW_H
