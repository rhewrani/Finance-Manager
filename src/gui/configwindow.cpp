#include "configwindow.h"
#include "ui_configwindow.h"
#include "core/manager.h"
#include <QTimer>

Configwindow::Configwindow(Manager *managerRef, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Configwindow)
    , manager(managerRef)
{
    ui->setupUi(this);

    entrywizard = new EntryWizard(manager, this);

    Init();

}

Configwindow::~Configwindow()
{
    delete ui;
}

void Configwindow::closeEvent(QCloseEvent *event)
{
    if (entrywizard->isVisible()) {
        entrywizard->close();
    }
    emit signal_updateMainPage();
    event->accept();
}

void Configwindow::Init()
{
    connect(entrywizard, &EntryWizard::signal_updateConfigwindow, this, [this](int page) {

        if (page == 1) {
            reloadRequiredInc = true;
            cw_loadDataIncTable();
        } else if (page == 2) {
            reloadRequiredExp = true;
            cw_loadDataExpTable();
        }

        cleanUpPage(page);

        if (curPage != page) {
            if (page == 1) {
                ui->STK_WGT->setCurrentWidget(ui->PG_INC);
            } else {
                ui->STK_WGT->setCurrentWidget(ui->PG_EXP);
            }

            curPage = page;
        }

        raise();
        activateWindow();


    });

    ui->BTN_EDIT->setDisabled(true);

    ui->PG_INC_TBL->clearSelection();
    ui->PG_INC_TBL->setCurrentCell(-1, -1);
    ui->PG_EXP_TBL->clearSelection();
    ui->PG_EXP_TBL->setCurrentCell(-1, -1);

    InitUI();

    manager->log("Initialised config window");
}

void Configwindow::InitUI()
{
    setWindowTitle(_("CW_TTL"));
    ui->BTN_PG_INC_SWT->setText(_("CW_EXP"));
    ui->BTN_PG_EXP_SWT->setText(_("CW_INC"));
    ui->LBL_DAT_TTL->setText(_("CW_LAST_SAVE"));
    ui->LBL_PG_INC->setText(_("TTL_INC"));
    ui->LBL_PG_EXP->setText(_("TTL_EXP"));
    ui->PG_INC_TBL->horizontalHeaderItem(0)->setText(_("AM"));
    ui->PG_INC_TBL->horizontalHeaderItem(1)->setText(_("ITV"));
    ui->PG_INC_TBL->horizontalHeaderItem(2)->setText(_("DES"));
    ui->PG_EXP_TBL->horizontalHeaderItem(0)->setText(_("AM"));
    ui->PG_EXP_TBL->horizontalHeaderItem(1)->setText(_("ITV"));
    ui->PG_EXP_TBL->horizontalHeaderItem(2)->setText(_("DES"));
}

void Configwindow::cw_show(int page)
{
    if (page == 1) {
        cw_loadDataIncTable();
        ui->STK_WGT->setCurrentWidget(ui->PG_INC);
    } else {
        cw_loadDataExpTable();
        ui->STK_WGT->setCurrentWidget(ui->PG_EXP);
    }

    cleanUpPage(page);
    curPage = page;
    QString date = manager->getDate();
    ui->LBL_DAT->setText(date);
    ui->LBL_DAT->setHidden(date.isEmpty());
    ui->LBL_DAT_TTL->setHidden(date.isEmpty());
    show();
    //raise();
    //activateWindow();

}

void Configwindow::cw_loadDataIncTable(bool resize)
{
    if (!reloadRequiredInc) return;

    ui->PG_INC_TBL->setRowCount(0);

    auto in = manager->getDataIncome();

    for (auto it = in.begin(); it != in.end(); ++it) {
        int id = it.key();
        QJsonObject obj = it.value();

        int amount = obj.value("amount_eurocent").toInt();
        QString des = obj.value("description").toString();
        int interval = obj.value("interval").toInt();

        double amountEuro = amount / 100.0;
        double converted = CurrencyConverter::instance()->convert(amountEuro, 0, manager->getSettingsStruct().intCurrency);
        QString amountStr = QString::number(converted, 'f', 2).replace('.', ',') + " " + CurrencyConverter::instance()->currencySymbol(manager->getSettingsStruct().intCurrency);

        QString intervalStr;
        if (interval == 1) {
            intervalStr = _("EW_CMBX_MTH");
        } else if (interval == 3) {
            intervalStr = _("EW_CMBX_QRT");
        } else if (interval == 12) {
            intervalStr = _("EW_CMBX_YR");
        } else {
            intervalStr = _("EW_CMBX_OTH_TEXT").arg(QString::number(interval));
        }

        int row = ui->PG_INC_TBL->rowCount();
        ui->PG_INC_TBL->insertRow(row);

        ui->PG_INC_TBL->setItem(row, 0, new QTableWidgetItem(amountStr));
        ui->PG_INC_TBL->setItem(row, 1, new QTableWidgetItem(intervalStr));
        ui->PG_INC_TBL->setItem(row, 2, new QTableWidgetItem(des));

        QTableWidgetItem *itemWithId = ui->PG_INC_TBL->item(row, 0);
        ui->PG_INC_TBL->item(row, 2)->setToolTip(des);
        itemWithId->setData(Qt::UserRole, id);
    }

    if (resize) {
        ui->PG_INC_TBL->resizeColumnsToContents();  // Passt an Inhalt an
        ui->PG_INC_TBL->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    }
    reloadRequiredInc = false;
}

void Configwindow::cw_loadDataExpTable(bool resize)
{
        if (!reloadRequiredExp) return;

        ui->PG_EXP_TBL->setRowCount(0);

        auto ex = manager->getDataExpenses();

        for (auto it = ex.begin(); it != ex.end(); ++it) {
            int id = it.key();
            QJsonObject obj = it.value();

            int amount = obj.value("amount_eurocent").toInt();
            QString des = obj.value("description").toString();
            int interval = obj.value("interval").toInt();

            double amountEuro = amount / 100.0;
            double converted = CurrencyConverter::instance()->convert(amountEuro, 0, manager->getSettingsStruct().intCurrency);
            QString amountStr = QString::number(converted, 'f', 2).replace('.', ',') + " " + CurrencyConverter::instance()->currencySymbol(manager->getSettingsStruct().intCurrency);

            QString intervalStr;
            if (interval == 1) {
                intervalStr = _("EW_CMBX_MTH");
            } else if (interval == 3) {
                intervalStr = _("EW_CMBX_QRT");
            } else if (interval == 12) {
                intervalStr = _("EW_CMBX_YR");
            } else {
                intervalStr = _("EW_CMBX_OTH_TEXT").arg(QString::number(interval));
            }

            int row = ui->PG_EXP_TBL->rowCount();
            ui->PG_EXP_TBL->insertRow(row);

            ui->PG_EXP_TBL->setItem(row, 0, new QTableWidgetItem(amountStr));
            ui->PG_EXP_TBL->setItem(row, 1, new QTableWidgetItem(intervalStr));
            ui->PG_EXP_TBL->setItem(row, 2, new QTableWidgetItem(des));

            QTableWidgetItem *itemWithId = ui->PG_EXP_TBL->item(row, 0);
            ui->PG_EXP_TBL->item(row, 2)->setToolTip(des);
            itemWithId->setData(Qt::UserRole, id);
        }

        if (resize) {
            ui->PG_EXP_TBL->resizeColumnsToContents();
            ui->PG_EXP_TBL->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
        }
        reloadRequiredExp = false;

}

void Configwindow::on_BTN_PG_EXP_SWT_clicked()
{

    ui->STK_WGT->setCurrentWidget(ui->PG_INC);

    cleanUpPage(1);

    curPage = 1;
    cw_loadDataIncTable();


}


void Configwindow::on_BTN_PG_INC_SWT_clicked()
{

    ui->STK_WGT->setCurrentWidget(ui->PG_EXP);

    cleanUpPage(2);

    curPage = 2;
    cw_loadDataExpTable();

}


void Configwindow::on_BTN_ADD_clicked()
{
    if (entrywizard->isVisible()) {
        entrywizard->raise();
        entrywizard->activateWindow();
    } else {
        entrywizard->ew_show(0, curPage);
    }
}


void Configwindow::on_BTN_EDIT_clicked()
{
    if (entrywizard->isVisible()) {
        entrywizard->raise();
        entrywizard->activateWindow();
    } else {

        int id = getIdFromSelectedRow();
        entrywizard->ew_show(id, curPage);
    }
}


int Configwindow::getIdFromSelectedRow()
{
    QTableWidget *tbl = ui->PG_INC_TBL;

    if (curPage == 2) {
        tbl = ui->PG_EXP_TBL;
    }

    QList<QTableWidgetItem*> selected = tbl->selectedItems();
    if (selected.isEmpty()) {
        qWarning() << "No cell selected!";
        return -1; // No cell selected or invalid value
    }

    QTableWidgetItem *firstItem = selected.first();

    return firstItem->data(Qt::UserRole).toInt();

}

void Configwindow::cleanUpPage(int page)
{
    if (curPage == 1) {

        ui->BTN_EDIT->setDisabled(true);
        ui->PG_INC_TBL->clearSelection();
        ui->PG_INC_TBL->setCurrentCell(-1, -1);

        return;
    }

    ui->BTN_EDIT->setDisabled(true);
    ui->PG_EXP_TBL->clearSelection();
    ui->PG_EXP_TBL->setCurrentCell(-1, -1);
}


void Configwindow::on_PG_INC_TBL_itemClicked(QTableWidgetItem *item)
{
    ui->BTN_EDIT->setDisabled(false);
}


void Configwindow::on_PG_EXP_TBL_itemClicked(QTableWidgetItem *item)
{
    ui->BTN_EDIT->setDisabled(false);
}

