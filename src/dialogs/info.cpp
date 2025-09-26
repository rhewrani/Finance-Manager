#include "info.h"
#include "ui_info.h"
#include "../core/manager.h"

Info::Info(Manager *managerRef, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Info)
    , manager(managerRef)
{
    ui->setupUi(this);

    InitUI();
}

Info::~Info()
{
    delete ui;
}

void Info::InitUI()
{
    setWindowTitle(_("INFO_TTL"));
    ui->LBL_TTL->setText(_("INFO_TTL2"));
    ui->LBL_CRED->setText(_("INFO_CRED"));

}
