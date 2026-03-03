
#include "ResetPasswordDialog.h"
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QMessageBox>
#include <QFrame>

ResetPasswordDialog::ResetPasswordDialog(QWidget* parent) : QDialog(parent) {
    setWindowTitle(QStringLiteral("Jelszó visszaállítás"));
    resize(360, 220);

    auto *root = new QVBoxLayout(this);

    auto *panel = new QFrame; panel->setObjectName("panel");
    panel->setStyleSheet(
        "QFrame#panel{background:#ffffff;border:1px solid #e9ecef;border-radius:16px;}"
        "QLineEdit{background:#f6f7f8;border:1px solid #e9ecef;border-radius:12px;padding:8px 12px;}"
        "QPushButton#primary{background:#0e7a6a;color:white;border-radius:10px;padding:8px 14px;font-weight:600;}"
    );

    auto *lay = new QVBoxLayout(panel); lay->setContentsMargins(16,16,16,16); lay->setSpacing(10);

    lay->addWidget(new QLabel(QStringLiteral("Add meg az e-mail címed, elküldjük a visszaállító linket.")));
    auto *email = new QLineEdit; email->setPlaceholderText(QStringLiteral("Email"));
    auto *btn = new QPushButton(QStringLiteral("Link küldése")); btn->setObjectName("primary");

    lay->addWidget(email);
    lay->addWidget(btn);

    root->addWidget(panel);

    connect(btn, &QPushButton::clicked, this, [this, email]{
        if (email->text().isEmpty()) {
            QMessageBox::warning(this, QStringLiteral("Hiba"), QStringLiteral("Kérlek add meg az e-mail címed."));
            return;
        }
        QMessageBox::information(this, QStringLiteral("Elküldve"), QStringLiteral("Ha létezik a fiókod, elküldtük a linket (demo)."));
        accept();
    });
}
