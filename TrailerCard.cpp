
#include "TrailerCard.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QFrame>

TrailerCard::TrailerCard(const QString& title, const QString& pricePerDay, QWidget* parent)
    : QFrame(parent) {
    setFrameShape(QFrame::NoFrame);
    setStyleSheet(
        "QFrame{background:#ffffff;border:1px solid #e9ecef;border-radius:16px;}"
        "QLabel#t{font-size:20px;font-weight:800;color:#0a2233;}"
        "QLabel#p{font-size:16px;color:#0a2233;}"
        "QPushButton#order{background:#0a2233;color:white;border-radius:8px;padding:4px 8px;font-weight:700;}"
    );

    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(16,16,16,16);
    root->setSpacing(8);

    auto *img = new QFrame; img->setMinimumHeight(160); img->setStyleSheet("QFrame{background:#f6f7f8;border-radius:12px;}");
    root->addWidget(img);

    auto *titleLbl = new QLabel(title); titleLbl->setObjectName("t");
    auto *priceLbl = new QLabel(pricePerDay + QStringLiteral(" Ft/nap")); priceLbl->setObjectName("p");
    auto *btn = new QPushButton(QStringLiteral("Order")); btn->setObjectName("order"); btn->setFixedWidth(64);

    root->addWidget(titleLbl);
    root->addWidget(priceLbl);
    root->addWidget(btn, 0, Qt::AlignLeft);

    connect(btn, &QPushButton::clicked, this, [this, title]{ emit orderClicked(title); });
}
