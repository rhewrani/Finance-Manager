#include "settingswindow.h"
#include "ui_settingswindow.h"
#include <QCloseEvent>


Settingswindow::Settingswindow(Manager *managerRef, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Settingswindow)
    , manager(managerRef)
    , settings(manager->getSettingsStruct())
{
    ui->setupUi(this);

    Init();
}

Settingswindow::~Settingswindow()
{
    delete ui;
}

void Settingswindow::closeEvent(QCloseEvent *event)
{
    checkVariables();

    if (saveButtonUsed == false && (settingsChanged || restartRequired)) {

        QString str = _("MB_CLST_CON");
        if (restartRequired) {
            str = _("MB_CLST_CON2");
        }

        QMessageBox msgBox(this);
        msgBox.setWindowTitle(_("MB_SAVE_TTL"));
        msgBox.setText(str);
        msgBox.setIcon(QMessageBox::Question);

        QPushButton *saveButton = msgBox.addButton(_("BTN_SAVE"), QMessageBox::AcceptRole);
        QPushButton *discardButton = msgBox.addButton(_("BTN_DIS"), QMessageBox::DestructiveRole);
        QPushButton *cancelButton = msgBox.addButton(_("BTN_CAN"), QMessageBox::RejectRole);

        msgBox.exec();

    if (msgBox.clickedButton() == saveButton) {

        emit signal_updatedSettings(restartRequired);

        if (manager->saveSettings(settingsEdit, restartRequired)) {
            event->accept();  // Close window
        }
    } else if (msgBox.clickedButton() == discardButton) {
        event->accept();  // Close window without saving
    } else {
        event->ignore();  // Don't close window
    }
    } else {
        event->accept();
    }
}

void Settingswindow::Init()
{

    InitUI();
    manager->log("Initialised settings window");

}

void Settingswindow::InitUI()
{
    setWindowTitle(_("SW_TTL"));
    ui->LBL_TITLE->setText(_("SW_TTL"));
    ui->LBL_LNG->setText(_("SW_LAN"));
    ui->LBL_CUR->setText(_("SW_CUR"));
    ui->LBL_DATE->setText(_("SW_DAT"));
    ui->LBL_FP->setText(_("SW_DATA_FP"));
    ui->CB_CD->setText(_("SW_CD"));
    ui->CB_EAS->setText(_("SW_EAS"));
    ui->CB_BOAE->setText(_("SW_BOAE"));
    ui->LBL_MXB->setText(_("SW_MXB"));
    ui->BTN_ER->setText(_("SW_ERASE"));
    ui->BTN_SAVE->setText(_("BTN_SAVE"));
    ui->BTN_RES->setText(_("BTN_RES"));
    ui->CB_DT->setText(_("SW_DT"));
    ui->CB_EL->setText(_("SW_EL"));
    ui->LBL_VER->setText(_("SW_VER") + " " + settings.strVersion);
}

void Settingswindow::sw_show() {
    sw_setDataWindow();
    show();
}

void Settingswindow::sw_setDataWindow()
{
    // this functions is called to input the actual current settings and therefore only used the actual values to input
    ui->CB_BOAE->setChecked(settings.bBackupOnExit);
    ui->CB_CD->setChecked(settings.bConfirmDelete);
    ui->CB_EAS->setChecked(settings.bAutoSave);
    ui->CB_EL->setChecked(settings.bEnableLogging);
    ui->SPBX_BU->setValue(settings.intMaxBackups);
    ui->CMBX_CUR->setCurrentIndex(settings.intCurrency);
    ui->CMBX_DATE->setCurrentIndex(settings.intDateFormat);
    ui->CMBX_LNG->setCurrentIndex(settings.intLanguage);
    ui->LN_FLPT->setText(settings.strDataPath);
    ui->CB_DT->setChecked(settings.bDevtools);

    settingsEdit.bAutoSave = settings.bAutoSave;
    settingsEdit.bEnableLogging = settings.bEnableLogging;
    settingsEdit.bConfirmDelete = settings.bConfirmDelete;
    settingsEdit.bBackupOnExit = settings.bBackupOnExit;
    settingsEdit.intLanguage = settings.intLanguage;
    settingsEdit.intCurrency = settings.intCurrency;
    settingsEdit.intDateFormat = settings.intDateFormat;
    settingsEdit.intMaxBackups = settings.intMaxBackups;
    settingsEdit.strDataPath = settings.strDataPath;

    if (settings.bDevtools == false) {
        ui->CB_EL->setHidden(true);
    }

    restartRequired = false;
    settingsChanged = false;
    saveButtonUsed = false;

}

void Settingswindow::checkVariables()
{
    restartRequired = false;
    settingsChanged = false;

    if (settingsEdit.bAutoSave != settings.bAutoSave) {
        restartRequired = true;
        return; // Return since no further checks are needed anyways
    }

    if (settingsEdit.bEnableLogging != settings.bEnableLogging) {
        settingsChanged = true;
    }

    if (settingsEdit.bConfirmDelete != settings.bConfirmDelete) {
        settingsChanged = true;
    }

    if (settingsEdit.bBackupOnExit != settings.bBackupOnExit) {
        settingsChanged = true;
    }

    if (settingsEdit.intLanguage != settings.intLanguage) {
        restartRequired = true;
        return;
    }

    if (settingsEdit.intCurrency != settings.intCurrency) {
        restartRequired = true;
        return;
    }

    if (settingsEdit.intDateFormat != settings.intDateFormat) {
        restartRequired = true;
        return;
    }

    if (settingsEdit.intMaxBackups != settings.intMaxBackups) {
        settingsChanged = true;
    }

    if (settingsEdit.strDataPath != settings.strDataPath) {
        restartRequired = true;
        return;
    }
}

void Settingswindow::on_TBTN_DIR_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(
        this,
        _("SW_DATA_TTL"),
        QDir::currentPath(),
        "JSON Files (*.json);;All Files (*.*)"
        );

    if (filePath.isEmpty()) return;

    if (manager->isValidDataFilePath(filePath) == false) {
        manager->critical(_("ERR_DATA_INVL"), false);
        return;
    }

    ui->LN_FLPT->setText(filePath);
}


void Settingswindow::on_CMBX_LNG_currentIndexChanged(int index)
{
    settingsEdit.intLanguage = index;
}


void Settingswindow::on_CMBX_CUR_currentIndexChanged(int index)
{
    settingsEdit.intCurrency = index;
}


void Settingswindow::on_CMBX_DATE_currentIndexChanged(int index)
{
    settingsEdit.intDateFormat = index;
}


void Settingswindow::on_CB_CD_checkStateChanged(const Qt::CheckState &arg1)
{
    settingsEdit.bConfirmDelete = arg1;
}


void Settingswindow::on_CB_EAS_checkStateChanged(const Qt::CheckState &arg1)
{
    settingsEdit.bAutoSave = arg1;
}


void Settingswindow::on_CB_BOAE_checkStateChanged(const Qt::CheckState &arg1)
{
    settingsEdit.bBackupOnExit = arg1;
}


void Settingswindow::on_SPBX_BU_valueChanged(int arg1)
{
    settingsEdit.intMaxBackups = arg1;
}


void Settingswindow::on_CB_EL_checkStateChanged(const Qt::CheckState &arg1)
{
    settingsEdit.bEnableLogging = arg1;
    manager->log("DEVTOOLS: Toggled logging");
}

void Settingswindow::on_LN_FLPT_textChanged(const QString &arg1)
{
    settingsEdit.strDataPath = arg1;
}

void Settingswindow::on_BTN_SAVE_clicked()
{
    checkVariables();

    if (restartRequired) {
        QMessageBox msgBox(this);
        msgBox.setWindowTitle(_("MB_SAVE_TTL"));
        msgBox.setText(_("MB_SAVE_CON"));
        msgBox.setIcon(QMessageBox::Question);

        QPushButton *yesButton = msgBox.addButton(_("BTN_YES"), QMessageBox::YesRole);
        QPushButton *noButton = msgBox.addButton(_("BTN_NO"), QMessageBox::NoRole);

        msgBox.exec();

        if (msgBox.clickedButton() == noButton) {
            return;
        }
    }

    saveButtonUsed = true;

    emit signal_updatedSettings(restartRequired);

    if (manager->saveSettings(settingsEdit, restartRequired)) {
        close();
    }
}


void Settingswindow::on_BTN_RES_clicked()
{
    settingsEdit.bAutoSave = true;
    settingsEdit.bEnableLogging = false;
    settingsEdit.bConfirmDelete = true;
    settingsEdit.bBackupOnExit = true;
    settingsEdit.intLanguage = 0;
    settingsEdit.intCurrency = 0;
    settingsEdit.intDateFormat = 0;
    settingsEdit.intMaxBackups = 3;
    settingsEdit.strDataPath = "data.json";

    ui->CB_BOAE->setChecked(settingsEdit.bBackupOnExit);
    ui->CB_CD->setChecked(settingsEdit.bConfirmDelete);
    ui->CB_EAS->setChecked(settingsEdit.bAutoSave);
    ui->CB_EL->setChecked(settingsEdit.bEnableLogging);
    ui->SPBX_BU->setValue(settingsEdit.intMaxBackups);
    ui->CMBX_CUR->setCurrentIndex(settingsEdit.intCurrency);
    ui->CMBX_DATE->setCurrentIndex(settingsEdit.intDateFormat);
    ui->CMBX_LNG->setCurrentIndex(settingsEdit.intLanguage);
    ui->LN_FLPT->setText(settingsEdit.strDataPath);

    restartRequired = true;
}


void Settingswindow::on_BTN_ER_clicked()
{
    QMessageBox msgBox(this);
    msgBox.setWindowTitle(_("MB_ER_TTL"));
    msgBox.setText(_("MB_ER_CON"));
    msgBox.setIcon(QMessageBox::Question);

    msgBox.addButton(_("BTN_YES"), QMessageBox::YesRole);
    QPushButton *noButton = msgBox.addButton(_("BTN_NO"), QMessageBox::NoRole);

    msgBox.exec();

    if (msgBox.clickedButton() == noButton) {
        return;
    }

    manager->clearData();
}

