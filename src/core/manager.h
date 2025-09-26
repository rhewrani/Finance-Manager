#ifndef MANAGER_H
#define MANAGER_H

#define _(key) (manager->t(key))

#include "fileagent.h"

#include <QObject>
#include <QApplication>
#include <QProcess>


class MainWindow;

class Manager : public QObject
{
    Q_OBJECT
public:
    explicit Manager(MainWindow *mainWindowRef, QObject *parent = nullptr);
    ~Manager();

    struct appSettings {
        bool bAutoSave = true;
        bool bDevtools = false; // add this
        bool bEnableLogging = false;
        bool bConfirmDelete = true;
        bool bBackupOnExit = true; // Remove this feature if it just creates too many issues
        int intLanguage = 0;
        int intCurrency = 0;
        int intDateFormat = 0;
        int intMaxBackups = 5;
        QString strVersion;
        QString strDataPath = "";
    };

    void debug(const QString &message);
    void log(const QString &message, bool MessageBox = false);
    void warning(const QString &message, bool MessageBox = true);
    void critical(const QString &message, bool MessageBox = true);

    QString t(const QString &key);

    void setDataIncome(QJsonArray arr);
    QMap<int, QJsonObject> getDataIncome();

    void setDataExpenses(QJsonArray arr);
    QMap<int, QJsonObject> getDataExpenses();

    QJsonObject getDataFromId(int page, int id);

    void saveDataEntry(QJsonObject entry, int page, int id);
    void deleteDataEntry(int page, int id);

    void setDataIncSum(int sum, int sumStrict);
    int getDataIncSum(int type = 0);

    void setDataExpSum(int sum, int sumStrict);
    int getDataExpSum(int type = 0);

    void setDataProfit(int sum);
    void recalcDataProfit();
    int getDataProfit(int type = 0);

    QString getDate() { return date; }


    appSettings& getSettingsStruct() { return settings; }

    bool getDataChanged() { return dataChanged; }
    void setDataChanged(bool state);

    void GetSettings();
    bool GetData(QString path);

    bool saveSettings(appSettings settings, bool restart);
    void setOTULanguage(int lang);
    void saveData(bool backUp = true); // writes all data back into the JSON
    void clearData();
    bool isValidDataFile(const QJsonObject &root);
    bool isValidDataFilePath(const QString path);
    bool isEmptyData(); // Checks whether the current data is empty

    void createBackup();
    void cleanupOldBackups(const QString &backupDir, int maxBackups);

signals:
    void signal_updateSaveAction(int state);
    void signal_updateDataCleared();

private:
    MainWindow *m_window;
    FileAgent *fileAgent;


    void Init();

    int generateUniqueId(int page);


    QMap<int, QJsonObject> incomeMap;
    QMap<int, QJsonObject> expensesMap;
    int incSum = 0;
    int expSum = 0;
    int profit = 0;
    int incSumStrict = 0;
    int expSumStrict = 0;
    int profitStrict = 0;
    QString date;
    bool dataChanged = false; // Is also manually set to true whenever there is a backup loaded in order to save the loaded data to data.json
    // Also add error catcher (e.g. it's the path is empty and no file could be openend) and general error handler for the entire app
    QJsonObject settingsRoot;
    appSettings settings;

};

#endif // MANAGER_H
