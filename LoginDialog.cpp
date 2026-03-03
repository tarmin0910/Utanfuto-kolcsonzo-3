#include "LoginDialog.h"
#include "RegisterDialog.h"
#include "ResetPasswordDialog.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QFrame>

LoginDialog::LoginDialog(QWidget* parent) : QDialog(parent) {
    setWindowTitle(QStringLiteral("Trailo – Bejelentkezés"));
    setModal(true);

    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(24,24,24,24);
    root->addWidget(buildPanel(), 1);
}

QWidget* LoginDialog::buildPanel() {
    auto *wrap = new QFrame;
    wrap->setObjectName("panel");
    wrap->setStyleSheet(
        "QFrame#panel{background:#ffffff;border:1px solid #e9ecef;border-radius:16px;}"
        "QLineEdit{background:#f6f7f8;border:1px solid #e9ecef;border-radius:12px;padding:10px 14px;}"
        "QPushButton#primary{background:#0e7a6a;color:white;border-radius:10px;padding:10px 18px;font-weight:600;}"
        "QPushButton#primary:hover{background:#0c6a5c;}"
        "QLabel#title{font-size:22px;font-weight:700;color:#0a2233;}"
        );

    auto *outer = new QVBoxLayout(wrap);
    outer->setContentsMargins(24,24,24,24);
    outer->setSpacing(16);


    auto *top = new QHBoxLayout;
    top->setSpacing(12);

    auto *lblLogo = new QLabel(QStringLiteral("<b style='font-size:18px'>Trailo</b>"));

    auto *btnTrailer = new QPushButton(QStringLiteral("Trailer"));
    auto *btnCalendar = new QPushButton(QStringLiteral("Calendar"));
    auto *btnOrders = new QPushButton(QStringLiteral("Orders"));

    auto styleTabLocal = [](QPushButton* b, bool active){
        b->setCheckable(true);
        b->setChecked(active);
        b->setStyleSheet(QString(
                             "QPushButton{padding:6px 12px;border-radius:10px;font-weight:600;"
                             "background:%1;color:%2}"
                             "QPushButton:hover{background:%3}")
                             .arg(active ? "#0a2233" : "#f1f3f5")
                             .arg(active ? "white" : "#0a2233")
                             .arg(active ? "#0a2233" : "#e9ecef"));
        b->setEnabled(false);
    };

    styleTabLocal(btnTrailer, true);
    styleTabLocal(btnCalendar, false);
    styleTabLocal(btnOrders, false);

    top->addWidget(lblLogo);
    top->addSpacing(8);
    top->addWidget(btnTrailer);
    top->addWidget(btnCalendar);
    top->addWidget(btnOrders);
    top->addStretch(1);
    top->addWidget(new QLabel(QStringLiteral("⚪")));
    top->addWidget(new QLabel(QStringLiteral("⚫")));

    outer->addLayout(top);

    auto *center = new QVBoxLayout;
    center->setSpacing(10);
    center->addSpacing(12);

    auto *title = new QLabel(QStringLiteral("<div style='font-size:28px;font-weight:700;color:#0a2233'>Jelentkezz be</div>"));
    title->setAlignment(Qt::AlignHCenter);

    m_user = new QLineEdit;
    m_user->setPlaceholderText(QStringLiteral("Felhasználónév (pl. adam)"));

    m_pass = new QLineEdit;
    m_pass->setPlaceholderText(QStringLiteral("Jelszó"));
    m_pass->setEchoMode(QLineEdit::Password);

    m_error = new QLabel;
    m_error->setStyleSheet("color:#c62828;");
    m_error->setVisible(false);

    auto *btnLogin = new QPushButton(QStringLiteral("Bejelentkezés"));
    btnLogin->setObjectName("primary");

    auto *linksRow = new QHBoxLayout;
    linksRow->setSpacing(16);

    auto *lblRegister = new QLabel(QStringLiteral("Még nem regisztrált? <a href='reg'>Regisztráció</a>"));
    lblRegister->setTextFormat(Qt::RichText);
    lblRegister->setTextInteractionFlags(Qt::TextBrowserInteraction);
    lblRegister->setOpenExternalLinks(false);

    auto *lblForgot = new QLabel(QStringLiteral("<a href='forgot'>Elfelejtetted a jelszavad?</a>"));
    lblForgot->setTextFormat(Qt::RichText);
    lblForgot->setTextInteractionFlags(Qt::TextBrowserInteraction);
    lblForgot->setOpenExternalLinks(false);

    linksRow->addWidget(lblRegister, 0, Qt::AlignLeft);
    linksRow->addStretch(1);
    linksRow->addWidget(lblForgot, 0, Qt::AlignRight);

    center->addWidget(title);
    center->addSpacing(8);
    center->addWidget(m_user);
    center->addWidget(m_pass);
    center->addWidget(m_error);
    center->addSpacing(4);
    center->addWidget(btnLogin);
    center->addLayout(linksRow);

    outer->addLayout(center);
    outer->addStretch(1);

    connect(btnLogin, &QPushButton::clicked, this, &LoginDialog::onLogin);
    connect(m_pass, &QLineEdit::returnPressed, this, &LoginDialog::onLogin);
    connect(lblRegister, &QLabel::linkActivated, this, [this](const QString&){ onRegister(); });
    connect(lblForgot, &QLabel::linkActivated, this, [this](const QString&){ onForgot(); });

    return wrap;
}

QString LoginDialog::username() const {
    return m_user->text().trimmed();
}

void LoginDialog::onLogin() {
    const auto u = m_user->text().trimmed();
    const auto p = m_pass->text();

    if ((u.compare(QStringLiteral("adam"), Qt::CaseInsensitive) == 0) &&
        p == QStringLiteral("trailo123")) {
        accept();
        return;
    }

    if (!u.isEmpty() && !p.isEmpty()) {
        accept();
        return;
    }

    m_error->setText(QStringLiteral("Kérlek töltsd ki a felhasználónevet és a jelszót."));
    m_error->setVisible(true);
}

void LoginDialog::onRegister() {
    RegisterDialog d(this);
    d.exec();
}

void LoginDialog::onForgot() {
    ResetPasswordDialog d(this);
    d.exec();
}
