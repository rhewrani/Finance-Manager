#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    manager = new Manager(this, this);
    configwindow = new Configwindow(manager, this);
    settingswindow = new Settingswindow(manager, this);
    info = new Info(manager, this);

    Logger::instance()->setParentWidget(this, manager->getSettingsStruct().intDateFormat);

    Init();

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (manager->getSettingsStruct().bAutoSave && !messageBoxAfterSaveRestartRequired) {
        //manager->saveData(manager->getSettingsStruct().bBackupOnExit); //no need to enable this since it's already done in manager class deconstructor; this would do it twice. Enable again if keeping it disabled causes problems
        event->accept();
    } else if (manager->getDataChanged()){
        QString str = _("MB_CLOS_CON2");
        if (!manager->getDate().isEmpty()) {
            str = _("MB_CLOS_CON").arg(manager->getDate());
        }

        QMessageBox msgBox(this);
        msgBox.setWindowTitle(_("MB_CLOS_TTL"));
        msgBox.setText(str);
        msgBox.setIcon(QMessageBox::Question);

        QPushButton *saveButton = msgBox.addButton(_("BTN_SAVE"), QMessageBox::AcceptRole);
        QPushButton *discardButton = msgBox.addButton(_("BTN_DIS"), QMessageBox::DestructiveRole);
        if (!messageBoxAfterSaveRestartRequired) {
            msgBox.addButton(_("BTN_CAN"), QMessageBox::RejectRole);
        }

        msgBox.exec();

        if (msgBox.clickedButton() == saveButton) {
            manager->saveData(manager->getSettingsStruct().bBackupOnExit);
            qDebug() << "Data changed state: " << manager->getDataChanged();
            event->accept();
        } else if (msgBox.clickedButton() == discardButton) {
            event->accept();
        } else {
            event->ignore();
        }
    }

}

void MainWindow::MainPage_updateDataFrontendAll(const QMap<int, QJsonObject> &in, const QMap<int, QJsonObject> &ex, bool updatePr)
{
    MainPage_updateDataFrontendIncome(in);
    MainPage_updateDataFrontendExpenses(ex);

    if (updatePr) {
        manager->recalcDataProfit();
        MainPage_updateDataFrontendProfit(manager->getDataProfit(showSumType));
    }

}

void MainWindow::MainPage_updateDataFrontendIncome(const QMap<int, QJsonObject> &in, bool resize)
{
    ui->TBL_INC->setRowCount(0);

    int count = 0;
    int countStrict = 0;

    for (auto it = in.begin(); it != in.end(); ++it) {
        int id = it.key();
        QJsonObject obj = it.value();

        int amount = obj.value("amount_eurocent").toInt();
        QString des = obj.value("description").toString();
        int interval = obj.value("interval").toInt();

        double amountEuro = amount / 100.0;
        double converted = CurrencyConverter::instance()->convert(amountEuro, 0, manager->getSettingsStruct().intCurrency);
        QString amountStr = QString::number(converted, 'f', 2).replace('.', ',') + " " + CurrencyConverter::instance()->currencySymbol(manager->getSettingsStruct().intCurrency);

        count += amount;

        QString intervalStr;
        if (interval == 1) {
            intervalStr = _("EW_CMBX_MTH");
            countStrict += amount;
        } else if (interval == 3) {
            intervalStr = _("EW_CMBX_QRT");
        } else if (interval == 12) {
            intervalStr = _("EW_CMBX_YR");
        } else {
            intervalStr = _("EW_CMBX_OTH_TEXT").arg(QString::number(interval));
        }

        int row = ui->TBL_INC->rowCount();
        ui->TBL_INC->insertRow(row);

        ui->TBL_INC->setItem(row, 0, new QTableWidgetItem(amountStr));
        ui->TBL_INC->setItem(row, 1, new QTableWidgetItem(intervalStr));
        ui->TBL_INC->setItem(row, 2, new QTableWidgetItem(des));

        QTableWidgetItem *itemWithId = ui->TBL_INC->item(row, 0);
        ui->TBL_INC->item(row, 2)->setToolTip(des);
        itemWithId->setData(Qt::UserRole, id);
    }

    if (resize) {
        ui->TBL_INC->resizeColumnsToContents();
        ui->TBL_INC->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    }
    manager->setDataIncSum(count, countStrict);
    if (showSumType == 0) {
        MainPage_updateDataFrontendIncSum(count);
    } else {
        MainPage_updateDataFrontendIncSum(countStrict);
    }

}

void MainWindow::MainPage_updateDataFrontendExpenses(const QMap<int, QJsonObject> &ex, bool resize)
{
    ui->TBL_EXP->setRowCount(0);

    int count = 0;
    int countStrict = 0;

    for (auto it = ex.begin(); it != ex.end(); ++it) {
        int id = it.key();
        QJsonObject obj = it.value();

        int amount = obj.value("amount_eurocent").toInt();
        QString des = obj.value("description").toString();
        int interval = obj.value("interval").toInt();

        double amountEuro = amount / 100.0;
        double converted = CurrencyConverter::instance()->convert(amountEuro, 0, manager->getSettingsStruct().intCurrency);
        QString amountStr = QString::number(converted, 'f', 2).replace('.', ',') + " " + CurrencyConverter::instance()->currencySymbol(manager->getSettingsStruct().intCurrency);

        count += amount;

        QString intervalStr;
        if (interval == 1) {
            intervalStr = _("EW_CMBX_MTH");
            countStrict += amount;
        } else if (interval == 3) {
            intervalStr = _("EW_CMBX_QRT");
        } else if (interval == 12) {
            intervalStr = _("EW_CMBX_YR");
        } else {
            intervalStr = _("EW_CMBX_OTH_TEXT").arg(QString::number(interval));
        }

        int row = ui->TBL_EXP->rowCount();
        ui->TBL_EXP->insertRow(row);

        ui->TBL_EXP->setItem(row, 0, new QTableWidgetItem(amountStr));
        ui->TBL_EXP->setItem(row, 1, new QTableWidgetItem(intervalStr));
        ui->TBL_EXP->setItem(row, 2, new QTableWidgetItem(des));

        QTableWidgetItem *itemWithId = ui->TBL_EXP->item(row, 0);
        ui->TBL_EXP->item(row, 2)->setToolTip(des);
        itemWithId->setData(Qt::UserRole, id);
    }

    if (resize) {
        ui->TBL_EXP->resizeColumnsToContents();
        ui->TBL_EXP->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    }
        manager->setDataExpSum(count, countStrict);
    if (showSumType == 0) {
        MainPage_updateDataFrontendExpSum(count);
    } else {
        MainPage_updateDataFrontendExpSum(countStrict);
    }
}

void MainWindow::MainPage_updateDataFrontendProfit(int sum)
{
    double euro = sum / 100.0;
    double converted = CurrencyConverter::instance()->convert(euro, 0, manager->getSettingsStruct().intCurrency);
    QString text = QString::number(converted, 'f', 2).replace('.', ',') + " " + CurrencyConverter::instance()->currencySymbol(manager->getSettingsStruct().intCurrency);

    ui->LN_RES->setText(text);
}

void MainWindow::MainPage_updateDataFrontendIncSum(int incSum)
{

    double euro = incSum / 100.0;
    double converted = CurrencyConverter::instance()->convert(euro, 0, manager->getSettingsStruct().intCurrency);
    QString text = QString::number(converted, 'f', 2).replace('.', ',') + " " + CurrencyConverter::instance()->currencySymbol(manager->getSettingsStruct().intCurrency);

    ui->LN_RES_INC->setText(text);
}

void MainWindow::MainPage_updateDataFrontendExpSum(int expSum)
{

    double euro = expSum / 100.0;
    double converted = CurrencyConverter::instance()->convert(euro, 0, manager->getSettingsStruct().intCurrency);
    QString text = QString::number(converted, 'f', 2).replace('.', ',') + " " + CurrencyConverter::instance()->currencySymbol(manager->getSettingsStruct().intCurrency);

    ui->LN_RES_EXP->setText(text);
}

void MainWindow::Init()
{
    connect(qApp, &QApplication::aboutToQuit, this, [this]() {
        manager->log("\n=== Application stopped: "
        + QDateTime::currentDateTime().toString(getDateFormat(manager->getSettingsStruct().intDateFormat))
        + " ===\n");

    });

    connect(configwindow, &Configwindow::signal_updateMainPage, this, [this]() {
        show();
        if (manager->getSettingsStruct().bAutoSave) {
            manager->saveData();
        }
        MainPage_updateDataFrontendAll(manager->getDataIncome(), manager->getDataExpenses(), true);
    });

    connect(settingswindow, &Settingswindow::signal_updatedSettings, this, [this](bool state) {

        messageBoxAfterSaveRestartRequired = state;
    });

    connect(manager, &Manager::signal_updateSaveAction, this, [this](bool state) {

        ui->actionSave_all->setEnabled(state);
    });

    connect(manager, &Manager::signal_updateDataCleared, this, [this]() {

        MainPage_updateDataFrontendAll(manager->getDataIncome(), manager->getDataExpenses(), true);
        configwindow->reloadRequiredInc = true;
        configwindow->reloadRequiredExp = true;

    });

    ui->actionSave_all->setEnabled(false);
    MainPage_updateDataFrontendAll(manager->getDataIncome(), manager->getDataExpenses(), manager->getDataProfit());
    InitUI();

    manager->log("Initialised main window");
}

void MainWindow::InitUI()
{
    ui->LBL_INC->setText(_("TTL_INC"));
    ui->LBL_EXP->setText(_("TTL_EXP"));
    ui->LBL_RES->setText(_("PRF_SUM"));
    ui->LBL_RES_INC->setText(_("INC_SUM"));
    ui->LBL_RES_EXP->setText(_("EXP_SUM"));
    ui->CB_MTH->setText(_("CB_MTH"));
    ui->TBL_INC->horizontalHeaderItem(0)->setText(_("AM"));
    ui->TBL_INC->horizontalHeaderItem(1)->setText(_("ITV"));
    ui->TBL_INC->horizontalHeaderItem(2)->setText(_("DES"));
    ui->TBL_EXP->horizontalHeaderItem(0)->setText(_("AM"));
    ui->TBL_EXP->horizontalHeaderItem(1)->setText(_("ITV"));
    ui->TBL_EXP->horizontalHeaderItem(2)->setText(_("DES"));
    ui->menuSettings->setTitle(_("MENU_MENU"));
    ui->menuAbout->setTitle(_("MENU_ABOUT"));
    ui->menuData->setTitle(_("MENU_DATA"));
    ui->actionInfo->setText(_("ACT_ABOUT"));
    ui->actionLoad_backup->setText(_("ACT_LOAD_BU"));
    ui->actionOpen_settings->setText(_("ACT_OPEN_SET"));
    ui->actionOpen_configuration->setText(_("ACT_OPEN_CON"));
    ui->actionSave_all->setText(_("ACT_SAVE_ALL"));
}




void MainWindow::on_BTN_OPEN_INC_clicked()
{
    if (configwindow->isVisible()) {
        configwindow->raise();
        configwindow->activateWindow();
    } else {
        configwindow->cw_show(1);
    }
}


void MainWindow::on_BTN_OPEN_EXP_clicked()
{
    if (configwindow->isVisible()) {
        configwindow->raise();
        configwindow->activateWindow();
    } else {
        configwindow->cw_show(2);
    }
}


void MainWindow::on_actionOpen_configuration_triggered()
{
    if (configwindow->isVisible()) {
        configwindow->raise();
        configwindow->activateWindow();
    } else {
        configwindow->cw_show(1);
    }
}

void MainWindow::on_CB_MTH_checkStateChanged(const Qt::CheckState &arg1)
{
    showSumType = arg1;
    MainPage_updateDataFrontendIncSum(manager->getDataIncSum(arg1));
    MainPage_updateDataFrontendExpSum(manager->getDataExpSum(arg1));
    MainPage_updateDataFrontendProfit(manager->getDataProfit(arg1));
}


void MainWindow::on_actionInfo_triggered()
{
    if (info->isVisible()) {
        info->raise();
        info->activateWindow();
    } else {
        info->show();
    }
}


void MainWindow::on_actionOpen_settings_triggered()
{
    if (settingswindow->isVisible()) {
        settingswindow->raise();
        settingswindow->activateWindow();
    } else {
        settingswindow->sw_show();
    }
}


void MainWindow::on_actionSave_all_triggered()
{
    manager->saveData();
}


void MainWindow::on_actionLoad_backup_triggered()
{
    QString filePath = QFileDialog::getOpenFileName(
        this,
        _("ACT_LOAD_BU"),
        QDir::currentPath() + "/backups",
        "JSON Files (*.json);;All Files (*.*)"
        );

    if (filePath.isEmpty()) return;

    if(manager->isValidDataFilePath(filePath)) { // pretty much checks is file is valid and if yes, fills data into backend variables
        QMessageBox msgBox(this);
        msgBox.setWindowTitle(_("MB_BU_TTL"));
        QString str = _("MB_BU_CON2");
        if (!manager->getDate().isEmpty()) {
            str = _("MB_BU_CON").arg(manager->getDate());
        }
        msgBox.setText(str);
        msgBox.setIcon(QMessageBox::Question);

        QPushButton *yesButton = msgBox.addButton(_("BTN_YES"), QMessageBox::YesRole);
        QPushButton *noButton = msgBox.addButton(_("BTN_NO"), QMessageBox::NoRole);

        msgBox.exec();

        if (msgBox.clickedButton() == noButton) {
            return;
        }

        manager->GetData(filePath);
        manager->setDataChanged(true); // Allows the user to not necessarily overwrite the last save/allows them to just look what the old backup looked like without making them lose their saved data
        //manager->saveData(false); // If I want the program to already save the loaded data from the backup file to data.json

        MainPage_updateDataFrontendAll(manager->getDataIncome(), manager->getDataExpenses(), manager->getDataProfit());
        configwindow->reloadRequiredInc = true;
        configwindow->reloadRequiredExp = true;
    }


}

