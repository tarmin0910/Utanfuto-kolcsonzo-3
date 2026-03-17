#pragma once
#include <QFrame>
#include <QString>

class TrailerCard : public QFrame {
    Q_OBJECT

public:
    TrailerCard(const QString& title,
                const QString& pricePerDay,
                const QString& imagePath,
                QWidget* parent = nullptr);

signals:
    void orderClicked(const QString& title);
};
