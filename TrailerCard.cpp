#include "TrailerCard.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QPixmap>
#include <QDebug>
#include <QColor>
#include <QSizePolicy>
#include <QFrame>

TrailerCard::TrailerCard(const QString& title,
                         const QString& pricePerDay,
                         const QString& imagePath,
                         QWidget* parent)
    : QFrame(parent)
{
    setFrameShape(QFrame::NoFrame);
    setMinimumHeight(320);
    setMaximumHeight(320);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    setStyleSheet(
        "QFrame{background:#ffffff;border:1px solid #e5e7eb;border-radius:18px;}"
        "QFrame:hover{border:1px solid #0f766e;}"
        "QLabel#t{font-size:20px;font-weight:800;color:#0a2233;background:transparent;}"
        "QLabel#p{font-size:15px;color:#475569;background:transparent;}"
        "QPushButton#order{background:#0f766e;color:white;border-radius:10px;padding:8px 12px;font-weight:700;}"
        "QPushButton#order:hover{background:#0a5e58;}"
        );

    auto *root = new QVBoxLayout(this);
    root->setContentsMargins(16,16,16,16);
    root->setSpacing(12);

    // ---- KÉPDOBÓZ ----
    auto *imageBox = new QFrame;
    imageBox->setMinimumHeight(160);
    imageBox->setMaximumHeight(160);
    imageBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    imageBox->setStyleSheet("QFrame{background:#f1f5f9;border-radius:12px;border:none;}");

    auto *imageLayout = new QHBoxLayout(imageBox);
    imageLayout->setContentsMargins(8,8,8,8);
    imageLayout->setSpacing(0);

    QLabel *img = new QLabel;
    img->setAlignment(Qt::AlignCenter);
    img->setMinimumSize(220, 140);
    img->setMaximumHeight(144);
    img->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    img->setStyleSheet("background:transparent;border:none;");

    QPixmap pix;

    if (!pix.load(imagePath)) {
        qDebug() << "Resource nem található:" << imagePath;

        QString localPath = imagePath;
        localPath.remove(":/");

        if (!pix.load(localPath)) {
            qDebug() << "Local sem található:" << localPath;
            pix = QPixmap(260, 140);
            pix.fill(QColor("#e5e7eb"));
        }
    }

    QPixmap scaled = pix.scaled(
        260, 140,
        Qt::KeepAspectRatio,
        Qt::SmoothTransformation
        );

    img->setPixmap(scaled);
    imageLayout->addStretch();
    imageLayout->addWidget(img);
    imageLayout->addStretch();


    auto *infoBox = new QWidget;
    infoBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    auto *infoLayout = new QVBoxLayout(infoBox);
    infoLayout->setContentsMargins(0,0,0,0);
    infoLayout->setSpacing(8);

    QLabel *titleLbl = new QLabel(title);
    titleLbl->setObjectName("t");
    titleLbl->setWordWrap(true);
    titleLbl->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    QLabel *priceLbl = new QLabel(pricePerDay + " Ft/nap");
    priceLbl->setObjectName("p");
    priceLbl->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    QPushButton *btn = new QPushButton("Bérlés");
    btn->setObjectName("order");
    btn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    infoLayout->addWidget(titleLbl);
    infoLayout->addWidget(priceLbl);
    infoLayout->addWidget(btn, 0, Qt::AlignLeft);

    root->addWidget(imageBox);
    root->addWidget(infoBox);
    root->addStretch();

    connect(btn, &QPushButton::clicked, this, [this, title]{
        emit orderClicked(title);
    });
}
