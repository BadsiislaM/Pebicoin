#include "transactionmodel.h"
#include "walletmodel.h"
#include <QColor>
#include <QFont>

TransactionRecord::TransactionRecord(const Pebicoin::Transaction& tx, const QString& walletAddress, bool isConfirmed)
    : txid(QString::fromStdString(tx.txid)),
      confirmed(isConfirmed)
{
    // Set timestamp
    timestamp.setSecsSinceEpoch(tx.timestamp);
    
    // Determine if this is a send or receive transaction
    // This is a simplified approach - in a real wallet, you'd need to analyze inputs and outputs
    bool isSend = false;
    QString addressUsed;
    Mizutsi transactionAmount = 0;
    
    // Check outputs to see if any are to our wallet address
    for (const auto& output : tx.outputs) {
        QString outputAddress = QString::fromStdString(output.address);
        
        if (outputAddress == walletAddress) {
            // This is a receive transaction
            isSend = false;
            addressUsed = outputAddress; // Our address
            transactionAmount = output.amount;
            break;
        } else {
            // This might be a send transaction
            isSend = true;
            addressUsed = outputAddress; // Recipient address
            transactionAmount = output.amount;
        }
    }
    
    // Set transaction type and address
    type = isSend ? "Sent" : "Received";
    address = addressUsed;
    amount = transactionAmount;
}

TransactionModel::TransactionModel(QObject *parent)
    : QAbstractTableModel(parent)
{
}

int TransactionModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    
    return transactions.size();
}

int TransactionModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    
    return ColumnCount;
}

QVariant TransactionModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= transactions.size())
        return QVariant();
    
    const TransactionRecord &record = transactions.at(index.row());
    
    if (role == Qt::DisplayRole) {
        switch (index.column()) {
            case Date:
                return record.timestamp.toString("yyyy-MM-dd hh:mm");
            case Type:
                return record.type;
            case Address:
                return record.address;
            case Amount:
                return WalletModel::formatPBC(record.amount);
            case Status:
                return record.confirmed ? tr("Confirmed") : tr("Pending");
            default:
                return QVariant();
        }
    } else if (role == Qt::TextAlignmentRole) {
        if (index.column() == Amount)
            return Qt::AlignRight + Qt::AlignVCenter;
        return Qt::AlignLeft + Qt::AlignVCenter;
    } else if (role == Qt::ForegroundRole) {
        if (index.column() == Amount) {
            // Green for received, red for sent
            return record.type == "Received" ? QColor(0, 170, 0) : QColor(255, 0, 0);
        }
        return QVariant();
    } else if (role == Qt::FontRole) {
        if (index.column() == Amount) {
            QFont font;
            font.setBold(true);
            return font;
        }
        return QVariant();
    }
    
    return QVariant();
}

QVariant TransactionModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
            case Date: return tr("Date");
            case Type: return tr("Type");
            case Address: return tr("Address");
            case Amount: return tr("Amount");
            case Status: return tr("Status");
            default: return QVariant();
        }
    }
    
    return QVariant();
}

void TransactionModel::addTransaction(const Pebicoin::Transaction& tx, const QString& walletAddress)
{
    beginInsertRows(QModelIndex(), 0, 0);
    transactions.prepend(TransactionRecord(tx, walletAddress));
    endInsertRows();
}

void TransactionModel::addTransaction(const TransactionRecord& record)
{
    beginInsertRows(QModelIndex(), 0, 0);
    transactions.prepend(record);
    endInsertRows();
}

void TransactionModel::clear()
{
    beginResetModel();
    transactions.clear();
    endResetModel();
}

TransactionRecord TransactionModel::getTransaction(int index) const
{
    if (index >= 0 && index < transactions.size())
        return transactions.at(index);
    
    return TransactionRecord();
}

QVector<TransactionRecord> TransactionModel::getRecentTransactions(int count) const
{
    QVector<TransactionRecord> recent;
    int numToGet = qMin(count, transactions.size());
    
    for (int i = 0; i < numToGet; ++i) {
        recent.append(transactions.at(i));
    }
    
    return recent;
}