#pragma once

#include <QDialog>
#include <QDate>
#include "DatabaseManager.h"

class QLabel;
class QDateEdit;
class QPushButton;

class RentDialog : public QDialog {
    Q_OBJECT
public:
    RentDialog(const TrailerInfo& trailer, const CustomerProfile& customer, QWidget* parent = nullptr);

private slots:
    void recalculate();
    void submitReservation();

private:
    TrailerInfo m_trailer;
    CustomerProfile m_customer;
    QLabel* m_daysLabel {nullptr};
    QLabel* m_totalLabel {nullptr};
    QLabel* m_infoLabel {nullptr};
    QLabel* m_stockLabel {nullptr};
    QDateEdit* m_fromEdit {nullptr};
    QDateEdit* m_toEdit {nullptr};
    QPushButton* m_submitButton {nullptr};
};
