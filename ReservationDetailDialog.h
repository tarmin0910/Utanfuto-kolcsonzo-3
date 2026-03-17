#pragma once

#include <QDialog>
#include "DatabaseManager.h"

class ReservationDetailsDialog : public QDialog {
    Q_OBJECT
public:
    ReservationDetailsDialog(const ReservationInfo& reservation, QWidget* parent = nullptr);

signals:
    void reservationDeleted(int reservationId);

private:
    ReservationInfo m_reservation;
};
