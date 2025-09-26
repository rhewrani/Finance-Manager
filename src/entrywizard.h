#ifndef ENTRYWIZARD_H
#define ENTRYWIZARD_H

#include <QDialog>
#include <QDialogButtonBox>
#include <QEvent>
#include <QMouseEvent>

class Manager;

namespace Ui {
class EntryWizard;
}

class EntryWizard : public QDialog
{
    Q_OBJECT

public:
    explicit EntryWizard(Manager *managerRef, QWidget *parent = nullptr);
    ~EntryWizard();

    void ew_show(int id, int page = 1);

    bool eventFilter(QObject *watched, QEvent *event) override;

    int currentOpenedId = 0; // 0 = new
    int currentEditPage = 1;

signals:
    void signal_updateConfigwindow(int page);

private slots:
    void on_CMBX_ADD_currentIndexChanged(int index);

    void on_CMBX_MTH_currentIndexChanged(int index);

    void on_BTNBX_RES_clicked(QAbstractButton *button);

    void on_LN_AM_editingFinished();

    void on_BTN_DEL_clicked();

    void on_LN_DES_textEdited(const QString &arg1);

private:
    Ui::EntryWizard *ui;
    Manager *manager;

    bool dataChanged = false;

    bool LN_AM_VALID = true;
    bool notFirst = false;

    QString amountText = "";

    void Init();
    void InitUI();

    void ew_loadDataIntoFields(int id);
    void ew_clearDataFromFields();
    void ew_saveDataAdd();

    void ew_setAmValid(bool valid);
    void ew_checkSaveButton();

    bool ew_checkDiffAm();
};

#endif // ENTRYWIZARD_H
