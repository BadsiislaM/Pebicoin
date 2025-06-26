#ifndef TRANSACTIONMODEL_H
#define TRANSACTIONMODEL_H

#include <QAbstractTableModel>
#include <QDateTime>
#include <QVector>
#include <QString>

#include "../../wallet/transaction.h"
#include "../../src/config.h"

using Pebicoin::Transaction;
using Pebicoin::Mizutsi;

// Structure to represent a transaction in the model
struct TransactionRecord {
    QString txid;           // Transaction ID
    QDateTime timestamp;    // Transaction timestamp
    QString type;           // "Sent" or "Received"
    QString address;        // Address involved (sender or recipient)
    Mizutsi amount;         // Amount in Mizutsi
    bool confirmed;         // Whether the transaction is confirmed
    
    TransactionRecord() : amount(0), confirmed(false) {}
    
    // Constructor from a Transaction object
    TransactionRecord(const Pebicoin::Transaction& tx, const QString& walletAddress, bool isConfirmed = false);
};

class TransactionModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    enum Columns {
        Date = 0,
        Type,
        Address,
        Amount,
        Status,
        ColumnCount
    };
    
    explicit TransactionModel(QObject *parent = nullptr);
    
    // QAbstractTableModel interface implementation
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    
    // Transaction management
    void addTransaction(const Pebicoin::Transaction& tx, const QString& walletAddress);
    void addTransaction(const TransactionRecord& record);
    void clear();
    
    // Get transaction by index
    TransactionRecord getTransaction(int index) const;
    
    // Get recent transactions (limited number)
    QVector<TransactionRecord> getRecentTransactions(int count) const;

private:
    QVector<TransactionRecord> transactions;
};

#endif // TRANSACTIONMODEL_H