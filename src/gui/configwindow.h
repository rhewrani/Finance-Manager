#ifndef CONFIGWINDOW_H
#define CONFIGWINDOW_H

#include <QDialog>
#include <qtablewidget.h>
#include "dialogs/entrywizard.h"

class Manager;

namespace Ui {
class Configwindow;
}

class Configwindow : public QDialog
{
    Q_OBJECT

public:
    explicit Configwindow(Manager *manager, QWidget *parent = nullptr);
    ~Configwindow();

    void closeEvent(QCloseEvent *event) override;

    bool reloadRequiredInc = true;
    bool reloadRequiredExp = true;

    void cw_show(int page);

    void cw_loadDataIncTable(bool resize = true);
    void cw_loadDataExpTable(bool resize = true);

signals:

    void signal_updateMainPage();

private slots:
    void on_BTN_PG_EXP_SWT_clicked();

    void on_BTN_PG_INC_SWT_clicked();

    void on_BTN_ADD_clicked();

    void on_BTN_EDIT_clicked();

    void on_PG_INC_TBL_itemClicked(QTableWidgetItem *item);

    void on_PG_EXP_TBL_itemClicked(QTableWidgetItem *item);

private:
    Ui::Configwindow *ui;
    Manager *manager;
    EntryWizard *entrywizard;


    int curPage = 1; // 1 = income, 2 = expenses


    void Init();
    void InitUI();

    int getIdFromSelectedRow();

    void cleanUpPage(int page);

    // Aber ganz wichtig: Im hauptfenster nach einer Änderung (Hinz., Del., Mod.) die Daten refreshen
    // Und schlau die incSum und expSum updaten. Nicht bei jeder Änderung die ganze Liste durchgehen um incSum/expSum zu erhalten sondern evtl. am bereits vorhandenen incSum/expSum (in manager) die Änderung hinzufügen (heißt einfach nur - oder + machen etc.)
};

#endif // CONFIGWINDOW_H
