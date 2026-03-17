#pragma once

#include <QObject>
#include <QDate>
#include <QList>
#include <QString>

struct TrailerInfo {
    int id {0};
    QString name;
    int pricePerDay {0};
    int stock {0};
};

struct CustomerProfile {
    int id {1};
    QString fullName;
    QString email;
    QString phone;
    QString billingName;
    QString country;
    QString postalCode;
    QString city;
    QString street;
    QString houseNumber;
    QString companyName;
    QString taxNumber;
    QString idNumber;
    QString licenseNumber;
};

struct ReservationInfo {
    int id {0};
    int trailerId {0};
    int customerId {0};
    QString trailerName;
    QString customerName;
    QString customerEmail;
    QString customerPhone;
    QDate startDate;
    QDate endDate;
    int days {0};
    int totalPrice {0};
    QString createdAt;
};

class DatabaseManager : public QObject {
    Q_OBJECT
public:
    static DatabaseManager& instance();

    bool initialize();
    QString lastError() const;

    CustomerProfile loadCustomerProfile();
    bool saveCustomerProfile(const CustomerProfile& profile);

    QList<TrailerInfo> trailers() const;
    TrailerInfo trailerByName(const QString& name) const;

    bool createReservation(int trailerId, int customerId, const QDate& startDate, const QDate& endDate,
                           int days, int totalPrice, QString* errorMessage = nullptr);
    QList<ReservationInfo> reservations() const;
    QList<ReservationInfo> reservationsForMonth(const QDate& monthDate) const;
    bool deleteReservation(int reservationId);
    int overlappingReservationsCount(int trailerId, const QDate& startDate, const QDate& endDate) const;

private:
    DatabaseManager() = default;
    bool createTables();
    bool seedTrailers();

    QString m_connectionName {"trailo_connection"};
    QString m_lastError;
};
