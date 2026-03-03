
#include "RegisterDialog.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QMessageBox>
#include <QFrame>

RegisterDialog::RegisterDialog(QWidget* parent) : QDialog(parent) {
    setWindowTitle(QStringLiteral("Regisztráció"));
    resize(420, 420);

    auto *root = new QVBoxLayout(this);

    auto *panel = new QFrame; panel->setObjectName("panel");
    panel->setStyleSheet(
        "QFrame#panel{background:#ffffff;border:1px solid #e9ecef;border-radius:16px;}"
        "QLineEdit{background:#f6f7f8;border:1px solid #e9ecef;border-radius:12px;padding:8px 12px;}"
        "QPushButton#primary{background:#0e7a6a;color:white;border-radius:10px;padding:8px 14px;font-weight:600;}"
    );

    auto *lay = new QVBoxLayout(panel); lay->setContentsMargins(16,16,16,16); lay->setSpacing(10);

    lay->addWidget(new QLabel(QStringLiteral("<b>Hozz létre fiókot</b>")));

    m_user = new QLineEdit; m_user->setPlaceholderText(QStringLiteral("Felhasználónév"));
    m_email= new QLineEdit; m_email->setPlaceholderText(QStringLiteral("Email"));
    m_pass1= new QLineEdit; m_pass1->setEchoMode(QLineEdit::Password); m_pass1->setPlaceholderText(QStringLiteral("Jelszó"));
    m_pass2= new QLineEdit; m_pass2->setEchoMode(QLineEdit::Password); m_pass2->setPlaceholderText(QStringLiteral("Jelszó még egyszer"));

    auto *btn = new QPushButton(QStringLiteral("Regisztrálok")); btn->setObjectName("primary");

    lay->addWidget(m_user); lay->addWidget(m_email); lay->addWidget(m_pass1); lay->addWidget(m_pass2);
    lay->addWidget(btn);

    root->addWidget(panel);

    connect(btn, &QPushButton::clicked, this, [this]{
        if (m_pass1->text() != m_pass2->text()) {
            QMessageBox::warning(this, QStringLiteral("Hiba"), QStringLiteral("A jelszavak nem egyeznek."));
            return;
        }
        QMessageBox::information(this, QStringLiteral("Siker"), QStringLiteral("Regisztráció sikeres (demo)."));
        accept();
    });
}
