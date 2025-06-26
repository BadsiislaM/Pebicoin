#include "mainwindow.h"
#include "../ui/ui_mainwindow.h"
#include <QFile>
#include <QTextStream>
#include <QStandardPaths>
#include <QDir>
#include <QDebug>
#include <cmath>
#include <QRandomGenerator>

using Pebicoin::Mizutsi;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , displayUnit(PBC)
    , darkThemeEnabled(true)
    , isConnected(false)
{
    ui->setupUi(this);
    
    // Load settings
    loadSettings();
    
    // Load stylesheet
    loadStyleSheet();
    
    // Setup models for UI components
    setupModels();
    
    // Setup status bar
    setupStatusBar();
    
    // Initialize wallet
    initializeWallet();
    
    // Load address book
    loadAddressBook();
    
    // Load transaction labels
    loadTransactionLabels();
    
    // Setup timer for periodic updates
    updateTimer = new QTimer(this);
    connect(updateTimer, &QTimer::timeout, this, &MainWindow::updateWalletInfo);
    updateTimer->start(30000); // Update every 30 seconds
    
    // Setup timer for network status updates
    networkStatusTimer = new QTimer(this);
    connect(networkStatusTimer, &QTimer::timeout, this, &MainWindow::updateNetworkStatus);
    networkStatusTimer->start(5000); // Update every 5 seconds
    
    // Initial updates
    updateWalletInfo();
    updateNetworkStatus();
    updateAddressBookDisplay();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::loadStyleSheet()
{
    QString stylesheetPath = darkThemeEnabled ? ":/styles/dark.qss" : ":/styles/light.qss";
    QFile file(stylesheetPath);
    
    // Default to dark theme if light theme stylesheet doesn't exist
    if (!file.exists() && !darkThemeEnabled) {
        file.setFileName(":/styles/dark.qss");
    }
    
    if (file.open(QFile::ReadOnly | QFile::Text))
    {
        QTextStream stream(&file);
        qApp->setStyleSheet(stream.readAll());
        file.close();
    } else {
        qDebug() << "Failed to load stylesheet";
    }
}

void MainWindow::setupStatusBar()
{
    // Create status label for network connection
    statusConnectionLabel = new QLabel(this);
    statusConnectionLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    statusConnectionLabel->setMinimumWidth(100);
    
    // Create status label for wallet info
    statusWalletLabel = new QLabel(this);
    statusWalletLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    statusWalletLabel->setMinimumWidth(200);
    
    // Add labels to status bar
    ui->statusbar->addPermanentWidget(statusWalletLabel);
    ui->statusbar->addPermanentWidget(statusConnectionLabel);
    
    // Initial status
    updateNetworkStatus();
}

void MainWindow::updateNetworkStatus()
{
    // In a real implementation, this would check actual network connectivity
    // For now, we'll simulate connection status
    
    // Simulate network connection (toggle for demonstration)
    static int counter = 0;
    counter++;
    
    // Every 6 calls (30 seconds with 5 second timer), toggle connection status for demo
    if (counter % 6 == 0) {
        isConnected = !isConnected;
    }
    
    if (isConnected) {
        statusConnectionLabel->setText("Connected");
        statusConnectionLabel->setStyleSheet("QLabel { color: green; }");
        statusWalletLabel->setText("Synced");
    } else {
        statusConnectionLabel->setText("Offline");
        statusConnectionLabel->setStyleSheet("QLabel { color: red; }");
        statusWalletLabel->setText("Not Synced");
    }
}

void MainWindow::loadSettings()
{
    QSettings settings("Pebicoin", "PebiWallet");
    
    // Load display unit preference
    int unitValue = settings.value("displayUnit", static_cast<int>(PBC)).toInt();
    displayUnit = static_cast<DisplayUnit>(unitValue);
    
    // Load theme preference
    darkThemeEnabled = settings.value("darkTheme", true).toBool();
}

void MainWindow::saveSettings()
{
    QSettings settings("Pebicoin", "PebiWallet");
    
    // Save display unit preference
    settings.setValue("displayUnit", static_cast<int>(displayUnit));
    
    // Save theme preference
    settings.setValue("darkTheme", darkThemeEnabled);
}

void MainWindow::loadAddressBook()
{
    QSettings settings("Pebicoin", "PebiWallet");
    int size = settings.beginReadArray("addressBook");
    
    for (int i = 0; i < size; ++i) {
        settings.setArrayIndex(i);
        AddressBookEntry entry;
        entry.address = settings.value("address").toString();
        entry.label = settings.value("label").toString();
        addressBook.append(entry);
    }
    
    settings.endArray();
    updateAddressBookDisplay();
}

void MainWindow::saveAddressBook()
{
    QSettings settings("Pebicoin", "PebiWallet");
    settings.beginWriteArray("addressBook");
    
    for (int i = 0; i < addressBook.size(); ++i) {
        settings.setArrayIndex(i);
        settings.setValue("address", addressBook.at(i).address);
        settings.setValue("label", addressBook.at(i).label);
    }
    
    settings.endArray();
}

void MainWindow::loadTransactionLabels()
{
    QSettings settings("Pebicoin", "PebiWallet");
    int size = settings.beginReadArray("transactionLabels");
    
    for (int i = 0; i < size; ++i) {
        settings.setArrayIndex(i);
        QString txId = settings.value("txId").toString();
        QString label = settings.value("label").toString();
        transactionLabels[txId] = label;
    }
    
    settings.endArray();
}

void MainWindow::saveTransactionLabels()
{
    QSettings settings("Pebicoin", "PebiWallet");
    settings.beginWriteArray("transactionLabels");
    
    int i = 0;
    for (auto it = transactionLabels.begin(); it != transactionLabels.end(); ++it, ++i) {
        settings.setArrayIndex(i);
        settings.setValue("txId", it.key());
        settings.setValue("label", it.value());
    }
    
    settings.endArray();
}

void MainWindow::updateAddressBookDisplay()
{
    // Create a model for the address book if it doesn't exist
    if (!addressBookModel) {
        addressBookModel = new QStandardItemModel(this);
        addressBookModel->setColumnCount(2);
        addressBookModel->setHorizontalHeaderLabels({"Label", "Address"});
        ui->tableViewAddressBook->setModel(addressBookModel);
        ui->tableViewAddressBook->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    }
    
    // Clear and repopulate the model
    addressBookModel->removeRows(0, addressBookModel->rowCount());
    
    for (const auto& entry : addressBook) {
        QList<QStandardItem*> row;
        row.append(new QStandardItem(entry.label));
        row.append(new QStandardItem(entry.address));
        addressBookModel->appendRow(row);
    }
}

void MainWindow::setupModels()
{
    // Transaction model for the main transactions tab
    transactionModel = new QStandardItemModel(0, 4, this);
    transactionModel->setHeaderData(0, Qt::Horizontal, tr("Date"));
    transactionModel->setHeaderData(1, Qt::Horizontal, tr("Type"));
    transactionModel->setHeaderData(2, Qt::Horizontal, tr("Address"));
    transactionModel->setHeaderData(3, Qt::Horizontal, tr("Amount (PBC)"));
    ui->tableViewTransactions->setModel(transactionModel);
    
    // Recent transactions model for the overview tab
    recentTransactionModel = new QStandardItemModel(0, 4, this);
    recentTransactionModel->setHeaderData(0, Qt::Horizontal, tr("Date"));
    recentTransactionModel->setHeaderData(1, Qt::Horizontal, tr("Type"));
    recentTransactionModel->setHeaderData(2, Qt::Horizontal, tr("Address"));
    recentTransactionModel->setHeaderData(3, Qt::Horizontal, tr("Amount (PBC)"));
    ui->tableViewRecentTransactions->setModel(recentTransactionModel);
    
    // Addresses model for the receive tab
    addressesModel = new QStringListModel(this);
    ui->listViewAddresses->setModel(addressesModel);
}

void MainWindow::initializeWallet()
{
    // Get application data path
    QString dataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dir(dataPath);
    if (!dir.exists()) {
        dir.mkpath(".");
    }
    
    QString walletPath = dataPath + "/wallet.dat";
    
    try {
        // Try to load existing wallet
        walletModel->initWallet(walletPath);
        
        // If no addresses, create one
        if (walletModel->getAddresses().empty()) {
            walletModel->generateNewAddress();
        }
    } catch (const std::exception& e) {
        // Create new wallet if loading fails
        walletModel->createNewWallet(walletPath); // ✅ استبدال wallet
        walletModel->generateNewAddress();
        
        showNotification(tr("New Wallet Created"), 
                         tr("A new wallet has been created. Please backup your wallet file regularly."));
    }
}

void MainWindow::updateWalletInfo()
{
    updateBalanceDisplay();
    updateAddressesDisplay();
    updateTransactionsDisplay();
}

void MainWindow::updateBalanceDisplay()
{
    Mizutsi balance = walletModel->getBalance();
    ui->labelBalance->setText(formatPBC(balance));
    ui->labelBalanceMizutsi->setText(formatMizutsi(balance));
}

void MainWindow::updateAddressesDisplay()
{
    // Get all addresses
    QStringList addresses = walletModel->getAddresses();
    
    // Update combo boxes
    ui->comboBoxAddresses->clear();
    ui->comboBoxFromAddress->clear();
    
    QStringList addressList;
    for (const auto& addr : addresses) {
        QString qAddr = addr;
        ui->comboBoxAddresses->addItem(qAddr);
        ui->comboBoxFromAddress->addItem(qAddr);
        addressList << qAddr;
    }
    
    // Update address list model
    addressesModel->setStringList(addressList);
    
    // Set current address in overview
    if (ui->comboBoxAddresses->count() > 0) {
        ui->lineEditAddress->setText(ui->comboBoxAddresses->currentText());
    }
}

void MainWindow::updateTransactionsDisplay()
{
    // Clear existing data
    transactionModel->removeRows(0, transactionModel->rowCount());
    recentTransactionModel->removeRows(0, recentTransactionModel->rowCount());
    
    // In a real implementation, we would get transactions from the wallet
    // For now, we'll just add some sample transactions
    // This would be replaced with actual transaction data
    
    // Sample transaction data (would come from wallet in real implementation)
    QDateTime now = QDateTime::currentDateTime();
    
    // For demonstration purposes only - replace with actual transaction data
    Transaction sampleTx1;
    sampleTx1.timestamp = now.toSecsSinceEpoch() - 3600; // 1 hour ago
    sampleTx1.outputs.push_back({"PBCAddress123", 500000000}); // 5 PBC
    
    Transaction sampleTx2;
    sampleTx2.timestamp = now.toSecsSinceEpoch() - 86400; // 1 day ago
    sampleTx2.outputs.push_back({"PBCAddress456", 100000000}); // 1 PBC
    
    // Display in both tables
    displayTransaction(sampleTx1, transactionModel, 0);
    displayTransaction(sampleTx2, transactionModel, 1);
    
    // Only show most recent in overview
    displayTransaction(sampleTx1, recentTransactionModel, 0);
    
    // Resize columns to content
    ui->tableViewTransactions->resizeColumnsToContents();
    ui->tableViewRecentTransactions->resizeColumnsToContents();
}

void MainWindow::displayTransaction(const Transaction& tx, QStandardItemModel* model, int row)
{
    // Convert timestamp to readable date
    QDateTime dateTime;
    dateTime.setSecsSinceEpoch(tx.timestamp);
    QString dateStr = dateTime.toString("yyyy-MM-dd hh:mm");
    
    // For simplicity, we're just showing the first output
    // In a real implementation, you'd need to determine if this is incoming or outgoing
    // based on the addresses involved
    QString type = "Received"; // Placeholder
    QString address = QString::fromStdString(tx.outputs[0].address);
    QString amount = formatPBC(tx.outputs[0].amount);
    
    model->insertRow(row);
    model->setData(model->index(row, 0), dateStr);
    model->setData(model->index(row, 1), type);
    model->setData(model->index(row, 2), address);
    model->setData(model->index(row, 3), amount);
}

void MainWindow::showNotification(const QString& title, const QString& message, bool isError)
{
    QMessageBox::Icon icon = isError ? QMessageBox::Critical : QMessageBox::Information;
    QMessageBox msgBox(icon, title, message, QMessageBox::Ok, this);
    msgBox.exec();
}

QString MainWindow::formatPBC(Mizutsi amount) const
{
    double pbc = static_cast<double>(amount) / 100000000.0;
    return QString("%1 PBC").arg(pbc, 0, 'f', 8);
}

QString MainWindow::formatMizutsi(Mizutsi amount) const
{
    return QString("%1 Mizutsi").arg(amount);
}

Mizutsi MainWindow::pbcToMizutsi(double pbc) const
{
    return static_cast<Mizutsi>(std::round(pbc * 100000000.0));
}

// Slot implementations

void MainWindow::on_actionExit_triggered()
{
    close();
}

void MainWindow::on_actionImportPrivateKey_triggered()
{
    QDialog* importDialog = new QDialog(this);
    importDialog->setWindowTitle("Import Wallet");
    importDialog->setFixedWidth(400);
    
    QVBoxLayout* mainLayout = new QVBoxLayout(importDialog);
    
    // Import options group
    QGroupBox* optionsGroup = new QGroupBox("Import Options", importDialog);
    QVBoxLayout* optionsLayout = new QVBoxLayout(optionsGroup);
    
    QRadioButton* walletFileRadio = new QRadioButton("Import wallet.dat file", optionsGroup);
    QRadioButton* privateKeyRadio = new QRadioButton("Import private key", optionsGroup);
    
    privateKeyRadio->setChecked(true);
    
    optionsLayout->addWidget(walletFileRadio);
    optionsLayout->addWidget(privateKeyRadio);
    optionsGroup->setLayout(optionsLayout);
    mainLayout->addWidget(optionsGroup);
    
    // Private key input
    QGroupBox* privateKeyGroup = new QGroupBox("Private Key", importDialog);
    QVBoxLayout* privateKeyLayout = new QVBoxLayout(privateKeyGroup);
    
    QLineEdit* privateKeyEdit = new QLineEdit(privateKeyGroup);
    privateKeyEdit->setPlaceholderText("Enter private key here");
    privateKeyEdit->setEchoMode(QLineEdit::Password);
    
    QCheckBox* showKeyCheckBox = new QCheckBox("Show key", privateKeyGroup);
    connect(showKeyCheckBox, &QCheckBox::toggled, [privateKeyEdit](bool checked) {
        privateKeyEdit->setEchoMode(checked ? QLineEdit::Normal : QLineEdit::Password);
    });
    
    privateKeyLayout->addWidget(privateKeyEdit);
    privateKeyLayout->addWidget(showKeyCheckBox);
    privateKeyGroup->setLayout(privateKeyLayout);
    mainLayout->addWidget(privateKeyGroup);
    
    // Wallet file input
    QGroupBox* walletFileGroup = new QGroupBox("Wallet File", importDialog);
    QVBoxLayout* walletFileLayout = new QVBoxLayout(walletFileGroup);
    
    QHBoxLayout* fileSelectLayout = new QHBoxLayout();
    QLineEdit* walletFileEdit = new QLineEdit(walletFileGroup);
    walletFileEdit->setPlaceholderText("Select wallet.dat file");
    walletFileEdit->setReadOnly(true);
    
    QPushButton* browseButton = new QPushButton("Browse", walletFileGroup);
    connect(browseButton, &QPushButton::clicked, [walletFileEdit, importDialog]() {
        QString fileName = QFileDialog::getOpenFileName(importDialog, "Import Wallet", 
                                                    QDir::homePath(), 
                                                    "Wallet Files (*.dat);;All Files (*)");
        if (!fileName.isEmpty()) {
            walletFileEdit->setText(fileName);
        }
    });
    
    fileSelectLayout->addWidget(walletFileEdit);
    fileSelectLayout->addWidget(browseButton);
    walletFileLayout->addLayout(fileSelectLayout);
    walletFileGroup->setLayout(walletFileLayout);
    mainLayout->addWidget(walletFileGroup);
    
    // Connect radio buttons to show/hide appropriate groups
    connect(privateKeyRadio, &QRadioButton::toggled, [privateKeyGroup, walletFileGroup](bool checked) {
        privateKeyGroup->setVisible(checked);
        walletFileGroup->setVisible(!checked);
    });
    
    connect(walletFileRadio, &QRadioButton::toggled, [privateKeyGroup, walletFileGroup](bool checked) {
        privateKeyGroup->setVisible(!checked);
        walletFileGroup->setVisible(checked);
    });
    
    // Initial visibility
    privateKeyGroup->setVisible(true);
    walletFileGroup->setVisible(false);
    
    // Buttons
    QHBoxLayout* buttonsLayout = new QHBoxLayout();
    QPushButton* cancelButton = new QPushButton("Cancel", importDialog);
    QPushButton* importButton = new QPushButton("Import", importDialog);
    
    buttonsLayout->addWidget(cancelButton);
    buttonsLayout->addWidget(importButton);
    mainLayout->addLayout(buttonsLayout);
    
    // Connect buttons
    connect(cancelButton, &QPushButton::clicked, importDialog, &QDialog::reject);
    connect(importButton, &QPushButton::clicked, [=]() {
        if (privateKeyRadio->isChecked()) {
            // Import private key
            QString privateKey = privateKeyEdit->text().trimmed();
            
            if (privateKey.isEmpty()) {
                QMessageBox::warning(importDialog, "Import Failed", "Please enter a private key.");
                return;
            }
            
            // In a real implementation, this would import the private key
            // For now, we'll simulate the import
            bool success = walletModel->importPrivateKey(privateKey);
            
            if (success) {
                QMessageBox::information(this, "Import Successful", 
                                      "Private key has been imported successfully.");
                updateWalletInfo();
                importDialog->accept();
            } else {
                QMessageBox::warning(importDialog, "Import Failed", 
                                   "Failed to import private key. Invalid key format.");
            }
        } else if (walletFileRadio->isChecked()) {
            // Import wallet.dat file
            QString fileName = walletFileEdit->text();
            
            if (fileName.isEmpty()) {
                QMessageBox::warning(importDialog, "Import Failed", "Please select a wallet file.");
                return;
            }
            
            // In a real implementation, this would import the wallet file
            // For now, we'll simulate the import
            QMessageBox::StandardButton reply = QMessageBox::question(this, "Import Wallet", 
                                                                  "This will replace your current wallet. Are you sure you want to continue?", 
                                                                  QMessageBox::Yes | QMessageBox::No);
            
            if (reply == QMessageBox::Yes) {
                // Simulate wallet import
                QFile walletFile(fileName);
                if (walletFile.exists()) {
                    // In a real implementation, this would copy the wallet file to the wallet directory
                    // and reload the wallet
                    QMessageBox::information(this, "Import Successful", 
                                          "Wallet has been imported successfully. The application will now restart.");
                    
                    // In a real implementation, this would restart the application
                    // For now, we'll just close the dialog and update the wallet info
                    updateWalletInfo();
                    importDialog->accept();
                } else {
                    QMessageBox::warning(importDialog, "Import Failed", 
                                       "Failed to import wallet. File does not exist.");
                }
            }
        }
    });
    
    importDialog->setLayout(mainLayout);
    importDialog->exec();
    delete importDialog;
}

void MainWindow::on_actionBackupWallet_triggered()
{
    QDialog* backupDialog = new QDialog(this);
    backupDialog->setWindowTitle("Backup Wallet");
    backupDialog->setFixedWidth(400);
    
    QVBoxLayout* mainLayout = new QVBoxLayout(backupDialog);
    
    // Backup options group
    QGroupBox* optionsGroup = new QGroupBox("Backup Options", backupDialog);
    QVBoxLayout* optionsLayout = new QVBoxLayout(optionsGroup);
    
    QRadioButton* walletFileRadio = new QRadioButton("Backup wallet.dat file", optionsGroup);
    QRadioButton* privateKeyRadio = new QRadioButton("Export private keys", optionsGroup);
    
    walletFileRadio->setChecked(true);
    
    optionsLayout->addWidget(walletFileRadio);
    optionsLayout->addWidget(privateKeyRadio);
    optionsGroup->setLayout(optionsLayout);
    mainLayout->addWidget(optionsGroup);
    
    // Buttons
    QHBoxLayout* buttonsLayout = new QHBoxLayout();
    QPushButton* cancelButton = new QPushButton("Cancel", backupDialog);
    QPushButton* backupButton = new QPushButton("Backup", backupDialog);
    
    buttonsLayout->addWidget(cancelButton);
    buttonsLayout->addWidget(backupButton);
    mainLayout->addLayout(buttonsLayout);
    
    // Connect buttons
    connect(cancelButton, &QPushButton::clicked, backupDialog, &QDialog::reject);
    connect(backupButton, &QPushButton::clicked, [=]() {
        if (walletFileRadio->isChecked()) {
            // Backup wallet.dat file
            QString fileName = QFileDialog::getSaveFileName(this, "Backup Wallet", 
                                                        QDir::homePath() + "/wallet_backup.dat", 
                                                        "Wallet Files (*.dat);;All Files (*)");
            
            if (!fileName.isEmpty()) {
                // In a real implementation, this would copy the wallet.dat file
                // For now, we'll simulate the backup
                QFile walletFile("wallet.dat");
                bool success = walletFile.copy(fileName);
                
                if (success) {
                    QMessageBox::information(this, "Backup Successful", 
                                          "Wallet has been backed up to:\n" + fileName);
                } else {
                    QMessageBox::warning(this, "Backup Failed", 
                                       "Failed to backup wallet. Error: " + walletFile.errorString());
                }
            }
        } else if (privateKeyRadio->isChecked()) {
            // Export private keys
            QString fileName = QFileDialog::getSaveFileName(this, "Export Private Keys", 
                                                        QDir::homePath() + "/pebicoin_private_keys.txt", 
                                                        "Text Files (*.txt);;All Files (*)");
            
            if (!fileName.isEmpty()) {
                // In a real implementation, this would export the private keys
                // For now, we'll simulate the export with dummy keys
                QFile keyFile(fileName);
                if (keyFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
                    QTextStream out(&keyFile);
                    out << "# Pebicoin private keys export - KEEP THIS FILE SECURE!\n";
                    out << "# Date: " << QDateTime::currentDateTime().toString() << "\n\n";
                    
                    // Get addresses from wallet model
                    QStringList addresses = walletModel->getAddresses();
                    
                    for (const QString& address : addresses) {
                        // In a real implementation, this would get the actual private key
                        // For now, we'll use a dummy key
                        QString dummyKey = "KyXwTh5yQB7jKRj2BnNkJ6EmHZdZc8zKUfJosGDFJ9TnJxYRJze" + address.right(8);
                        out << address << ": " << dummyKey << "\n";
                    }
                    
                    keyFile.close();
                    QMessageBox::information(this, "Export Successful", 
                                          "Private keys have been exported to:\n" + fileName + 
                                          "\n\nWARNING: Keep this file secure! Anyone with access to your private keys can spend your coins.");
                } else {
                    QMessageBox::warning(this, "Export Failed", 
                                       "Failed to export private keys. Error: " + keyFile.errorString());
                }
            }
        }
        
        backupDialog->accept();
    });
    
    backupDialog->setLayout(mainLayout);
    backupDialog->exec();
    delete backupDialog;
}

void MainWindow::on_actionAbout_triggered()
{
    QDialog* aboutDialog = new QDialog(this);
    aboutDialog->setWindowTitle("About PebiWallet");
    aboutDialog->setFixedSize(400, 300);
    
    QVBoxLayout* layout = new QVBoxLayout(aboutDialog);
    
    // Logo
    QLabel* logoLabel = new QLabel(aboutDialog);
    QPixmap logo(":/icons/pebicoin.svg");
    logoLabel->setPixmap(logo.scaled(100, 100, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    logoLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(logoLabel);
    
    // Title
    QLabel* titleLabel = new QLabel("PebiWallet", aboutDialog);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(16);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(titleLabel);
    
    // Version
    QLabel* versionLabel = new QLabel("Version 1.0", aboutDialog);
    versionLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(versionLabel);
    
    // Description
    QLabel* descLabel = new QLabel("A secure wallet for Pebicoin cryptocurrency.", aboutDialog);
    descLabel->setAlignment(Qt::AlignCenter);
    descLabel->setWordWrap(true);
    layout->addWidget(descLabel);
    
    // Creator info
    QLabel* creatorLabel = new QLabel("Created by Pebicoin Development Team", aboutDialog);
    creatorLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(creatorLabel);
    
    // Copyright
    QLabel* copyrightLabel = new QLabel("© 2023 Pebicoin", aboutDialog);
    copyrightLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(copyrightLabel);
    
    // Close button
    QPushButton* closeButton = new QPushButton("Close", aboutDialog);
    connect(closeButton, &QPushButton::clicked, aboutDialog, &QDialog::accept);
    layout->addWidget(closeButton);
    
    aboutDialog->setLayout(layout);
    aboutDialog->exec();
    delete aboutDialog;
}

void MainWindow::on_actionSettings_triggered()
{
    QDialog* settingsDialog = new QDialog(this);
    settingsDialog->setWindowTitle("Preferences");
    settingsDialog->setFixedWidth(400);
    
    QVBoxLayout* mainLayout = new QVBoxLayout(settingsDialog);
    
    // Display units group
    QGroupBox* unitsGroup = new QGroupBox("Display Units", settingsDialog);
    QVBoxLayout* unitsLayout = new QVBoxLayout(unitsGroup);
    
    QRadioButton* pbcRadio = new QRadioButton("PBC", unitsGroup);
    QRadioButton* mpbcRadio = new QRadioButton("mPBC (1/1000 PBC)", unitsGroup);
    QRadioButton* mizutsiRadio = new QRadioButton("Mizutsi (1/100,000,000 PBC)", unitsGroup);
    
    // Set the current selection based on current display unit
    switch (displayUnit) {
        case PBC:
            pbcRadio->setChecked(true);
            break;
        case MPBC:
            mpbcRadio->setChecked(true);
            break;
        case MIZUTSI:
            mizutsiRadio->setChecked(true);
            break;
    }
    
    unitsLayout->addWidget(pbcRadio);
    unitsLayout->addWidget(mpbcRadio);
    unitsLayout->addWidget(mizutsiRadio);
    unitsGroup->setLayout(unitsLayout);
    mainLayout->addWidget(unitsGroup);
    
    // Theme group
    QGroupBox* themeGroup = new QGroupBox("Theme", settingsDialog);
    QVBoxLayout* themeLayout = new QVBoxLayout(themeGroup);
    
    QRadioButton* darkRadio = new QRadioButton("Dark Theme", themeGroup);
    QRadioButton* lightRadio = new QRadioButton("Light Theme", themeGroup);
    
    darkRadio->setChecked(darkThemeEnabled);
    lightRadio->setChecked(!darkThemeEnabled);
    
    themeLayout->addWidget(darkRadio);
    themeLayout->addWidget(lightRadio);
    themeGroup->setLayout(themeLayout);
    mainLayout->addWidget(themeGroup);
    
    // Buttons
    QHBoxLayout* buttonsLayout = new QHBoxLayout();
    QPushButton* cancelButton = new QPushButton("Cancel", settingsDialog);
    QPushButton* saveButton = new QPushButton("Save", settingsDialog);
    
    buttonsLayout->addWidget(cancelButton);
    buttonsLayout->addWidget(saveButton);
    mainLayout->addLayout(buttonsLayout);
    
    // Connect buttons
    connect(cancelButton, &QPushButton::clicked, settingsDialog, &QDialog::reject);
    connect(saveButton, &QPushButton::clicked, [=]() {
        // Save display unit setting
        if (pbcRadio->isChecked()) {
            displayUnit = PBC;
        } else if (mpbcRadio->isChecked()) {
            displayUnit = MPBC;
        } else if (mizutsiRadio->isChecked()) {
            displayUnit = MIZUTSI;
        }
        
        // Save theme setting
        bool newThemeSetting = darkRadio->isChecked();
        bool themeChanged = (darkThemeEnabled != newThemeSetting);
        darkThemeEnabled = newThemeSetting;
        
        // Save settings
        saveSettings();
        
        // Apply theme if changed
        if (themeChanged) {
            loadStyleSheet();
        }
        
        // Update display
        updateBalanceDisplay();
        
        settingsDialog->accept();
    });
    
    settingsDialog->setLayout(mainLayout);
    settingsDialog->exec();
    delete settingsDialog;
}

void MainWindow::on_pushButtonRefresh_clicked()
{
    // Update wallet information
    updateWalletInfo();
    
    // Update network status
    updateNetworkStatus();
    
    // Show a temporary message in the status bar
    ui->statusbar->showMessage("Refreshed", 3000);
}

void MainWindow::on_pushButtonAddToAddressBook_clicked()
{
    QString address = ui->lineEditToAddress->text().trimmed();
    
    if (address.isEmpty()) {
        QMessageBox::warning(this, "Add to Address Book", "Please enter an address first.");
        return;
    }
    
    // Check if address is already in address book
    for (const auto& entry : addressBook) {
        if (entry.address == address) {
            QMessageBox::information(this, "Add to Address Book", 
                                    "This address is already in your address book.");
            return;
        }
    }
    
    // Prompt for label
    bool ok;
    QString label = QInputDialog::getText(this, "Add to Address Book", 
                                        "Enter a label for this address:", 
                                        QLineEdit::Normal, "", &ok);
    
    if (ok && !label.isEmpty()) {
        // Add to address book
        AddressBookEntry entry;
        entry.address = address;
        entry.label = label;
        addressBook.append(entry);
        
        // Save address book
        saveAddressBook();
        
        // Update display
        updateAddressBookDisplay();
        
        QMessageBox::information(this, "Add to Address Book", 
                                "Address added to address book successfully.");
    }
}

void MainWindow::on_pushButtonAddAddress_clicked()
{
    // Prompt for address and label
    QDialog dialog(this);
    dialog.setWindowTitle("Add Address");
    
    QVBoxLayout layout(&dialog);
    
    QLabel addressLabel("Address:", &dialog);
    QLineEdit addressEdit(&dialog);
    
    QLabel labelLabel("Label:", &dialog);
    QLineEdit labelEdit(&dialog);
    
    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
    
    layout.addWidget(&addressLabel);
    layout.addWidget(&addressEdit);
    layout.addWidget(&labelLabel);
    layout.addWidget(&labelEdit);
    layout.addWidget(&buttonBox);
    
    connect(&buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(&buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    
    if (dialog.exec() == QDialog::Accepted) {
        QString address = addressEdit.text().trimmed();
        QString label = labelEdit.text().trimmed();
        
        if (address.isEmpty()) {
            QMessageBox::warning(this, "Add Address", "Please enter an address.");
            return;
        }
        
        // Check if address is already in address book
        for (const auto& entry : addressBook) {
            if (entry.address == address) {
                QMessageBox::information(this, "Add Address", 
                                        "This address is already in your address book.");
                return;
            }
        }
        
        // Add to address book
        AddressBookEntry entry;
        entry.address = address;
        entry.label = label;
        addressBook.append(entry);
        
        // Save address book
        saveAddressBook();
        
        // Update display
        updateAddressBookDisplay();
    }
}

void MainWindow::on_pushButtonEditAddress_clicked()
{
    // Get selected address
    QModelIndex index = ui->tableViewAddressBook->currentIndex();
    if (!index.isValid()) {
        QMessageBox::warning(this, "Edit Address", "Please select an address to edit.");
        return;
    }
    
    int row = index.row();
    if (row < 0 || row >= addressBook.size()) {
        return;
    }
    
    // Get current values
    QString currentAddress = addressBook[row].address;
    QString currentLabel = addressBook[row].label;
    
    // Prompt for new label
    QDialog dialog(this);
    dialog.setWindowTitle("Edit Address");
    
    QVBoxLayout layout(&dialog);
    
    QLabel addressLabel("Address:", &dialog);
    QLineEdit addressEdit(&dialog);
    addressEdit.setText(currentAddress);
    addressEdit.setReadOnly(true); // Don't allow changing the address
    
    QLabel labelLabel("Label:", &dialog);
    QLineEdit labelEdit(&dialog);
    labelEdit.setText(currentLabel);
    
    QDialogButtonBox buttonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
    
    layout.addWidget(&addressLabel);
    layout.addWidget(&addressEdit);
    layout.addWidget(&labelLabel);
    layout.addWidget(&labelEdit);
    layout.addWidget(&buttonBox);
    
    connect(&buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(&buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    
    if (dialog.exec() == QDialog::Accepted) {
        QString newLabel = labelEdit.text().trimmed();
        
        // Update address book
        addressBook[row].label = newLabel;
        
        // Save address book
        saveAddressBook();
        
        // Update display
        updateAddressBookDisplay();
    }
}

void MainWindow::on_pushButtonDeleteAddress_clicked()
{
    // Get selected address
    QModelIndex index = ui->tableViewAddressBook->currentIndex();
    if (!index.isValid()) {
        QMessageBox::warning(this, "Delete Address", "Please select an address to delete.");
        return;
    }
    
    int row = index.row();
    if (row < 0 || row >= addressBook.size()) {
        return;
    }
    
    // Confirm deletion
    QString label = addressBook[row].label;
    QString address = addressBook[row].address;
    QString confirmMessage = "Are you sure you want to delete this address?\n\n";
    if (!label.isEmpty()) {
        confirmMessage += "Label: " + label + "\n";
    }
    confirmMessage += "Address: " + address;
    
    QMessageBox::StandardButton reply = QMessageBox::question(this, "Delete Address", 
                                                           confirmMessage, 
                                                           QMessageBox::Yes | QMessageBox::No);
    
    if (reply == QMessageBox::Yes) {
        // Remove from address book
        addressBook.removeAt(row);
        
        // Save address book
        saveAddressBook();
        
        // Update display
        updateAddressBookDisplay();
    }
}

void MainWindow::on_pushButtonUseAddress_clicked()
{
    // Get selected address
    QModelIndex index = ui->tableViewAddressBook->currentIndex();
    if (!index.isValid()) {
        QMessageBox::warning(this, "Use Address", "Please select an address to use.");
        return;
    }
    
    int row = index.row();
    if (row < 0 || row >= addressBook.size()) {
        return;
    }
    
    // Set the address in the send tab
    QString address = addressBook[row].address;
    ui->lineEditToAddress->setText(address);
    
    // Switch to send tab
    ui->tabWidget->setCurrentIndex(1); // Assuming Send tab is at index 1
}

void MainWindow::on_pushButtonNewAddress_clicked()
{
    QString newAddress = walletModel->generateNewAddress();
    updateAddressesDisplay();
    
    // Select the new address
    int index = ui->comboBoxAddresses->findText(newAddress);
    if (index >= 0) {
        ui->comboBoxAddresses->setCurrentIndex(index);
    }
    
    showNotification(tr("New Address"), tr("New address generated: %1").arg(newAddress));
}

void MainWindow::on_pushButtonCopyAddress_clicked()
{
    QString address = ui->lineEditAddress->text();
    if (!address.isEmpty()) {
        QClipboard *clipboard = QApplication::clipboard();
        clipboard->setText(address);
        showNotification(tr("Address Copied"), tr("Address copied to clipboard"));
    }
}

void MainWindow::on_comboBoxAddresses_currentIndexChanged(int index)
{
    if (index >= 0) {
        ui->lineEditAddress->setText(ui->comboBoxAddresses->currentText());
    }
}

void MainWindow::on_pushButtonSend_clicked()
{
    QString fromAddress = ui->comboBoxFromAddress->currentText();
    QString toAddress = ui->lineEditToAddress->text();
    double amount = ui->doubleSpinBoxAmount->value();
    double fee = ui->doubleSpinBoxFee->value();
    QString label = ui->lineEditTransactionLabel->text();
    
    // Basic validation
    if (fromAddress.isEmpty() || toAddress.isEmpty() || amount <= 0) {
        QMessageBox::warning(this, "Error", "Please fill in all fields with valid values.");
        return;
    }
    
    // Convert PBC to Mizutsi
    Mizutsi amountMizutsi = pbcToMizutsi(amount);
    Mizutsi feeMizutsi = pbcToMizutsi(fee);
    
    // Check if we have enough balance
    if (walletModel->getBalance() < (amountMizutsi + feeMizutsi)) {
        QMessageBox::warning(this, "Insufficient Balance", 
                           QString("You don't have enough balance. Current balance: %1").arg(formatPBC(walletModel->getBalance())));
        return;
    }
    
    // In a real implementation, you would create and sign the transaction
    // Transaction tx = walletModel->createTransaction(fromAddress.toStdString(), toAddress.toStdString(), amountMizutsi, feeMizutsi);
    // bool success = walletModel->signTransaction(tx);
    
    // Simulate transaction ID creation
    QString txId = QString("tx%1").arg(QDateTime::currentDateTime().toSecsSinceEpoch());
    
    // Broadcast transaction to network
    bool broadcastSuccess = broadcastTransaction(txId);
    
    if (!broadcastSuccess) {
        QMessageBox::warning(this, "Warning", 
                           "Transaction created but could not be broadcast to the network. " 
                           "It will be broadcast automatically when connection is restored.");
    }
    
    // Save transaction label if provided
    if (!label.isEmpty()) {
        transactionLabels[txId] = label;
        saveTransactionLabels();
    }
    
    // For now, just show a success message
    QMessageBox::information(this, "Success", 
                           QString("Transaction created successfully.\nTransaction ID: %1").arg(txId));
    
    // Clear fields
    ui->lineEditToAddress->clear();
    ui->doubleSpinBoxAmount->setValue(0);
    ui->lineEditTransactionLabel->clear();
    
    // Update wallet info
    updateWalletInfo();
}

bool MainWindow::broadcastTransaction(const QString& txId)
{
    // In a real implementation, this would connect to the Pebicoin network
    // and broadcast the transaction to peers
    
    if (!isConnected) {
        // Can't broadcast if not connected
        return false;
    }
    
    // Simulate network broadcast
    // In a real implementation, this would use sockets or HTTP to send the transaction
    
    // For demonstration purposes, we'll simulate a 90% success rate
    int randomValue = QRandomGenerator::global()->bounded(100);
    bool success = (randomValue < 90);
    
    if (success) {
        qDebug() << "Transaction" << txId << "broadcast successfully";
    } else {
        qDebug() << "Failed to broadcast transaction" << txId;
    }
    
    return success;
}

void MainWindow::on_pushButtonNewAddressReceive_clicked()
{
    // Reuse the same function from overview tab
    on_pushButtonNewAddress_clicked();
}

void MainWindow::on_pushButtonCopyAddressReceive_clicked()
{
    // Get selected address from list view
    QModelIndex index = ui->listViewAddresses->currentIndex();
    if (index.isValid()) {
        QString address = addressesModel->data(index, Qt::DisplayRole).toString();
        QClipboard *clipboard = QApplication::clipboard();
        clipboard->setText(address);
        showNotification(tr("Address Copied"), tr("Address copied to clipboard"));
    } else {
        showNotification(tr("Error"), tr("Please select an address to copy"), true);
    }
}