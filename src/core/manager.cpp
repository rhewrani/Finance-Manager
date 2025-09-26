#include "gui/mainwindow.h"

Manager::Manager(MainWindow *mainWindowRef, QObject *parent)
    : QObject{parent}
    , m_window(mainWindowRef)
{
    fileAgent = new FileAgent();

    Init();

}

Manager::~Manager()
{

    if (settings.bAutoSave && dataChanged == true) {
        qDebug() << "Manager save";
        saveData(settings.bBackupOnExit);
    }
}

void Manager::debug(const QString &message)
{
    qDebug() << message;
    Logger::instance()->debug(message);
}

void Manager::log(const QString &message, bool MessageBox)
{
    if (settings.bEnableLogging) {
        Logger::instance()->log(message, MessageBox);
    }
}

void Manager::warning(const QString &message, bool MessageBox)
{
    Logger::instance()->warning(message, MessageBox);
}

void Manager::critical(const QString &message, bool MessageBox)
{
    Logger::instance()->critical(message, MessageBox);
}

QString Manager::t(const QString &key)
{

    return translate(key, settings.intLanguage);
}

void Manager::setDataIncome(QJsonArray arr)
{
    incomeMap.clear();

    for (const QJsonValue &v : arr) {
        QJsonObject obj = v.toObject();
        int id = obj.value("id").toInt();
        incomeMap[id] = obj;
    }
}

void Manager::setDataExpenses(QJsonArray arr)
{
    expensesMap.clear();

    for (const QJsonValue &v : arr) {
        QJsonObject obj = v.toObject();
        int id = obj.value("id").toInt();
        expensesMap[id] = obj;
    }
}

void Manager::setDataProfit(int sum)
{
    profit = sum;
}

void Manager::recalcDataProfit()
{
    profit = incSum - expSum;
    profitStrict = incSumStrict - expSumStrict;
}

QMap<int, QJsonObject> Manager::getDataIncome() {

    return incomeMap;
}

QMap<int, QJsonObject> Manager::getDataExpenses() {

    return expensesMap;
}

QJsonObject Manager::getDataFromId(int page, int id)
{
    if (page == 1) return incomeMap[id];
    return expensesMap[id];
}

void Manager::saveDataEntry(QJsonObject entry, int page, int id)
{
    QMap<int, QJsonObject> &targetMap = (page == 2) ? expensesMap : incomeMap;

    if (id == 0) id = generateUniqueId(page);
    entry["id"] = id;

    targetMap[id] = entry;
    setDataChanged(true);


    log("Saved data entry with id " + QString::number(id));
}

void Manager::deleteDataEntry(int page, int id)
{
    QMap<int, QJsonObject> &targetMap = (page == 2) ? expensesMap : incomeMap;

    if (targetMap.contains(id)) {
        targetMap.remove(id);
        setDataChanged(true);

        log("Removed data entry with id " + QString::number(id));
    }
}

void Manager::setDataIncSum(int sum, int sumStrict)
{
    incSum = sum;
    incSumStrict = sumStrict;
}

void Manager::setDataExpSum(int sum, int sumStrict)
{
    expSum = sum;
    expSumStrict = sumStrict;
}

int Manager::getDataIncSum(int type)
{
    if (type) {
        return incSumStrict;
    }
    return incSum;
}

int Manager::getDataExpSum(int type)
{
    if (type) {
        return expSumStrict;
    }
    return expSum;
}

int Manager::getDataProfit(int type) {

    if (type) {
        return profitStrict;
    }
    return profit;
}

/*
int Manager::getDataCurrency()
{
    return currency;
}

void Manager::setDataCurrency(int cur)
{
    currency = cur;
}

void Manager::checkCurrency(bool backup)
{
    if (currency == settings.intCurrency) return;

    int oldCurrency = currency;
    int newCurrency = settings.intCurrency;

    double rate = CurrencyConverter::instance()->convert(1.0, oldCurrency, newCurrency);


    for (auto it = incomeMap.begin(); it != incomeMap.end(); ++it) {
        QJsonObject obj = it.value();
        int oldAmount = obj.value("amount_eurocent").toInt();
        int newAmount = static_cast<int>(oldAmount * rate);
        obj["amount_eurocent"] = newAmount;
        incomeMap[it.key()] = obj;
    }


    for (auto it = expensesMap.begin(); it != expensesMap.end(); ++it) {
        QJsonObject obj = it.value();
        int oldAmount = obj.value("amount_eurocent").toInt();
        int newAmount = static_cast<int>(oldAmount * rate);
        obj["amount_eurocent"] = newAmount;
        expensesMap[it.key()] = obj;
    }


    incSum = static_cast<int>(incSum * rate);
    expSum = static_cast<int>(expSum * rate);
    incSumStrict = static_cast<int>(incSum * rate);
    expSumStrict = static_cast<int>(expSum * rate);
    recalcDataProfit();

    setDataCurrency(settings.intCurrency);

    dataChanged = true;
    if (!backup) {
        saveData(); // Intentionally no backup parameter passed since the parameter in this function (checkCurrency) is only to prevent the program from automatically saving changes when a backup is loaded
    }
    log(QString("Converted all data from %1 to %2 (rate: %3)")
            .arg(CurrencyConverter::instance()->currencyName(oldCurrency))
            .arg(CurrencyConverter::instance()->currencyName(newCurrency))
            .arg(rate));
}
*/

void Manager::setDataChanged(bool state) {
    dataChanged = state;
    emit signal_updateSaveAction(state);
}

void Manager::saveData(bool backUp)
{

    if (dataChanged == false) return;

    if (isValidDataFilePath(settings.strDataPath) == false) {
        critical(t("ERR_DATA_INVL"));
        return;
    }

    if (backUp) {
        createBackup();
    }

    QJsonArray incArr;
    for (auto it = incomeMap.begin(); it != incomeMap.end(); ++it) {
        QJsonObject obj = it.value();
        obj["id"] = it.key();
        incArr.append(obj);
    }

    QJsonArray expArr;
    for (auto it = expensesMap.begin(); it != expensesMap.end(); ++it) {
        QJsonObject obj = it.value();
        obj["id"] = it.key();
        expArr.append(obj);
    }

    setDataProfit((incSum-expSum));

    QFile file(settings.strDataPath);

    QJsonDocument doc = fileAgent->File_GetDataDocument(file);
    QJsonObject root = doc.object();

    date = QDateTime::currentDateTime().toString(getDateFormat(settings.intDateFormat));
    root["income"] = incArr;
    root["expenses"] = expArr;
    root["totalSum"] = profit;
    root["incSum"] = incSum;
    root["incSumStrict"] = incSumStrict;
    root["expSum"] = expSum;
    root["expSumStrict"] = expSumStrict;
    root["date"] = date;

    doc.setObject(root);

    fileAgent->File_Open(file, 1);
    file.write(doc.toJson(QJsonDocument::JsonFormat::Indented));
    setDataChanged(false);
    file.close();

    log("Saved data to file " + settings.strDataPath);

}

void Manager::clearData()
{
    incomeMap.clear();
    expensesMap.clear();
    incSum = 0;
    expSum = 0;
    profit = 0;
    incSumStrict = 0;
    expSumStrict = 0;
    profitStrict = 0;
    setDataChanged(true);
    saveData();
    emit signal_updateDataCleared();
}

bool Manager::isValidDataFile(const QJsonObject &root)
{
    QStringList requiredFields = {
        "income", "expenses", "totalSum", "incSum", "expSum", "incSumStrict", "expSumStrict", "date"
    };

    for (const QString &field : requiredFields) {
        if (!root.contains(field)) {
            warning("Invalid data file: Missing field '" + field + "'", false);
            return false;
        }
    }

    if (!root.value("income").isArray()) {
        warning("Invalid data file: 'income' is not an array", false);
        return false;
    }

    if (!root.value("expenses").isArray()) {
        warning("Invalid data file: 'expenses' is not an array", false);
        return false;
    }

    if (!root.value("totalSum").isDouble()) {
        warning("Invalid data file: 'totalSum' is not a number", false);
        return false;
    }

    if (!root.value("incSum").isDouble()) {
        warning("Invalid data file: 'incSum' is not a number", false);
        return false;
    }

    if (!root.value("expSum").isDouble()) {
        warning("Invalid data file: 'expSum' is not a number", false);
        return false;
    }

    if (!root.value("date").isString()) {
        warning("Invalid data file: 'date' is not a string", false);
        return false;
    }

    return true;
}

bool Manager::isValidDataFilePath(const QString path)
{
    QFile file(path);

    QJsonDocument doc = fileAgent->File_GetDataDocument(file);

    QJsonObject root = doc.object();

    return isValidDataFile(root);
}

bool Manager::isEmptyData()
{
    QFile file(settings.strDataPath);

    QJsonDocument doc = fileAgent->File_GetDataDocument(file);

    if (doc.isNull() || doc.isEmpty()) {
        log("Data file is empty - no backup created");
        return true;
    }

    QJsonObject root = doc.object();

    bool hasIncome = root.contains("income") && root.value("income").isArray() &&
                     !root.value("income").toArray().isEmpty();
    bool hasExpenses = root.contains("expenses") && root.value("expenses").isArray() &&
                       !root.value("expenses").toArray().isEmpty();

    // only create backup if there is data
    if (!hasIncome && !hasExpenses) {
        log("No income or expense data found: no backup created");
        return true;
    }

    return false;
}

void Manager::createBackup()
{
    if (settings.intMaxBackups == 0) return;

    QString originalPath = settings.strDataPath;

    if (isEmptyData()) return;

    QFileInfo fileInfo(originalPath);
    QString backupDir = fileInfo.absolutePath() + "/backups";

    QDir().mkpath(backupDir);

    QString timestamp = QDateTime::currentDateTime().toString(getDateFormat(settings.intDateFormat, true));
    QString backupPath = backupDir + "/backup_" + timestamp + ".json";

    if (QFile::exists(originalPath)) {
        QFile::copy(originalPath, backupPath);
        log("Created backup: " + backupPath);
    }

    // Delete old backups (max. settings.intMaxBackups)
    cleanupOldBackups(backupDir, settings.intMaxBackups);
}

void Manager::cleanupOldBackups(const QString &backupDir, int maxBackups)
{
    QDir dir(backupDir);
    QStringList backups = dir.entryList(QStringList() << "backup_*.json", QDir::Files, QDir::Time);

    while (backups.size() > maxBackups) {
        QString oldest = backups.takeLast();  // Oldest backup
        QString fullPath = backupDir + "/" + oldest;
        QFile::remove(fullPath);
        log("Removed old backup: " + oldest);
    }
}

bool Manager::saveSettings(appSettings settingsStruct, bool restart)
{
    if (!restart) {
        // if restart is not required, I can directly updated the program settings variables. If restart is required, I don't need to do the stress of updating the program variables anyways.
        settings.intLanguage = settingsStruct.intLanguage;
        settings.intCurrency = settingsStruct.intCurrency;
        settings.intDateFormat = settingsStruct.intDateFormat;
        settings.intMaxBackups = settingsStruct.intMaxBackups;
        settings.bEnableLogging = settingsStruct.bEnableLogging;
        settings.bConfirmDelete = settingsStruct.bConfirmDelete;
        settings.bBackupOnExit = settingsStruct.bBackupOnExit;
        settings.bAutoSave = settingsStruct.bAutoSave;
        settings.strDataPath = settingsStruct.strDataPath;
    }

    QFile file("settings.json");

    QJsonDocument doc = fileAgent->File_GetDataDocument(file);
    QJsonObject root = doc.object();

    QJsonObject appObj = root.value("app").toObject();
    QJsonObject userObj = root.value("user").toObject();
    QJsonObject featuresObj = root.value("features").toObject();
    QJsonObject dataObj = root.value("data").toObject();


    root["app"] = appObj;

    // User
    userObj["language"] = settingsStruct.intLanguage;
    userObj["currency"] = settingsStruct.intCurrency;
    userObj["dateFormat"] = settingsStruct.intDateFormat;
    root["user"] = userObj;


    featuresObj["enableLogging"] = settingsStruct.bEnableLogging;
    featuresObj["confirmDelete"] = settingsStruct.bConfirmDelete;
    root["features"] = featuresObj;

    // Data
    dataObj["dataPath"] = settingsStruct.strDataPath;
    dataObj["backupOnExit"] = settingsStruct.bBackupOnExit;
    dataObj["autoSave"] = settingsStruct.bAutoSave;
    dataObj["maxBackups"] = settingsStruct.intMaxBackups;
    root["data"] = dataObj;


    doc.setObject(root);
    fileAgent->File_Open(file, 1);
    file.write(doc.toJson(QJsonDocument::JsonFormat::Indented));
    file.close();

    log("Saved settings");

    if (restart) {
        log("Quitting application");
        log("Initiating application restart");
        //QProcess::startDetached(QApplication::applicationFilePath()); // Remove this line if I want to disable automatic restarts

        QApplication::quit();
        return false;
    }
    return true;
}

void Manager::Init()
{
    GetSettings();
    GetData(settings.strDataPath);

}

void Manager::GetSettings()
{
    QFile file("settings.json");

    QJsonDocument doc = fileAgent->File_GetDataDocument(file);

    settingsRoot = doc.object();

    QJsonObject appObj = settingsRoot.value("app").toObject();
    QJsonObject userObj = settingsRoot.value("user").toObject();
    QJsonObject featuresObj = settingsRoot.value("features").toObject();
    QJsonObject dataObj = settingsRoot.value("data").toObject();

    settings.bAutoSave = dataObj.value("autoSave").toBool();
    settings.bDevtools = featuresObj.value("devtoolsEnabled").toBool();
    settings.bConfirmDelete = featuresObj.value("confirmDelete").toBool();
    settings.bBackupOnExit = dataObj.value("backupOnExit").toBool();

    settings.intLanguage = userObj.value("language").toInt();
    settings.intCurrency = userObj.value("currency").toInt();
    settings.intDateFormat = userObj.value("dateFormat").toInt();
    settings.intMaxBackups = dataObj.value("maxBackups").toInt();

    settings.strVersion = appObj.value("version").toString();
    settings.strDataPath = dataObj.value("dataPath").toString();

    if (settings.bDevtools) {
        settings.bEnableLogging = featuresObj.value("enableLogging").toBool();
        log("DEVTOOLS: Enabled");
    }
}

bool Manager::GetData(QString path)
{
    QFile file(path);

    QJsonDocument doc = fileAgent->File_GetDataDocument(file);

    QJsonObject root = doc.object();

    if (!isValidDataFile(root)) {
        critical(t("ERR_DATA_INVL"));
        return false;
    }


    setDataIncome(root.value("income").toArray());
    setDataExpenses(root.value("expenses").toArray());
    setDataProfit(root.value("totalSum").toInt());
    setDataIncSum(root.value("incSum").toInt(), root.value("incSumStrict").toInt());
    setDataExpSum(root.value("expSum").toInt(), root.value("expSum").toInt());
    date = root.value("date").toString();

    return true;
}

int Manager::generateUniqueId(int page)
{
    QMap<int, QJsonObject> &map = (page == 2) ? expensesMap : incomeMap;
    if (map.isEmpty()) {
        return 1;
    }
    return map.lastKey() + 1;
}
