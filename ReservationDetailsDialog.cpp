#include "ReservationDetailsDialog.h"

#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>

namespace {
QFrame* infoCard()
{
    auto* card = new QFrame;
    card->setStyleSheet("QFrame{background:#ffffff;border:1px solid #eef2f7;border-radius:14px;}");
    return card;
}
}

ReservationDetailsDialog::ReservationDetailsDialog(const ReservationInfo& reservation, QWidget* parent)
    : QDialog(parent), m_reservation(reservation)
{
    setWindowTitle(QStringLiteral("Foglalás részletei"));
    resize(520, 360);
    setModal(true);
    setStyleSheet("QDialog{background:#f8fafc;}");

    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(18, 18, 18, 18);
    root->setSpacing(12);

    root->addWidget(new QLabel(QStringLiteral("<b style='font-size:20px'>Foglalás részletei</b>")));

    auto* card = infoCard();
    auto* grid = new QGridLayout(card);
    grid->setContentsMargins(14, 14, 14, 14);
    grid->setHorizontalSpacing(12);
    grid->setVerticalSpacing(10);

    int row = 0;
    auto addRow = [&](const QString& label, const QString& value) {
        auto* l = new QLabel(label);
        auto* v = new QLabel(value);
        v->setWordWrap(true);
        grid->addWidget(l, row, 0);
        grid->addWidget(v, row, 1);
        ++row;
    };

    addRow(QStringLiteral("Ügyfél"), reservation.customerName);
    addRow(QStringLiteral("Email"), reservation.customerEmail);
    addRow(QStringLiteral("Telefon"), reservation.customerPhone);
    addRow(QStringLiteral("Utánfutó"), reservation.trailerName);
    addRow(QStringLiteral("Kezdés"), reservation.startDate.toString("yyyy-MM-dd"));
    addRow(QStringLiteral("Vége"), reservation.endDate.toString("yyyy-MM-dd"));
    addRow(QStringLiteral("Napok"), QString::number(reservation.days));
    addRow(QStringLiteral("Teljes ár"), QStringLiteral("%1 Ft").arg(reservation.totalPrice));
    addRow(QStringLiteral("Létrehozva"), reservation.createdAt);

    root->addWidget(card);

    auto* btns = new QHBoxLayout;
    btns->addStretch();
    auto* close = new QPushButton(QStringLiteral("Bezár"));
    close->setStyleSheet("QPushButton{padding:10px 14px;border-radius:12px;background:#e2e8f0;font-weight:700;} QPushButton:hover{background:#cbd5e1;}");
    auto* del = new QPushButton(QStringLiteral("Törlés"));
    del->setStyleSheet("QPushButton{padding:10px 14px;border-radius:12px;background:#dc2626;color:white;font-weight:800;} QPushButton:hover{background:#b91c1c;}");
    btns->addWidget(close);
    btns->addWidget(del);
    root->addLayout(btns);

    connect(close, &QPushButton::clicked, this, &QDialog::reject);
    connect(del, &QPushButton::clicked, this, [this]{
        const auto reply = QMessageBox::question(this, QStringLiteral("Foglalás törlése"), QStringLiteral("Biztosan törölni szeretnéd ezt a foglalást?"));
        if (reply != QMessageBox::Yes) return;
        if (!DatabaseManager::instance().deleteReservation(m_reservation.id)) {
            QMessageBox::warning(this, QStringLiteral("Hiba"), QStringLiteral("Nem sikerült törölni a foglalást."));
            return;
        }
        emit reservationDeleted(m_reservation.id);
        accept();
    });
}
