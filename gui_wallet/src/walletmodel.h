#ifndef WALLETMODEL_H
#define WALLETMODEL_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QVector>
#include <memory>

#include "../../wallet/wallet.h"
#include "../../src/config.h"

class WalletModel : public QObject
{
    Q_OBJECT

public:
    explicit WalletModel(QObject *parent = nullptr);
    ~WalletModel();

    // Wallet initialization
    bool initWallet(const QString& walletPath);
    bool createNewWallet(const QString& walletPath);
    bool isWalletLoaded() const;

    // Address management
    QString generateNewAddress();
    QStringList getAddresses() const;
    bool hasPrivateKey(const QString& address) const;

    // Balance management
    Pebicoin::Mizutsi getBalance() const;
    void updateBalance();

    // Transaction management
    bool createAndSignTransaction(const QString& fromAddress, const QString& toAddress, 
                                  Pebicoin::Mizutsi amount, Pebicoin::Mizutsi fee);

    // Broadcast a transaction to the network
    bool broadcastTransaction(const QString& txId);
    bool importPrivateKey(const QString& privateKey);

    // Conversion helpers
    static QString formatPBC(Pebicoin::Mizutsi amount);
    static QString formatMizutsi(Pebicoin::Mizutsi amount);
    static Pebicoin::Mizutsi pbcToMizutsi(double pbc);
    static double mizutsiToPBC(Pebicoin::Mizutsi mizutsi);

signals:
    void walletLoaded();
    void walletCreated();
    void balanceChanged(qint64 newBalance); // ✅ استبدال Mizutsi بـ qint64 هنا فقط
    void addressAdded(const QString& address);
    void transactionCreated(const QString& txid);
    void error(const QString& message);

private:
    std::unique_ptr<Pebicoin::Wallet> wallet;
    bool walletLoadedFlag;
};

#endif // WALLETMODEL_H
