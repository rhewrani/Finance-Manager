#include "utils.h"
#include <QStringList>
#include <QDebug>
#include <QtMath>


bool isValidEuroAmount(const QString &input) {
    QString s = input.trimmed();

    // Avoid leading zeros (except "0", "0,..." or "-0,...")
    QStringList parts = s.startsWith('-') ? s.mid(1).split(',') : s.split(',');
    QString intPart = parts.first();

    if (intPart.length() > 1 && intPart.startsWith('0')) {
        return false;  // "0123", "00", "01" -> invalid
    }

    return true;
}

int stringToEurocent(const QString &amountStr)
{
    if (amountStr.isEmpty()) return 0;

    QString clean = amountStr;
    clean.replace(',', '.');

    bool ok;
    double euro = clean.toDouble(&ok);

    if (!ok) {
        Logger::instance()->critical("Conversion failed: " + amountStr);
        return 0;
    }

    return qRound(euro * 100.0);
}

QString translate(const QString &key, int lang)
{
    switch (lang) {
    case 0:  // English
        return texts_en.value(key, key);
    case 1:  // German
        return texts_de.value(key, key);
    case 2:  // Dutch
        return texts_nl.value(key, key);
    case 3:  // French
        return texts_fr.value(key, key);
    case 4:  // Spanish
        return texts_es.value(key, key);
    case 5:  // Italian
        return texts_it.value(key, key);
    case 6:  // Chinese
        return texts_zh.value(key, key);
    case 7:  // Japanese
        return texts_jp.value(key, key);
    case 8:  // Korean
        return texts_kr.value(key, key);
    case 9:  // Arabic
        return texts_ar.value(key, key);
    default:
        return key;  // Fallback: Key itself
    }
}

CurrencyConverter* CurrencyConverter::instance()
{
    static CurrencyConverter instance;
    return &instance;
}

CurrencyConverter::CurrencyConverter()
{
    exchangeRates[0] = 1.0;      // EUR
    exchangeRates[1] = 1.1736;     // USD
    exchangeRates[2] = 0.87080;     // GBP
    exchangeRates[3] = 0.9344;     // CHF
    exchangeRates[4] = 173.79;   // JPY
    exchangeRates[5] = 1640.88;  // KRW (South Korean Won)
    exchangeRates[6] = 1.7802;     // AUD (Australian Dollar)
    exchangeRates[7] = 1.6214;     // CAD (Canadian Dollar)
    exchangeRates[8] = 2.0029;     // NZD (New Zealand Dollar)
    exchangeRates[9] = 11.0705;     // SEK (Swedish Krona)
    exchangeRates[10] = 11.6705;     // NOK (Norwegian Krone)
    exchangeRates[11] = 4.2630;    // PLN (Polish Zloty)
    exchangeRates[12] = 24.292;   // CZK (Czech Koruna)
    exchangeRates[13] = 390.55;  // HUF (Hungarian Forint)
    exchangeRates[14] = 98.19;  // RUB (Russian Ruble)
    exchangeRates[15] = 6.2567;    // BRL (Brazilian Real)
    exchangeRates[16] = 8.3510;    // CNY (Chinese Yuan)
    exchangeRates[17] = 21.6277;   // MXN (Mexican Peso)
    exchangeRates[18] = 1.5081;    // SGD (Singapore Dollar)
    exchangeRates[19] = 37.420;   // THB (Thai Baht)
    exchangeRates[20] = 8.25;     // HKD (Hong Kong Dollar)
    exchangeRates[21] = 67.032;    // PHP (Philippine Peso)
    exchangeRates[22] = 5.0750;     // RON (Romanian Leu)
    exchangeRates[23] = 143.00;   // ISK (Icelandic Króna)
    exchangeRates[24] = 48.5812;    // TRY (Turkish Lira)
    exchangeRates[25] = 19541.55; // IDR (Indonesian Rupiah)
    exchangeRates[26] = 103.4650;    // INR (Indian Rupee)
    exchangeRates[27] = 4.9373;     // MYR (Malaysian Ringgit)
    exchangeRates[28] = 20.4038;    // ZAR (South African Rand)


    currencyNames[0] = "Euro";
    currencyNames[1] = "US Dollar";
    currencyNames[2] = "British Pound";
    currencyNames[3] = "Swiss Franc";
    currencyNames[4] = "Japanese Yen";
    currencyNames[5] = "South Korean Won";
    currencyNames[6] = "Australian Dollar";
    currencyNames[7] = "Canadian Dollar";
    currencyNames[8] = "New Zealand Dollar";
    currencyNames[9] = "Swedish Krona";
    currencyNames[10] = "Norwegian Krone";
    currencyNames[11] = "Polish Zloty";
    currencyNames[12] = "Czech Koruna";
    currencyNames[13] = "Hungarian Forint";
    currencyNames[14] = "Russian Ruble";
    currencyNames[15] = "Brazilian Real";
    currencyNames[16] = "Chinese Yuan";
    currencyNames[17] = "Mexican Peso";
    currencyNames[18] = "Singapore Dollar";
    currencyNames[19] = "Thai Baht";
    currencyNames[20] = "Hong Kong Dollar";
    currencyNames[21] = "Philippine Peso";
    currencyNames[22] = "Romanian Leu";
    currencyNames[23] = "Icelandic Króna";
    currencyNames[24] = "Turkish Lira";
    currencyNames[25] = "Indonesian Rupiah";
    currencyNames[26] = "Indian Rupee";
    currencyNames[27] = "Malaysian Ringgit";
    currencyNames[28] = "South African Rand";


    currencySymbols[0] = "€";
    currencySymbols[1] = "$";
    currencySymbols[2] = "£";
    currencySymbols[3] = "CHF";
    currencySymbols[4] = "¥";
    currencySymbols[5] = "₩";
    currencySymbols[6] = "A$";
    currencySymbols[7] = "C$";
    currencySymbols[8] = "NZ$";
    currencySymbols[9] = "kr";
    currencySymbols[10] = "kr";
    currencySymbols[11] = "zł";
    currencySymbols[12] = "Kč";
    currencySymbols[13] = "Ft";
    currencySymbols[14] = "₽";
    currencySymbols[15] = "R$";
    currencySymbols[16] = "CN¥";
    currencySymbols[17] = "$";
    currencySymbols[18] = "S$";
    currencySymbols[19] = "฿";
    currencySymbols[20] = "HK$";
    currencySymbols[21] = "₱";
    currencySymbols[22] = "lei";
    currencySymbols[23] = "kr";
    currencySymbols[24] = "₺";
    currencySymbols[25] = "Rp";
    currencySymbols[26] = "₹";
    currencySymbols[27] = "RM";
    currencySymbols[28] = "R";
}

double CurrencyConverter::convert(double amount, int from, int to) const
{
    if (from == to) return amount;
    if (!isValidCurrency(from) || !isValidCurrency(to)) return amount;

    // Calculation with EUR (0) as temporary currency
    double amountInEUR = amount / exchangeRates[from];
    double result = amountInEUR * exchangeRates[to];

    return result;
}

QString CurrencyConverter::currencyName(int currency) const
{
    return currencyNames.value(currency, "Unknown");
}

QString CurrencyConverter::currencySymbol(int currency) const
{
    return currencySymbols.value(currency, "?");
}

bool CurrencyConverter::isValidCurrency(int currency) const
{
    return exchangeRates.contains(currency);
}

Logger* Logger::instance()
{
    static Logger instance;
    return &instance;
}

Logger::Logger()
{
    logFile.setFileName("log.txt");
    if (!logFile.open(QIODevice::Append | QIODevice::Text)) {
        qWarning() << "Can't open log file!";
        return;
    }
    logStream.setDevice(&logFile);
    logStream << "\n=== New Logging-session started: "
              << QDateTime::currentDateTime().toString(getDateFormat(intDateFormat))
              << " ===\n";
    logStream.flush();
}

void Logger::setParentWidget(QWidget *parent, int dateFormat)
{
    messageParent = parent;
    intDateFormat = dateFormat;
}

void Logger::log(const QString &message, bool MB, QWidget *parent)
{
    logMessage(LogLevel::Info, message, false);
}

void Logger::warning(const QString &message, bool MB, QWidget *parent)
{
    logMessage(LogLevel::Warning, message, MB, parent);
}

void Logger::critical(const QString &message, bool MB, QWidget *parent)
{
    logMessage(LogLevel::Critical, message, MB, parent);
}

void Logger::debug(const QString &message)
{
    logMessage(LogLevel::Debug, message, false);
}

void Logger::logMessage(LogLevel level, const QString &message, bool MessageBox, QWidget *parent)
{
    QString timestamp = QDateTime::currentDateTime().toString(getDateFormat(intDateFormat));
    QString levelStr = levelToString(level);

    logStream << "[" << timestamp << "][" << levelStr << "] " << message << "\n";
    logStream.flush();

    if (MessageBox == true && (level == LogLevel::Info || level == LogLevel::Warning || level == LogLevel::Critical)) {
        QWidget *useParent = parent ? parent : messageParent;

        if (useParent) {
            if (level == LogLevel::Warning) {
                QMessageBox::warning(useParent, "Warning", message);
            } else if (level == LogLevel::Critical) {
                QMessageBox::critical(useParent, "Critical", message);
            }
        }
    }
}

QString Logger::levelToString(LogLevel level)
{
    switch (level) {
    case LogLevel::Debug:    return "DEBUG";
    case LogLevel::Info:     return "INFO";
    case LogLevel::Warning:  return "WARN";
    case LogLevel::Critical: return "CRIT";
    default:                 return "INFO";
    }
}


QString getDateFormat(int format, bool backupFormat)
{
    if (backupFormat) {
        switch (format) {
        case 0: return "dd-MM-yyyy_hh-mm-ss";
        case 1: return "MM-dd-yyyy_hh-mm-ss";
        case 2: return "yyyy-MM-dd_hh-mm-ss";
        default: return "dd-MM-yyyy_hh-mm-ss";
        }
    } else {
    switch (format) {
    case 0: return "dd-MM-yyyy hh:mm:ss";
    case 1: return "MM-dd-yyyy hh:mm:ss";
    case 2: return "yyyy-MM-dd hh:mm:ss";
    default: return "dd-MM-yyyy hh:mm:ss";
    }
}
}

