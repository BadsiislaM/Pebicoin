#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>
#include <QClipboard>
#include <QMessageBox>
#include <QFileDialog>
#include <QInputDialog>
#include <QTimer>
#include <QSettings>
#include <QDateTime>
#include <QStringListModel>
#include <QRadioButton>         // ✅ مضاف
#include <QCheckBox>            // ✅ مضاف
#include <QDialogButtonBox>     // ✅ مضاف

#include "../../wallet/wallet.h"
#include "../../src/config.h"
#include "walletmodel.h"        // ✅ مضاف

using Pebicoin::Wallet;
using Pebicoin::Transaction;
using Pebicoin::Mizutsi;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    // Menu actions
    void on_actionExit_triggered();
    void on_actionImportPrivateKey_triggered();
    void on_actionBackupWallet_triggered();
    void on_actionAbout_triggered();
    void on_actionSettings_triggered();
    
    // Overview tab
    void on_pushButtonNewAddress_clicked();
    void on_pushButtonCopyAddress_clicked();
    void on_comboBoxAddresses_currentIndexChanged(int index);
    void on_pushButtonRefresh_clicked();
    
    // Send tab
    void on_pushButtonSend_clicked();
    void on_pushButtonAddToAddressBook_clicked();
    
    // Receive tab
    void on_pushButtonNewAddressReceive_clicked();
    void on_pushButtonCopyAddressReceive_clicked();
    
    // Address Book tab
    void on_pushButtonAddAddress_clicked();
    void on_pushButtonEditAddress_clicked();
    void on_pushButtonDeleteAddress_clicked();
    void on_pushButtonUseAddress_clicked();
    
    // Timer update
    void updateWalletInfo();
    void updateNetworkStatus();

private:
    Ui::MainWindow *ui;
    WalletModel* walletModel;                      // ✅ مضاف
    QStandardItemModel *transactionModel;
    QStandardItemModel *recentTransactionModel;
    QStringListModel *addressesModel;
    QStandardItemModel *addressBookModel;
    QTimer *updateTimer;
    QTimer *networkStatusTimer;
    QLabel *statusLabel;
    QLabel *connectionLabel;
    QLabel *statusConnectionLabel;                 // ✅ مضاف
    QLabel *statusWalletLabel;                     // ✅ مضاف
    QPushButton *refreshButton;
    
    // Settings
    enum DisplayUnit { PBC, MPBC, MIZUTSI };
    DisplayUnit displayUnit;
    bool darkThemeEnabled;
    
    // Address book
    struct AddressBookEntry {
        QString address;
        QString label;
        QString notes;
    };
    QVector<AddressBookEntry> addressBook;
    
    // Transaction labels
    QMap<QString, QString> transactionLabels; // txid -> label
    
    // Network status
    bool isConnected;
    
    // Helper methods
    void initializeWallet();
    void setupModels();
    void setupStatusBar();
    void updateBalanceDisplay();
    void updateAddressesDisplay();
    void updateTransactionsDisplay();
    void updateAddressBookDisplay();
    void displayTransaction(const Transaction& tx, QStandardItemModel* model, int row);
    void showNotification(const QString& title, const QString& message, bool isError = false);
    void loadStyleSheet();
    void saveSettings();
    void loadSettings();
    void saveAddressBook();
    void loadAddressBook();
    void saveTransactionLabels();
    void loadTransactionLabels();
    bool broadcastTransaction(const QString& txId);
    void setDisplayUnit(DisplayUnit unit);
    
    // Conversion helpers
    QString formatAmount(Mizutsi amount) const;
    QString formatPBC(Mizutsi amount) const;
    QString formatMPBC(Mizutsi amount) const;
    QString formatMizutsi(Mizutsi amount) const;
    Mizutsi pbcToMizutsi(double pbc) const;
    double mizutsiToPBC(Mizutsi mizutsi) const;
};

#endif // MAINWINDOW_H
