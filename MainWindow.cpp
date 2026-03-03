#include "MainWindow.h"
#include "TrailerCard.h"

#include <QToolBar>
#include <QAction>
#include <QStackedWidget>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QMessageBox>
#include <QFrame>
#include <QStyle>
#include <QApplication>
#include <QListWidget>
#include <QDateTime>

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
    : QMainWindow(parent)
{
    setWindowTitle(QStringLiteral("Trailo"));
    applyGlobalStyle();

    m_rootPages = new QStackedWidget(this);
    m_rootPages->addWidget(buildLoginPage()); // 0
    m_rootPages->addWidget(buildAppShell());  // 1
    setCentralWidget(m_rootPages);

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
                        "\nQListWidget{border:1px solid #eef2f7;border-radius:12px;padding:6px;}"
                        "\nQListWidget::item{padding:10px;border-radius:10px;}"
                        "\nQListWidget::item:selected{background:#0a2233;color:white;}"
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

    m_tabTrailer  = new QPushButton(QStringLiteral("Trailer"));
    m_tabCalendar = new QPushButton(QStringLiteral("Calendar"));
    m_tabOrders   = new QPushButton(QStringLiteral("Orders"));

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
    connect(m_settingsAction, &QAction::triggered, this, [](){
        QMessageBox::information(nullptr, "PoC", "Beállítások (PoC)");
    });

    auto* shell = new QWidget;
    auto* shellLay = new QVBoxLayout(shell);
    shellLay->setContentsMargins(0,0,0,0);
    shellLay->setSpacing(0);

    m_pages = new QStackedWidget(shell);
    m_pages->addWidget(buildTrailerPage());  // 0
    m_pages->addWidget(buildCalendarPage()); // 1
    m_pages->addWidget(buildOrdersPage());   // 2
    m_pages->addWidget(buildProfilePage());  // 3
    shellLay->addWidget(m_pages);

    return shell;
}

QWidget* MainWindow::buildTrailerPage() {
    auto *outer = new QWidget; outer->setStyleSheet("QWidget{background:transparent;}");
    auto *outerLay = new QVBoxLayout(outer); outerLay->setContentsMargins(16,12,16,16); outerLay->setSpacing(12);

    auto *panel = roundedPanel("panel");
    auto *root = new QVBoxLayout(panel); root->setContentsMargins(16,16,16,16); root->setSpacing(16);

    auto *searchWrap = new QFrame; searchWrap->setObjectName("search");
    searchWrap->setStyleSheet("QFrame#search{background:#f6f7f8;border:1px solid #e9ecef;border-radius:14px;}");
    auto *sLay = new QHBoxLayout(searchWrap); sLay->setContentsMargins(12,6,12,6);
    auto *ico = new QLabel(QStringLiteral("🔍"));
    auto *search = new QLineEdit; search->setPlaceholderText(QStringLiteral("Kategóriák")); search->setFrame(false);
    sLay->addWidget(ico); sLay->addSpacing(6); sLay->addWidget(search);

    root->addWidget(searchWrap);

    auto *cards = new QHBoxLayout; cards->setSpacing(20);
    auto *openTrailer = new TrailerCard(QStringLiteral("Nyitott Rakomány Utánfutó"), QStringLiteral("9 000"));
    auto *closedTrailer = new TrailerCard(QStringLiteral("Zárt Utánfutó"), QStringLiteral("11 500"));
    cards->addWidget(openTrailer, 1);
    cards->addWidget(closedTrailer, 1);
    root->addLayout(cards);

    outerLay->addWidget(panel);
    outerLay->addStretch();

    connect(openTrailer, &TrailerCard::orderClicked, this, &MainWindow::orderTrailer);
    connect(closedTrailer, &TrailerCard::orderClicked, this, &MainWindow::orderTrailer);
    return outer;
}

QWidget* MainWindow::buildCalendarPage() {
    auto *outer = new QWidget; outer->setStyleSheet("QWidget{background:transparent;}");
    auto *outerLay = new QVBoxLayout(outer); outerLay->setContentsMargins(16,12,16,16); outerLay->setSpacing(12);

    auto *panel = roundedPanel("panel");
    auto *root = new QVBoxLayout(panel); root->setContentsMargins(16,16,16,16); root->setSpacing(16);

    root->addWidget(new QLabel(QStringLiteral("<b>Calendar</b> (PoC)\nItt lesz a foglaltsági naptár.")));
    outerLay->addWidget(panel);
    outerLay->addStretch();
    return outer;
}

QWidget* MainWindow::buildOrdersPage() {
    auto *outer = new QWidget; outer->setStyleSheet("QWidget{background:transparent;}");
    auto *outerLay = new QVBoxLayout(outer); outerLay->setContentsMargins(16,12,16,16); outerLay->setSpacing(12);

    auto *panel = roundedPanel("panel");
    auto *root = new QVBoxLayout(panel); root->setContentsMargins(16,16,16,16); root->setSpacing(12);

    auto *title = new QLabel(QStringLiteral("<b style='font-size:18px'>Orders</b>"));
    root->addWidget(title);

    auto *hint = new QLabel(QStringLiteral("Itt jelennek meg a leadott rendelések. (PoC)"));
    hint->setStyleSheet("color:#475569;");
    root->addWidget(hint);

    m_ordersList = new QListWidget;
    m_ordersList->setMinimumHeight(320);
    root->addWidget(m_ordersList, 1);

    auto *row = new QHBoxLayout;
    row->addStretch();

    auto *clearBtn = new QPushButton(QStringLiteral("Lista ürítése"));
    clearBtn->setStyleSheet("QPushButton{padding:8px 12px;border-radius:10px;background:#f1f3f5;font-weight:800;}"
                            "QPushButton:hover{background:#e9ecef;}");
    row->addWidget(clearBtn);
    root->addLayout(row);

    connect(clearBtn, &QPushButton::clicked, this, [this]{
        if (m_ordersList) m_ordersList->clear();
    });

    outerLay->addWidget(panel);
    outerLay->addStretch();
    return outer;
}

QWidget* MainWindow::buildProfilePage()
{
    auto *outer = new QWidget; outer->setStyleSheet("QWidget{background:transparent;}");
    auto *outerLay = new QVBoxLayout(outer); outerLay->setContentsMargins(16,12,16,16); outerLay->setSpacing(12);

    auto *panel = roundedPanel("panel");
    auto *root = new QVBoxLayout(panel); root->setContentsMargins(16,16,16,16); root->setSpacing(14);

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

    pLay->addWidget(new QLabel("Teljes név"), 0,0);
    pLay->addWidget(m_profName, 0,1);
    pLay->addWidget(new QLabel("Email"), 1,0);
    pLay->addWidget(m_profEmail, 1,1);
    pLay->addWidget(new QLabel("Telefon"), 2,0);
    pLay->addWidget(m_profPhone, 2,1);
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
    if (auto* tb = findChild<QToolBar*>()) {
        tb->setVisible(false);
    }
    m_rootPages->setCurrentIndex(0);
    setWindowTitle("Trailo – Bejelentkezés");
}

void MainWindow::showApp()
{
    if (auto* tb = findChild<QToolBar*>()) {
        tb->setVisible(true);
    }
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

    if (m_profEmail) m_profEmail->setText(m_email);
    if (m_profPhone) m_profPhone->setText(m_phone);
    if (m_profName && m_profName->text().trimmed().isEmpty()) m_profName->setText(m_user);
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
    setWindowTitle("Trailo – Trailer");
}

void MainWindow::showCalendar(){
    setTabActive(1);
    m_pages->setCurrentIndex(1);
    setWindowTitle("Trailo – Calendar");
}

void MainWindow::showOrders(){
    setTabActive(2);
    m_pages->setCurrentIndex(2);
    setWindowTitle("Trailo – Orders");
}

void MainWindow::orderTrailer(const QString& title) {
    if (m_ordersList) {
        const QString ts = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm");
        m_ordersList->addItem(QStringLiteral("%1 • %2").arg(ts, title));
    }

    showOrders();

    QMessageBox::information(
        this,
        QStringLiteral("Order"),
        QStringLiteral(u"\"%1\" bérlésének megkezdése.\nItt jöhet a foglalás űrlap (AM/PM/egész nap)." ).arg(title)
        );
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

    showApp();
}

void MainWindow::onRegisterClicked()
{
    QMessageBox::information(this, "Regisztráció (PoC)",
                             "Regisztráció (PoC): itt lenne egy regisztrációs oldal/űrlap.");
}

void MainWindow::onForgotClicked()
{
    QMessageBox::information(this, "Jelszó emlékeztető (PoC)",
                             "Elfelejtett jelszó (PoC): itt lenne jelszó-visszaállítás.");
}

void MainWindow::onSaveProfileClicked()
{
    if (m_profEmail) m_email = m_profEmail->text().trimmed();
    if (m_profPhone) m_phone = m_profPhone->text().trimmed();

    QMessageBox::information(this, "Mentve", "Profil adatok elmentve (PoC).\n(A valódi mentés DB-be menne.)");
}
