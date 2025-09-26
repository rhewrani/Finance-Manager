#include "entrywizard.h"
#include "ui_entrywizard.h"
#include "manager.h"
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <qpushbutton.h>

EntryWizard::EntryWizard(Manager *managerRef, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::EntryWizard)
    , manager(managerRef)
{
    ui->setupUi(this);

    this->installEventFilter(this);

    Init();
}

EntryWizard::~EntryWizard()
{
    delete ui;
}

bool EntryWizard::eventFilter(QObject *obj, QEvent *event)
{
    Q_UNUSED(obj)

    if (event->type() == QEvent::MouseButtonPress ||
        event->type() == QEvent::MouseButtonDblClick) {


        if (ui->LN_AM->hasFocus()) {
            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);

            if (!ui->LN_AM->geometry().contains(mouseEvent->pos())) {
                //onLineEditTextChanged(ui->LN_AM->text());
                ui->LN_AM->clearFocus();
            }
        }
    }

    return false;
}

void EntryWizard::Init() {

    ui->SPBX_MTH->setHidden(true);
    ui->LBL_ERR_TTL->setHidden(true);
    ui->LBL_ERR_ICR->setHidden(true);
    ui->LBL_ERR_EMT->setHidden(true);
    ui->BTN_DEL->setHidden(true);
    ui->LBL_EUR->setText(CurrencyConverter::instance()->currencySymbol(manager->getSettingsStruct().intCurrency));

    QRegularExpression rx("^(\\d{0,7}(,\\d{0,2})?|\\d{0,7},?)?$");
    ui->LN_AM->setValidator(new QRegularExpressionValidator(rx, this));

    InitUI();

    manager->log("Initialised entry wizard");

}

void EntryWizard::InitUI()
{
    setWindowTitle(_("EW_TTL"));
    ui->SPBX_MTH->setSuffix(_("EW_SPBX_SUF"));
    ui->CMBX_MTH->setItemText(0, _("EW_CMBX_MTH"));
    ui->CMBX_MTH->setItemText(1, _("EW_CMBX_QRT"));
    ui->CMBX_MTH->setItemText(2, _("EW_CMBX_YR"));
    ui->CMBX_MTH->setItemText(3, _("EW_CMBX_ONT"));
    ui->CMBX_MTH->setItemText(4, _("EW_CMBX_OTH"));
    ui->CMBX_ADD->setItemText(0, _("CW_INC"));
    ui->CMBX_ADD->setItemText(1, _("CW_EXP"));
    ui->LBL_ADD->setText(_("EW_ADD_TO"));
    ui->LBL_AM->setText(_("AM"));
    ui->LBL_DES->setText(_("DES"));
    ui->LBL_ITV->setText(_("ITV"));
    ui->LBL_ERR_TTL->setText(_("EW_ERR_TTL"));
    ui->LBL_ERR_ICR->setText(_("EW_ERR_ICR"));
    ui->LBL_ERR_EMT->setText(_("EW_ERR_EMT"));
    ui->BTNBX_RES->button(QDialogButtonBox::Cancel)->setText(_("BTN_CAN"));
    ui->BTNBX_RES->button(QDialogButtonBox::Reset)->setText(_("BTN_RES"));

}

void EntryWizard::ew_show(int id, int page)
{

    ui->CMBX_ADD->setCurrentIndex(page-1); // this also sets currentEditPage

    if (id == 0) { // id = 0 means add mode

        ui->CMBX_ADD->setDisabled(false);
        ui->BTN_DEL->setHidden(true);
        ui->LBL_ADD->setText(_("EW_ADD_TO"));
        ui->BTNBX_RES->button(QDialogButtonBox::Apply)->setText(_("BTN_ADD"));
        ui->BTNBX_RES->button(QDialogButtonBox::Apply)->setEnabled(false); //uncomment this later!!!!

        ew_clearDataFromFields();

    } else {

        ui->CMBX_ADD->setDisabled(true);
        ui->BTN_DEL->setHidden(false);
        ui->LBL_ADD->setText(_("EW_EDIT"));
        ui->BTNBX_RES->button(QDialogButtonBox::Apply)->setText(_("BTN_APP"));

        ew_loadDataIntoFields(id);

    }

    currentOpenedId = id;

    show();
}

void EntryWizard::ew_loadDataIntoFields(int id)
{

    QJsonObject obj = manager->getDataFromId(currentEditPage, id);

    double amount = obj.value("amount_eurocent").toInt() / 100.0;
    QString des = obj.value("description").toString();
    int interval = obj.value("interval").toInt();

    double converted = CurrencyConverter::instance()->convert(amount, 0, manager->getSettingsStruct().intCurrency);
    QString amountStr = QString::number(converted, 'f', 2).replace('.', ',');

    ui->SPBX_MTH->setValue(2);

    if (interval == 1) {
        ui->CMBX_MTH->setCurrentIndex(0);
    } else if (interval == 3) {
        ui->CMBX_MTH->setCurrentIndex(1);
    } else if (interval == 12) {
        ui->CMBX_MTH->setCurrentIndex(2);
    } else if (interval == 0) {
        ui->CMBX_MTH->setCurrentIndex(3);
    } else {
        ui->CMBX_MTH->setCurrentIndex(4);
        ui->SPBX_MTH->setValue(interval);
    }

    ui->LN_AM->setText(amountStr);
    ui->LN_DES->setText(des);

    amountText = amountStr;


}

void EntryWizard::ew_clearDataFromFields()
{
    ui->LN_AM->setText("0,00");
    ui->LN_DES->clear();
    ui->CMBX_MTH->setCurrentIndex(0);
    ui->SPBX_MTH->setValue(2);
    ui->LBL_ERR_TTL->setHidden(true);
    ui->LBL_ERR_EMT->setHidden(true);
    ui->LBL_ERR_ICR->setHidden(true);
}

void EntryWizard::ew_saveDataAdd()
{
    QJsonObject entry;

    int amountEurocent = stringToEurocent(ui->LN_AM->text());
    double amountEuro = amountEurocent / 100.0;
    int converted = qRound(CurrencyConverter::instance()->convert(amountEuro, manager->getSettingsStruct().intCurrency, 0)*100.0);
    int interval = 1;

    if (ui->CMBX_MTH->currentIndex() == 1) {
        interval = 3;
    } else if (ui->CMBX_MTH->currentIndex() == 2) {
        interval = 12;
    } else if (ui->CMBX_MTH->currentIndex() == 3) {
        interval = 0;
    } else if (ui->CMBX_MTH->currentIndex() == 4) {
        interval = ui->SPBX_MTH->value();
    }
    entry["amount_eurocent"] = converted;
    entry["description"] = ui->LN_DES->text();
    entry["interval"] = interval;

    manager->saveDataEntry(entry, currentEditPage, currentOpenedId);
    emit signal_updateConfigwindow(currentEditPage);

    close();


}

void EntryWizard::ew_setAmValid(bool valid)
{

    ui->LBL_ERR_TTL->setHidden(valid);
    if (valid) {
        ui->LBL_ERR_EMT->setHidden(true);
        ui->LBL_ERR_ICR->setHidden(true);
    }
    LN_AM_VALID = valid;
    ew_checkSaveButton();
    /*
    if (notFirst) {
    }
    notFirst = true;
    */
}

void EntryWizard::ew_checkSaveButton()
{

    if (LN_AM_VALID) {
        ui->BTNBX_RES->button(QDialogButtonBox::Apply)->setEnabled(true);
        return;
    }

    ui->BTNBX_RES->button(QDialogButtonBox::Apply)->setEnabled(false);

}

bool EntryWizard::ew_checkDiffAm()
{
    if (amountText == ui->LN_AM->text()) {
        return true;
    }

    return false;
}

void EntryWizard::on_CMBX_ADD_currentIndexChanged(int index)
{

    // This function is also called when the index is changed manually in the code
    currentEditPage = index+1;
}

void EntryWizard::on_CMBX_MTH_currentIndexChanged(int index)
{
    if (index == 4) {
        ui->SPBX_MTH->setHidden(false);
    } else {
        ui->SPBX_MTH->setHidden(true);
    }
}


void EntryWizard::on_BTNBX_RES_clicked(QAbstractButton *button)
{
    QDialogButtonBox::StandardButton stdBtn = ui->BTNBX_RES->standardButton(button);

    switch (stdBtn) {
    case QDialogButtonBox::Reset:
        ew_clearDataFromFields();
        break;

    case QDialogButtonBox::Apply:
        ew_saveDataAdd();

        break;

    case QDialogButtonBox::Cancel:
        reject();
        break;

    default:
        manager->log("Unknown Button pressed:" + button->text());
        break;
    }
}


void EntryWizard::on_LN_AM_editingFinished()
{

    ew_checkDiffAm();

    QString text = ui->LN_AM->text();

    if (text.isEmpty()) {

        ui->LBL_ERR_TTL->setHidden(false);
        ui->LBL_ERR_EMT->setHidden(false);
        ew_setAmValid(false);
        return;
    }

    ui->LBL_ERR_EMT->setHidden(true);
    bool valid = isValidEuroAmount(text);

    if (!valid) {
        ui->LBL_ERR_ICR->setHidden(false);
    }

    ew_setAmValid(valid);


}


void EntryWizard::on_BTN_DEL_clicked()
{
    if (manager->getSettingsStruct().bConfirmDelete) {

        QMessageBox msgBox(this);
        msgBox.setWindowTitle(_("MB_DEL_TTL"));
        msgBox.setText(_("MB_DEL_CON"));
        msgBox.setIcon(QMessageBox::Question);

        QPushButton *yesButton = msgBox.addButton(_("BTN_YES"), QMessageBox::YesRole);
        QPushButton *noButton = msgBox.addButton(_("BTN_NO"), QMessageBox::NoRole);

        msgBox.exec();

        if (msgBox.clickedButton() == noButton) {
            return;

        }
    }

    manager->deleteDataEntry(currentEditPage, currentOpenedId);
    emit signal_updateConfigwindow(currentEditPage);

    close();
}


void EntryWizard::on_LN_DES_textEdited(const QString &arg1)
{
    //ew_checkSaveButton();
}


