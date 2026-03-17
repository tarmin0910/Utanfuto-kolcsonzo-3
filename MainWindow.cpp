#include "MainWindow.h"
#include "TrailerCard.h"
#include "RentDialog.h"
#include "ReservationDetailsDialog.h"

#include <QAction>
#include <QApplication>
#include <QDateTime>
#include <QFrame>
#include <QGridLayout>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QListWidgetItem>
#include <QLocale>
#include <QColor>
#include <QMessageBox>
#include <QPushButton>
#include <QScrollArea>
#include <QStackedWidget>
#include <QStyle>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QToolBar>
#include <QVBoxLayout>

static void styleTab(QPushButton* b, bool active){
    b->setCheckable(true);
    b->setChecked(active);
    b->setStyleSheet(QString(
                         "QPushButton{padding:6px 12px;border-radius:10px;font-weight:700;"
                         "background:%1;color:%2}"
                         "QPushButton:hover{background:%3}")
                         .arg(active ? "#0a2233" : "#f1f3f5")
                         .arg(active ? "white" : "#0a2233")
                         .arg(active ? "#0a2233" : "#e9ecef"));
}

static QFrame* roundedPanel(const QString& name){
    auto* panel = new QFrame;
    panel->setObjectName(name);
    panel->setStyleSheet(
        "QFrame{background:#ffffff;border:1px solid #e9ecef;border-radius:16px;}"
        );
    return panel;
}

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), m_calendarMonth(QDate::currentDate())
{
    setWindowTitle(QStringLiteral("Trailo"));
    applyGlobalStyle();

    m_rootPages = new QStackedWidget(this);
    m_rootPages->addWidget(buildLoginPage());
    m_rootPages->addWidget(buildAppShell());
    setCentralWidget(m_rootPages);

    fillProfileFields(DatabaseManager::instance().loadCustomerProfile());
    refreshOrders();
    refreshCalendar();

    showLogin();
}

void MainWindow::applyGlobalStyle()
{
    this->setStyleSheet("QMainWindow{background:#f4f6f8;}");
    qApp->setStyleSheet(qApp->styleSheet() +
                        "\nQLineEdit{padding:10px 12px;border-radius:12px;border:1px solid #e9ecef;background:#fff;}"
                        "\nQLineEdit:focus{border:1px solid #94a3b8;}"
                        "\nQPushButton#primary{background:#0f766e;color:#fff;border:none;border-radius:12px;padding:10px 14px;font-weight:800;}"
                        "\nQPushButton#primary:hover{background:#0a5e58;}"
                        "\nQPushButton#link{background:transparent;border:none;color:#2563eb;font-weight:800;}"
                        "\nQPushButton#link:hover{color:#1d4ed8;}"
                        "\nQLabel#title{font-size:34px;font-weight:900;color:#0a2233;}"
                        "\nQLabel#error{color:#dc2626;font-weight:800;}"
                        "\nQListWidget{border:1px solid #eef2f7;border-radius:12px;padding:6px;background:#fff;}"
                        "\nQListWidget::item{padding:10px;border-radius:10px;}"
                        "\nQListWidget::item:selected{background:#0a2233;color:white;}"
                        "\nQTableWidget{background:#fff;border:1px solid #eef2f7;border-radius:12px;gridline-color:#eef2f7;}"
                        "\nQHeaderView::section{background:#f8fafc;border:none;padding:8px;font-weight:800;color:#0a2233;}"
                        );
}

QWidget* MainWindow::buildLoginPage()
{
    auto* outer = new QWidget;
    auto* lay = new QVBoxLayout(outer);
    lay->setContentsMargins(24, 24, 24, 24);
    lay->setSpacing(14);

    lay->addStretch();

    auto* panel = roundedPanel("loginPanel");
    auto* pLay = new QVBoxLayout(panel);
    pLay->setContentsMargins(28, 22, 28, 22);
    pLay->setSpacing(14);

    auto* top = new QHBoxLayout;
    auto* brand = new QLabel(QStringLiteral("<b style='font-size:18px'>Trailo</b>"));
    top->addWidget(brand);
    top->addStretch();
    pLay->addLayout(top);

    auto* title = new QLabel(QStringLiteral("Jelentkezz be"));
    title->setObjectName("title");
    title->setAlignment(Qt::AlignHCenter);
    pLay->addSpacing(8);
    pLay->addWidget(title);

    m_loginUser = new QLineEdit;
    m_loginUser->setPlaceholderText(QStringLiteral("Felhasználónév (pl. adam)"));

    m_loginPass = new QLineEdit;
    m_loginPass->setPlaceholderText(QStringLiteral("Jelszó"));
    m_loginPass->setEchoMode(QLineEdit::Password);

    auto* loginBtn = new QPushButton(QStringLiteral("Bejelentkezés"));
    loginBtn->setObjectName("primary");

    m_loginError = new QLabel;
    m_loginError->setObjectName("error");

    pLay->addWidget(m_loginUser);
    pLay->addWidget(m_loginPass);
    pLay->addWidget(loginBtn);
    pLay->addWidget(m_loginError);

    auto* links = new QHBoxLayout;
    auto* left = new QHBoxLayout;
    left->addWidget(new QLabel(QStringLiteral("Még nem regisztrált?")));
    auto* reg = new QPushButton(QStringLiteral("Regisztráció"));
    reg->setObjectName("link");
    left->addWidget(reg);
    left->addStretch();

    auto* forgot = new QPushButton(QStringLiteral("Elfelejtetted a jelszavad?"));
    forgot->setObjectName("link");

    links->addLayout(left);
    links->addStretch();
    links->addWidget(forgot);
    pLay->addLayout(links);

    lay->addWidget(panel);
    lay->addStretch();

    connect(loginBtn, &QPushButton::clicked, this, &MainWindow::onLoginClicked);
    connect(m_loginPass, &QLineEdit::returnPressed, this, &MainWindow::onLoginClicked);
    connect(reg, &QPushButton::clicked, this, &MainWindow::onRegisterClicked);
    connect(forgot, &QPushButton::clicked, this, &MainWindow::onForgotClicked);

    return outer;
}

QWidget* MainWindow::buildAppShell()
{
    auto *tb = addToolBar("Top");
    tb->setMovable(false);
    tb->setIconSize({20,20});
    tb->setStyleSheet("QToolBar{background:#ffffff;border:none;} ");

    auto *logo = new QLabel(QStringLiteral("<b style='font-size:18px'>Trailo</b>"));

    m_tabTrailer  = new QPushButton(QStringLiteral("Utánfutók"));
    m_tabCalendar = new QPushButton(QStringLiteral("Naptár"));
    m_tabOrders   = new QPushButton(QStringLiteral("Rendelések"));

    styleTab(m_tabTrailer, true);
    styleTab(m_tabCalendar, false);
    styleTab(m_tabOrders, false);

    connect(m_tabTrailer,  &QPushButton::clicked, this, &MainWindow::showTrailer);
    connect(m_tabCalendar, &QPushButton::clicked, this, &MainWindow::showCalendar);
    connect(m_tabOrders,   &QPushButton::clicked, this, &MainWindow::showOrders);

    tb->addWidget(logo);
    QWidget* spacer12 = new QWidget; spacer12->setFixedWidth(12); tb->addWidget(spacer12);
    tb->addWidget(m_tabTrailer);
    tb->addWidget(m_tabCalendar);
    tb->addWidget(m_tabOrders);

    QWidget* spring = new QWidget; spring->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    tb->addWidget(spring);

    m_profileAction  = tb->addAction(style()->standardIcon(QStyle::SP_ComputerIcon), QStringLiteral("Profil"));
    m_settingsAction = tb->addAction(style()->standardIcon(QStyle::SP_FileDialogDetailedView), QStringLiteral("Beállítások"));

    connect(m_profileAction, &QAction::triggered, this, &MainWindow::showProfile);
    connect(m_settingsAction, &QAction::triggered, this, [this](){
        QMessageBox::information(this, QStringLiteral("Információ"), QStringLiteral("A foglalások mentése SQLite adatbázisba történik a program mappájában."));
    });

    auto* shell = new QWidget;
    auto* shellLay = new QVBoxLayout(shell);
    shellLay->setContentsMargins(0,0,0,0);
    shellLay->setSpacing(0);

    m_pages = new QStackedWidget(shell);
    m_pages->addWidget(buildTrailerPage());
    m_pages->addWidget(buildCalendarPage());
    m_pages->addWidget(buildOrdersPage());
    m_pages->addWidget(buildProfilePage());
    shellLay->addWidget(m_pages);

    return shell;
}

QWidget* MainWindow::buildTrailerPage() {
    auto *outer = new QWidget;
    outer->setStyleSheet("QWidget{background:transparent;}");

    auto *outerLay = new QVBoxLayout(outer);
    outerLay->setContentsMargins(16,12,16,16);
    outerLay->setSpacing(12);

    auto *panel = roundedPanel("panel");
    auto *root = new QVBoxLayout(panel);
    root->setContentsMargins(16,16,16,16);
    root->setSpacing(16);

    auto *title = new QLabel(QStringLiteral("<b style='font-size:18px'>Utánfutók</b>"));
    auto* subtitle = new QLabel(QStringLiteral("Válassz típust, majd add meg a bérlési időszakot a felugró ablakban."));
    subtitle->setStyleSheet("color:#475569;");
    root->addWidget(title);
    root->addWidget(subtitle);

    auto *grid = new QGridLayout;
    grid->setSpacing(16);

    auto *t1 = new TrailerCard("Nyitott, oldalfalas utánfutó", "5000", ":/images/open.jpg");
    auto *t2 = new TrailerCard("Ponyvás", "7000", ":/images/ponyva.png");
    auto *t3 = new TrailerCard("Motorszállító", "8000", ":/images/motor.jpg");
    auto *t4 = new TrailerCard("Autószállító", "12000", ":/images/auto.png");
    auto *t5 = new TrailerCard("Zárt dobozos / autó- és áruszállító", "18000", ":/images/box.jpg");
    auto *t6 = new TrailerCard("Billenőplatós utánfutó", "7500", ":/images/billeno.png");

    grid->addWidget(t1,0,0);
    grid->addWidget(t2,0,1);
    grid->addWidget(t3,1,0);
    grid->addWidget(t4,1,1);
    grid->addWidget(t5,2,0);
    grid->addWidget(t6,2,1);

    root->addLayout(grid);
    root->addStretch();

    for (auto* card : {t1,t2,t3,t4,t5,t6}) {
        connect(card, &TrailerCard::orderClicked, this, &MainWindow::orderTrailer);
    }

    QScrollArea* scroll = new QScrollArea;
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    scroll->setWidget(panel);

    outerLay->addWidget(scroll);

    return outer;
}


QWidget* MainWindow::buildCalendarPage() {
    auto *outer = new QWidget;
    auto *outerLay = new QVBoxLayout(outer);
    outerLay->setContentsMargins(16,12,16,16);
    outerLay->setSpacing(12);

    auto *panel = roundedPanel("panel");
    auto *root = new QVBoxLayout(panel);
    root->setContentsMargins(16,16,16,16);
    root->setSpacing(12);

    auto* header = new QHBoxLayout;
    auto* prevBtn = new QPushButton(QStringLiteral("◀ Előző hónap"));
    auto* nextBtn = new QPushButton(QStringLiteral("Következő hónap ▶"));
    prevBtn->setStyleSheet("QPushButton{padding:8px 12px;border-radius:10px;background:#f1f5f9;font-weight:700;} QPushButton:hover{background:#e2e8f0;}");
    nextBtn->setStyleSheet(prevBtn->styleSheet());
    m_calendarMonthLabel = new QLabel;
    m_calendarMonthLabel->setStyleSheet("font-size:18px;font-weight:900;color:#0a2233;");
    header->addWidget(prevBtn);
    header->addStretch();
    header->addWidget(m_calendarMonthLabel);
    header->addStretch();
    header->addWidget(nextBtn);
    root->addLayout(header);

    m_calendarLegendLabel = new QLabel(QStringLiteral("Zöld: van foglalás • Piros: egy adott napon az összes 3 db elfogyott • A cella tooltipjében ott a név és az időszak."));
    m_calendarLegendLabel->setStyleSheet("color:#475569;");
    root->addWidget(m_calendarLegendLabel);

    m_calendarTable = new QTableWidget;
    m_calendarTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_calendarTable->setSelectionMode(QAbstractItemView::NoSelection);
    m_calendarTable->verticalHeader()->setVisible(true);
    m_calendarTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    m_calendarTable->horizontalHeader()->setDefaultSectionSize(34);
    m_calendarTable->verticalHeader()->setDefaultSectionSize(54);
    root->addWidget(m_calendarTable, 1);

    connect(prevBtn, &QPushButton::clicked, this, &MainWindow::showPreviousMonth);
    connect(nextBtn, &QPushButton::clicked, this, &MainWindow::showNextMonth);

    outerLay->addWidget(panel);
    return outer;
}

QWidget* MainWindow::buildOrdersPage() {
    auto *outer = new QWidget;
    auto *outerLay = new QVBoxLayout(outer);
    outerLay->setContentsMargins(16,12,16,16);
    outerLay->setSpacing(12);

    auto *panel = roundedPanel("panel");
    auto *root = new QVBoxLayout(panel);
    root->setContentsMargins(16,16,16,16);
    root->setSpacing(12);

    auto *title = new QLabel(QStringLiteral("<b style='font-size:18px'>Rendelések</b>"));
    root->addWidget(title);

    auto *hint = new QLabel(QStringLiteral("Itt jelennek meg az elmentett foglalások. Dupla kattintással megnyithatod a részleteket és törölheted is őket."));
    hint->setStyleSheet("color:#475569;");
    root->addWidget(hint);

    m_ordersList = new QListWidget;
    m_ordersList->setMinimumHeight(320);
    root->addWidget(m_ordersList, 1);

    auto *row = new QHBoxLayout;
    row->addStretch();
    auto *refreshBtn = new QPushButton(QStringLiteral("Frissítés"));
    refreshBtn->setStyleSheet("QPushButton{padding:8px 12px;border-radius:10px;background:#f1f3f5;font-weight:800;} QPushButton:hover{background:#e9ecef;}");
    row->addWidget(refreshBtn);
    root->addLayout(row);

    connect(refreshBtn, &QPushButton::clicked, this, &MainWindow::refreshOrders);
    connect(m_ordersList, &QListWidget::itemDoubleClicked, this, &MainWindow::onOrderItemDoubleClicked);

    outerLay->addWidget(panel);
    return outer;
}

QWidget* MainWindow::buildProfilePage()
{
    auto *outer = new QWidget;
    auto *outerLay = new QVBoxLayout(outer);
    outerLay->setContentsMargins(16,12,16,16);
    outerLay->setSpacing(12);

    auto *panel = roundedPanel("panel");
    auto *root = new QVBoxLayout(panel);
    root->setContentsMargins(16,16,16,16);
    root->setSpacing(14);

    auto *title = new QLabel(QStringLiteral("<b style='font-size:18px'>Profil / Számlázási adatok</b>"));
    root->addWidget(title);

    auto *personal = roundedPanel("personal");
    personal->setStyleSheet("QFrame{background:#ffffff;border:1px solid #eef2f7;border-radius:14px;}");
    auto *pLay = new QGridLayout(personal);
    pLay->setContentsMargins(14,14,14,14);
    pLay->setHorizontalSpacing(12);
    pLay->setVerticalSpacing(10);

    m_profName  = new QLineEdit; m_profName->setPlaceholderText("Teljes név");
    m_profEmail = new QLineEdit; m_profEmail->setPlaceholderText("Email");
    m_profPhone = new QLineEdit; m_profPhone->setPlaceholderText("Telefonszám");

    pLay->addWidget(new QLabel("Teljes név"), 0,0); pLay->addWidget(m_profName, 0,1);
    pLay->addWidget(new QLabel("Email"), 1,0); pLay->addWidget(m_profEmail, 1,1);
    pLay->addWidget(new QLabel("Telefon"), 2,0); pLay->addWidget(m_profPhone, 2,1);
    root->addWidget(personal);

    auto *billing = roundedPanel("billing");
    billing->setStyleSheet("QFrame{background:#ffffff;border:1px solid #eef2f7;border-radius:14px;}");
    auto *bLay = new QGridLayout(billing);
    bLay->setContentsMargins(14,14,14,14);
    bLay->setHorizontalSpacing(12);
    bLay->setVerticalSpacing(10);

    m_profBillingName    = new QLineEdit; m_profBillingName->setPlaceholderText("Számlázási név");
    m_profBillingCountry = new QLineEdit; m_profBillingCountry->setPlaceholderText("Ország");
    m_profBillingZip     = new QLineEdit; m_profBillingZip->setPlaceholderText("Irányítószám");
    m_profBillingCity    = new QLineEdit; m_profBillingCity->setPlaceholderText("Város");
    m_profBillingStreet  = new QLineEdit; m_profBillingStreet->setPlaceholderText("Utca" );
    m_profBillingHouse   = new QLineEdit; m_profBillingHouse->setPlaceholderText("Házszám" );

    bLay->addWidget(new QLabel("Számlázási név"), 0,0); bLay->addWidget(m_profBillingName, 0,1,1,3);
    bLay->addWidget(new QLabel("Ország"), 1,0); bLay->addWidget(m_profBillingCountry, 1,1);
    bLay->addWidget(new QLabel("Irányítószám"), 1,2); bLay->addWidget(m_profBillingZip, 1,3);
    bLay->addWidget(new QLabel("Város"), 2,0); bLay->addWidget(m_profBillingCity, 2,1);
    bLay->addWidget(new QLabel("Utca"), 2,2); bLay->addWidget(m_profBillingStreet, 2,3);
    bLay->addWidget(new QLabel("Házszám"), 3,0); bLay->addWidget(m_profBillingHouse, 3,1);
    root->addWidget(billing);

    auto *extra = roundedPanel("extra");
    extra->setStyleSheet("QFrame{background:#ffffff;border:1px solid #eef2f7;border-radius:14px;}");
    auto *eLay = new QGridLayout(extra);
    eLay->setContentsMargins(14,14,14,14);
    eLay->setHorizontalSpacing(12);
    eLay->setVerticalSpacing(10);

    m_profCompany       = new QLineEdit; m_profCompany->setPlaceholderText("Cégnév (opcionális)");
    m_profTaxNumber     = new QLineEdit; m_profTaxNumber->setPlaceholderText("Adószám (opcionális)");
    m_profIdNumber      = new QLineEdit; m_profIdNumber->setPlaceholderText("Személyi igazolvány szám (opcionális)");
    m_profDriverLicense = new QLineEdit; m_profDriverLicense->setPlaceholderText("Jogosítvány szám (opcionális)");

    eLay->addWidget(new QLabel("Cégnév"), 0,0); eLay->addWidget(m_profCompany, 0,1,1,3);
    eLay->addWidget(new QLabel("Adószám"), 1,0); eLay->addWidget(m_profTaxNumber, 1,1);
    eLay->addWidget(new QLabel("Személyi"), 2,0); eLay->addWidget(m_profIdNumber, 2,1);
    eLay->addWidget(new QLabel("Jogosítvány"), 2,2); eLay->addWidget(m_profDriverLicense, 2,3);
    root->addWidget(extra);

    auto *btnRow = new QHBoxLayout;
    btnRow->addStretch();
    auto *save = new QPushButton("Mentés");
    save->setObjectName("primary");
    btnRow->addWidget(save);
    root->addLayout(btnRow);

    outerLay->addWidget(panel);
    outerLay->addStretch();

    connect(save, &QPushButton::clicked, this, &MainWindow::onSaveProfileClicked);
    return outer;
}

void MainWindow::showLogin()
{
    if (auto* tb = findChild<QToolBar*>()) tb->setVisible(false);
    m_rootPages->setCurrentIndex(0);
    setWindowTitle("Trailo – Bejelentkezés");
}

void MainWindow::showApp()
{
    if (auto* tb = findChild<QToolBar*>()) tb->setVisible(true);
    m_rootPages->setCurrentIndex(1);
    showTrailer();
}

void MainWindow::showProfile()
{
    styleTab(m_tabTrailer, false);
    styleTab(m_tabCalendar, false);
    styleTab(m_tabOrders, false);
    m_pages->setCurrentIndex(3);
    setWindowTitle("Trailo – Profil");
    fillProfileFields(DatabaseManager::instance().loadCustomerProfile());
}

bool MainWindow::validateLogin(const QString& user, const QString& pass)
{
    return !user.trimmed().isEmpty() && !pass.isEmpty();
}

void MainWindow::setTabActive(int tabIndex){
    styleTab(m_tabTrailer,  tabIndex == 0);
    styleTab(m_tabCalendar, tabIndex == 1);
    styleTab(m_tabOrders,   tabIndex == 2);
}

void MainWindow::showTrailer(){
    setTabActive(0);
    m_pages->setCurrentIndex(0);
    setWindowTitle("Trailo – Utánfutók");
}

void MainWindow::showCalendar(){
    setTabActive(1);
    m_pages->setCurrentIndex(1);
    setWindowTitle("Trailo – Naptár");
    refreshCalendar();
}

void MainWindow::showOrders(){
    setTabActive(2);
    m_pages->setCurrentIndex(2);
    setWindowTitle("Trailo – Rendelések");
    refreshOrders();
}

bool MainWindow::ensureProfileReadyForReservation()
{
    const auto profile = collectProfileFromFields();
    if (profile.fullName.trimmed().isEmpty() || profile.email.trimmed().isEmpty() || profile.phone.trimmed().isEmpty()) {
        QMessageBox::warning(this, QStringLiteral("Hiányzó profil adatok"),
                             QStringLiteral("Foglalás előtt töltsd ki és mentsd el legalább a teljes nevet, email címet és telefonszámot a Profil oldalon."));
        showProfile();
        return false;
    }
    if (!DatabaseManager::instance().saveCustomerProfile(profile)) {
        QMessageBox::warning(this, QStringLiteral("Hiba"), QStringLiteral("A profil mentése nem sikerült, ezért a foglalás sem indulhat el."));
        return false;
    }
    return true;
}

CustomerProfile MainWindow::collectProfileFromFields() const
{
    CustomerProfile profile;
    profile.id = 1;
    if (m_profName) profile.fullName = m_profName->text().trimmed();
    if (m_profEmail) profile.email = m_profEmail->text().trimmed();
    if (m_profPhone) profile.phone = m_profPhone->text().trimmed();
    if (m_profBillingName) profile.billingName = m_profBillingName->text().trimmed();
    if (m_profBillingCountry) profile.country = m_profBillingCountry->text().trimmed();
    if (m_profBillingZip) profile.postalCode = m_profBillingZip->text().trimmed();
    if (m_profBillingCity) profile.city = m_profBillingCity->text().trimmed();
    if (m_profBillingStreet) profile.street = m_profBillingStreet->text().trimmed();
    if (m_profBillingHouse) profile.houseNumber = m_profBillingHouse->text().trimmed();
    if (m_profCompany) profile.companyName = m_profCompany->text().trimmed();
    if (m_profTaxNumber) profile.taxNumber = m_profTaxNumber->text().trimmed();
    if (m_profIdNumber) profile.idNumber = m_profIdNumber->text().trimmed();
    if (m_profDriverLicense) profile.licenseNumber = m_profDriverLicense->text().trimmed();
    return profile;
}

void MainWindow::fillProfileFields(const CustomerProfile& profile)
{
    if (m_profName) m_profName->setText(profile.fullName.isEmpty() ? m_user : profile.fullName);
    if (m_profEmail) m_profEmail->setText(profile.email.isEmpty() ? m_email : profile.email);
    if (m_profPhone) m_profPhone->setText(profile.phone.isEmpty() ? m_phone : profile.phone);
    if (m_profBillingName) m_profBillingName->setText(profile.billingName);
    if (m_profBillingCountry) m_profBillingCountry->setText(profile.country);
    if (m_profBillingZip) m_profBillingZip->setText(profile.postalCode);
    if (m_profBillingCity) m_profBillingCity->setText(profile.city);
    if (m_profBillingStreet) m_profBillingStreet->setText(profile.street);
    if (m_profBillingHouse) m_profBillingHouse->setText(profile.houseNumber);
    if (m_profCompany) m_profCompany->setText(profile.companyName);
    if (m_profTaxNumber) m_profTaxNumber->setText(profile.taxNumber);
    if (m_profIdNumber) m_profIdNumber->setText(profile.idNumber);
    if (m_profDriverLicense) m_profDriverLicense->setText(profile.licenseNumber);
}

void MainWindow::orderTrailer(const QString& title) {
    if (!ensureProfileReadyForReservation()) return;

    const auto trailer = DatabaseManager::instance().trailerByName(title);
    if (trailer.id <= 0) {
        QMessageBox::warning(this, QStringLiteral("Hiba"), QStringLiteral("Nem található az utánfutó az adatbázisban."));
        return;
    }

    RentDialog dialog(trailer, DatabaseManager::instance().loadCustomerProfile(), this);
    if (dialog.exec() == QDialog::Accepted) {
        refreshOrders();
        refreshCalendar();
        showOrders();
        QMessageBox::information(this, QStringLiteral("Sikeres foglalás"), QStringLiteral("A foglalás el lett mentve."));
    }
}

void MainWindow::refreshOrders()
{
    if (!m_ordersList) return;
    m_ordersList->clear();

    const auto list = DatabaseManager::instance().reservations();
    for (const auto& item : list) {
        auto* row = new QListWidgetItem(QStringLiteral("%1\n%2 • %3 → %4 • %5 Ft")
                                            .arg(item.customerName,
                                                 item.trailerName,
                                                 item.startDate.toString("yyyy-MM-dd"),
                                                 item.endDate.toString("yyyy-MM-dd"),
                                                 QString::number(item.totalPrice)));
        row->setData(Qt::UserRole, item.id);
        row->setToolTip(QStringLiteral("Dupla kattintással megnyitható\nNapok: %1").arg(item.days));
        m_ordersList->addItem(row);
    }
}

void MainWindow::refreshCalendar()
{
    if (!m_calendarTable || !m_calendarMonthLabel) return;
    m_calendarMonthLabel->setText(QLocale(QLocale::Hungarian).toString(m_calendarMonth, "yyyy. MMMM"));
    populateCalendarTable();
}

void MainWindow::populateCalendarTable()
{
    const auto trailers = DatabaseManager::instance().trailers();
    const auto monthReservations = DatabaseManager::instance().reservationsForMonth(m_calendarMonth);
    const QDate first(m_calendarMonth.year(), m_calendarMonth.month(), 1);
    const int daysInMonth = first.daysInMonth();

    m_calendarTable->clear();
    m_calendarTable->setRowCount(trailers.size());
    m_calendarTable->setColumnCount(daysInMonth);

    QStringList headers;
    for (int day = 1; day <= daysInMonth; ++day) {
        headers << QString::number(day);
    }
    m_calendarTable->setHorizontalHeaderLabels(headers);

    for (int row = 0; row < trailers.size(); ++row) {
        m_calendarTable->setVerticalHeaderItem(row, new QTableWidgetItem(trailers[row].name));
        for (int col = 0; col < daysInMonth; ++col) {
            auto* item = new QTableWidgetItem;
            item->setTextAlignment(Qt::AlignCenter);
            m_calendarTable->setItem(row, col, item);
        }
    }

    for (int row = 0; row < trailers.size(); ++row) {
        const auto& trailer = trailers[row];
        for (int day = 1; day <= daysInMonth; ++day) {
            const QDate currentDay(m_calendarMonth.year(), m_calendarMonth.month(), day);
            QList<ReservationInfo> matches;
            for (const auto& res : monthReservations) {
                if (res.trailerId != trailer.id) continue;
                if (res.startDate <= currentDay && res.endDate >= currentDay) matches.append(res);
            }

            auto* cell = m_calendarTable->item(row, day - 1);
            if (!cell) continue;

            if (!matches.isEmpty()) {
                const bool full = matches.size() >= trailer.stock;
                cell->setBackground(full ? QColor("#fecaca") : QColor("#bbf7d0"));
                cell->setText(QString::number(matches.size()));
                QStringList tooltipLines;
                for (const auto& res : matches) {
                    tooltipLines << QStringLiteral("%1 (%2 → %3)")
                                        .arg(res.customerName,
                                             res.startDate.toString("yyyy-MM-dd"),
                                             res.endDate.toString("yyyy-MM-dd"));
                }
                cell->setToolTip(tooltipLines.join("\n"));
            } else {
                cell->setBackground(QColor("#ffffff"));
                cell->setText("");
                cell->setToolTip(QString());
            }
        }
    }

    m_calendarTable->resizeRowsToContents();
}

void MainWindow::openReservationDetails(const ReservationInfo& reservation)
{
    auto* dialog = new ReservationDetailsDialog(reservation, this);
    connect(dialog, &ReservationDetailsDialog::reservationDeleted, this, [this](int){
        refreshOrders();
        refreshCalendar();
    });
    dialog->exec();
    dialog->deleteLater();
}

void MainWindow::onLoginClicked()
{
    const QString u = m_loginUser ? m_loginUser->text().trimmed() : QString();
    const QString p = m_loginPass ? m_loginPass->text() : QString();

    if (!validateLogin(u, p)) {
        if (m_loginError) m_loginError->setText("Kérlek add meg a felhasználónevet és a jelszót.");
        return;
    }

    m_user = u;
    if (m_email.isEmpty()) m_email = u + "@example.com";
    if (m_phone.isEmpty()) m_phone = "+36 30 123 4567";
    if (m_loginError) m_loginError->setText("");

    fillProfileFields(DatabaseManager::instance().loadCustomerProfile());
    showApp();
}

void MainWindow::onRegisterClicked()
{
    QMessageBox::information(this, "Regisztráció (PoC)",
                             "Regisztráció továbbra is PoC, de a foglalási és profilkezelési részek már működnek.");
}

void MainWindow::onForgotClicked()
{
    QMessageBox::information(this, "Jelszó emlékeztető (PoC)",
                             "Az elfelejtett jelszó folyamat ennél a verziónál még nincs kidolgozva.");
}

void MainWindow::onSaveProfileClicked()
{
    const auto profile = collectProfileFromFields();
    if (profile.fullName.isEmpty() || profile.email.isEmpty() || profile.phone.isEmpty()) {
        QMessageBox::warning(this, QStringLiteral("Hiányzó adatok"), QStringLiteral("A teljes név, email és telefon mezők kitöltése kötelező."));
        return;
    }

    if (!DatabaseManager::instance().saveCustomerProfile(profile)) {
        QMessageBox::warning(this, QStringLiteral("Hiba"), QStringLiteral("Nem sikerült elmenteni a profil adatokat."));
        return;
    }

    m_email = profile.email;
    m_phone = profile.phone;
    QMessageBox::information(this, QStringLiteral("Mentve"), QStringLiteral("A profil adatok el lettek mentve az adatbázisba."));
}

void MainWindow::onOrderItemDoubleClicked(QListWidgetItem* item)
{
    if (!item) return;
    const int reservationId = item->data(Qt::UserRole).toInt();
    const auto list = DatabaseManager::instance().reservations();
    for (const auto& reservation : list) {
        if (reservation.id == reservationId) {
            openReservationDetails(reservation);
            return;
        }
    }
}

void MainWindow::showPreviousMonth()
{
    m_calendarMonth = m_calendarMonth.addMonths(-1);
    refreshCalendar();
}

void MainWindow::showNextMonth()
{
    m_calendarMonth = m_calendarMonth.addMonths(1);
    refreshCalendar();
}
