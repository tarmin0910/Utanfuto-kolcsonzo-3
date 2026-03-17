#include "RentDialog.h"

#include <QDateEdit>
#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>

namespace {
QFrame* sectionBox()
{
    auto* box = new QFrame;
    box->setStyleSheet("QFrame{background:#ffffff;border:1px solid #eef2f7;border-radius:14px;}");
    return box;
}
}

RentDialog::RentDialog(const TrailerInfo& trailer, const CustomerProfile& customer, QWidget* parent)
    : QDialog(parent), m_trailer(trailer), m_customer(customer)
{
    setWindowTitle(QStringLiteral("Bérlés"));
    setModal(true);
    resize(560, 360);
    setStyleSheet("QDialog{background:#f8fafc;} QLabel#muted{color:#64748b;} QLabel#big{font-size:18px;font-weight:800;color:#0a2233;} ");

    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(18, 18, 18, 18);
    root->setSpacing(12);

    auto* title = new QLabel(QStringLiteral("<b style='font-size:20px'>Foglalás véglegesítése</b>"));
    root->addWidget(title);

    auto* infoBox = sectionBox();
    auto* infoLay = new QGridLayout(infoBox);
    infoLay->setContentsMargins(14, 14, 14, 14);
    infoLay->setHorizontalSpacing(12);
    infoLay->setVerticalSpacing(10);

    infoLay->addWidget(new QLabel(QStringLiteral("Utánfutó")), 0, 0);
    auto* trailerLabel = new QLabel(trailer.name);
    trailerLabel->setObjectName("big");
    infoLay->addWidget(trailerLabel, 0, 1);

    infoLay->addWidget(new QLabel(QStringLiteral("Ügyfél")), 1, 0);
    infoLay->addWidget(new QLabel(customer.fullName), 1, 1);
    infoLay->addWidget(new QLabel(QStringLiteral("Ár / nap")), 2, 0);
    infoLay->addWidget(new QLabel(QStringLiteral("%1 Ft").arg(trailer.pricePerDay)), 2, 1);
    root->addWidget(infoBox);

    auto* periodBox = sectionBox();
    auto* periodLay = new QGridLayout(periodBox);
    periodLay->setContentsMargins(14, 14, 14, 14);
    periodLay->setHorizontalSpacing(12);
    periodLay->setVerticalSpacing(10);

    m_fromEdit = new QDateEdit(QDate::currentDate(), this);
    m_fromEdit->setCalendarPopup(true);
    m_fromEdit->setDisplayFormat("yyyy-MM-dd");
    m_fromEdit->setMinimumDate(QDate::currentDate());

    m_toEdit = new QDateEdit(QDate::currentDate().addDays(1), this);
    m_toEdit->setCalendarPopup(true);
    m_toEdit->setDisplayFormat("yyyy-MM-dd");
    m_toEdit->setMinimumDate(QDate::currentDate());

    periodLay->addWidget(new QLabel(QStringLiteral("Mettől")), 0, 0);
    periodLay->addWidget(m_fromEdit, 0, 1);
    periodLay->addWidget(new QLabel(QStringLiteral("Meddig")), 1, 0);
    periodLay->addWidget(m_toEdit, 1, 1);

    m_daysLabel = new QLabel;
    m_totalLabel = new QLabel;
    m_stockLabel = new QLabel;
    m_infoLabel = new QLabel;
    m_infoLabel->setWordWrap(true);
    m_infoLabel->setObjectName("muted");

    periodLay->addWidget(new QLabel(QStringLiteral("Napok száma")), 2, 0);
    periodLay->addWidget(m_daysLabel, 2, 1);
    periodLay->addWidget(new QLabel(QStringLiteral("Összesen")), 3, 0);
    periodLay->addWidget(m_totalLabel, 3, 1);
    periodLay->addWidget(new QLabel(QStringLiteral("Foglaltság")), 4, 0);
    periodLay->addWidget(m_stockLabel, 4, 1);
    periodLay->addWidget(m_infoLabel, 5, 0, 1, 2);
    root->addWidget(periodBox);

    auto* buttons = new QHBoxLayout;
    buttons->addStretch();
    auto* cancel = new QPushButton(QStringLiteral("Mégse"));
    cancel->setStyleSheet("QPushButton{padding:10px 14px;border-radius:12px;background:#e2e8f0;font-weight:700;} QPushButton:hover{background:#cbd5e1;}");
    m_submitButton = new QPushButton(QStringLiteral("Foglalás"));
    m_submitButton->setObjectName("primary");
    buttons->addWidget(cancel);
    buttons->addWidget(m_submitButton);
    root->addLayout(buttons);

    connect(cancel, &QPushButton::clicked, this, &QDialog::reject);
    connect(m_submitButton, &QPushButton::clicked, this, &RentDialog::submitReservation);
    connect(m_fromEdit, &QDateEdit::dateChanged, this, &RentDialog::recalculate);
    connect(m_toEdit, &QDateEdit::dateChanged, this, &RentDialog::recalculate);

    recalculate();
}

void RentDialog::recalculate()
{
    const QDate from = m_fromEdit->date();
    if (m_toEdit->date() < from) {
        m_toEdit->setDate(from);
    }

    const QDate to = m_toEdit->date();
    const int days = from.daysTo(to) + 1;
    const int total = days * m_trailer.pricePerDay;
    const int overlapping = DatabaseManager::instance().overlappingReservationsCount(m_trailer.id, from, to);
    const int freeCount = qMax(0, m_trailer.stock - overlapping);

    m_daysLabel->setText(QString::number(days));
    m_totalLabel->setText(QStringLiteral("%1 Ft").arg(total));
    m_stockLabel->setText(QStringLiteral("%1 / %2 szabad").arg(freeCount).arg(m_trailer.stock));

    if (freeCount <= 0) {
        m_infoLabel->setText(QStringLiteral("Erre az időszakra nincs szabad utánfutó ebből a típusból."));
        m_infoLabel->setStyleSheet("color:#b91c1c;font-weight:700;");
        m_submitButton->setEnabled(false);
    } else {
        m_infoLabel->setText(QStringLiteral("A foglalás a profilban megadott ügyféladatokkal kerül mentésre."));
        m_infoLabel->setStyleSheet("color:#475569;");
        m_submitButton->setEnabled(true);
    }
}

void RentDialog::submitReservation()
{
    const QDate from = m_fromEdit->date();
    const QDate to = m_toEdit->date();
    const int days = from.daysTo(to) + 1;
    const int total = days * m_trailer.pricePerDay;

    QString error;
    if (!DatabaseManager::instance().createReservation(m_trailer.id, m_customer.id, from, to, days, total, &error)) {
        QMessageBox::warning(this, QStringLiteral("Sikertelen foglalás"), error.isEmpty() ? QStringLiteral("Nem sikerült létrehozni a foglalást.") : error);
        recalculate();
        return;
    }

    accept();
}
