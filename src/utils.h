#ifndef UTILS_H
#define UTILS_H

#include <QString>
#include <QObject>
#include <QObject>
#include <QFile>
#include <QFileInfo>
#include <QFileDialog>
#include <QDir>
#include <QSaveFile>
#include <QJsonObject>
#include <QJsonArray>
#include <QMessageBox>

#include "lang.h"

// Keine Klasse – nur freie Funktionen
bool isValidEuroAmount(const QString &input);
int stringToEurocent(const QString &amountStr);
QString getDateFormat(int format, bool backupFormat = false);
QString translate (const QString &key, int lang);

// utils.h

class CurrencyConverter
{
public:
    static CurrencyConverter* instance();

    double convert(double amount, int fromCurrency, int toCurrency) const;
    QString currencyName(int currency) const;
    QString currencySymbol(int currency) const;

    bool isValidCurrency(int currency) const;

private:
    CurrencyConverter();

    // Kurs-Tabelle (Basis: EUR = 0)
    QMap<int, double> exchangeRates;
    QMap<int, QString> currencyNames;
    QMap<int, QString> currencySymbols;
};

enum class LogLevel {
    Debug,
    Info,
    Warning,
    Critical
};

class Logger : public QObject
{
    Q_OBJECT
public:
    static Logger* instance();

    void setParentWidget(QWidget *parent, int dateFormat);

    void log(const QString &message, bool MB = false, QWidget *parent = nullptr);           // Info (Standard)
    void warning(const QString &message, bool MB = true, QWidget *parent = nullptr); // Warning
    void critical(const QString &message, bool MB = true, QWidget *parent = nullptr); // Cricital
    void debug(const QString &message);         // Debug
private:
    QFile logFile;
    QTextStream logStream;
    QWidget *messageParent = nullptr;
    Logger();

    void logMessage(LogLevel level, const QString &message, bool MessageBox, QWidget *parent = nullptr);
    QString levelToString(LogLevel level);

    int intDateFormat = 0;
};


#endif // UTILS_H
