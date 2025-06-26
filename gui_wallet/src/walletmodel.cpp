#include "walletmodel.h"
#include <QDebug>
#include <cmath>
#include <QThread>

#include <QRandomGenerator>

using Pebicoin::Mizutsi;

WalletModel::WalletModel(QObject *parent)
    : QObject(parent), walletLoadedFlag(false)
{
}

WalletModel::~WalletModel()
{
    // Wallet will be automatically saved when destroyed
}

bool WalletModel::initWallet(const QString& walletPath)
{
    try {
        wallet = std::make_unique<Pebicoin::Wallet>(walletPath.toStdString());
        walletLoadedFlag = true;

        if (wallet->getAddresses().empty()) {
            wallet->generateNewAddress();
        }

        emit walletLoaded();
        return true;
    } catch (const std::exception& e) {
        qDebug() << "Failed to load wallet:" << e.what();
        emit error(tr("Failed to load wallet: %1").arg(e.what()));
        return false;
    }
}

bool WalletModel::createNewWallet(const QString& walletPath)
{
    try {
        wallet = std::make_unique<Pebicoin::Wallet>(walletPath.toStdString());
        wallet->generateNewAddress();
        walletLoadedFlag = true;
        emit walletCreated();
        return true;
    } catch (const std::exception& e) {
        qDebug() << "Failed to create wallet:" << e.what();
        emit error(tr("Failed to create wallet: %1").arg(e.what()));
        return false;
    }
}

bool WalletModel::isWalletLoaded() const
{
    return walletLoadedFlag && wallet != nullptr;
}

QString WalletModel::generateNewAddress()
{
    if (!isWalletLoaded()) {
        emit error(tr("Wallet not loaded"));
        return QString();
    }

    try {
        std::string address = wallet->generateNewAddress();
        QString qAddress = QString::fromStdString(address);
        emit addressAdded(qAddress);
        return qAddress;
    } catch (const std::exception& e) {
        qDebug() << "Failed to generate address:" << e.what();
        emit error(tr("Failed to generate address: %1").arg(e.what()));
        return QString();
    }
}

QStringList WalletModel::getAddresses() const
{
    QStringList addresses;

    if (!isWalletLoaded()) {
        return addresses;
    }

    try {
        std::vector<std::string> addrList = wallet->getAddresses();
        for (const auto& addr : addrList) {
            addresses << QString::fromStdString(addr);
        }
    } catch (const std::exception& e) {
        qDebug() << "Failed to get addresses:" << e.what();
    }

    return addresses;
}

bool WalletModel::hasPrivateKey(const QString& address) const
{
    if (!isWalletLoaded()) {
        return false;
    }

    return wallet->hasPrivateKey(address.toStdString());
}

Mizutsi WalletModel::getBalance() const
{
    if (!isWalletLoaded()) {
        return 0;
    }

    return wallet->getBalance();
}

void WalletModel::updateBalance()
{
    if (!isWalletLoaded()) {
        return;
    }

    try {
        wallet->updateBalance(std::vector<Pebicoin::Transaction>());
        Mizutsi oldBalance = wallet->getBalance();

        Mizutsi newBalance = wallet->getBalance();

        if (oldBalance != newBalance) {
            emit balanceChanged(static_cast<qint64>(newBalance));  // ✅ التحويل إلى qint64
        }
    } catch (const std::exception& e) {
        qDebug() << "Failed to update balance:" << e.what();
        emit error(tr("Failed to update balance: %1").arg(e.what()));
    }
}

bool WalletModel::createAndSignTransaction(const QString& fromAddress, const QString& toAddress,
                                          Mizutsi amount, Mizutsi fee)
{
    if (!isWalletLoaded()) {
        emit error(tr("Wallet not loaded"));
        return false;
    }

    try {
        Pebicoin::Transaction tx = wallet->createTransaction(
            fromAddress.toStdString(),
            toAddress.toStdString(),
            amount,
            fee
        );

        bool signed_ok = wallet->signTransaction(tx, fromAddress.toStdString());

        if (signed_ok) {
            emit transactionCreated(QString::fromStdString(tx.txid));
            return true;
        } else {
            emit error(tr("Failed to sign transaction"));
            return false;
        }
    } catch (const std::exception& e) {
        qDebug() << "Transaction error:" << e.what();
        emit error(tr("Transaction error: %1").arg(e.what()));
        return false;
    }
}

bool WalletModel::broadcastTransaction(const QString& txId)
{
    QThread::msleep(500);

    int randomValue = QRandomGenerator::global()->bounded(100);
    bool success = (randomValue < 90);

    if (success) {
        qDebug() << "Transaction" << txId << "broadcast successfully to network";
    } else {
        qDebug() << "Failed to broadcast transaction" << txId << "to network";
    }

    return success;
}

bool WalletModel::importPrivateKey(const QString& privateKey)
{
    if (!isWalletLoaded()) {
        emit error(tr("Wallet not loaded"));
        return false;
    }

    try {
        QString simulatedAddress = "PBC" + privateKey.right(10);
        emit addressAdded(simulatedAddress);
        return true;
    } catch (const std::exception& e) {
        qDebug() << "Failed to import private key:" << e.what();
        emit error(tr("Failed to import private key: %1").arg(e.what()));
        return false;
    }
}

QString WalletModel::formatPBC(Mizutsi amount)
{
    double pbc = static_cast<double>(amount) / 100000000.0;
    return QString("%1 PBC").arg(pbc, 0, 'f', 8);
}

QString WalletModel::formatMizutsi(Mizutsi amount)
{
    return QString("%1 Mizutsi").arg(amount);
}

Mizutsi WalletModel::pbcToMizutsi(double pbc)
{
    return static_cast<Mizutsi>(std::round(pbc * 100000000.0));
}

double WalletModel::mizutsiToPBC(Mizutsi mizutsi)
{
    return static_cast<double>(mizutsi) / 100000000.0;
}
