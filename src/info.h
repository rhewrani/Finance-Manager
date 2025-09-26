#ifndef INFO_H
#define INFO_H

#include <QDialog>

class Manager;

namespace Ui {
class Info;
}

class Info : public QDialog
{
    Q_OBJECT

public:
    explicit Info(Manager *managerRef, QWidget *parent = nullptr);
    ~Info();

private:
    Ui::Info *ui;
    Manager *manager;

    void InitUI();
};

#endif // INFO_H
