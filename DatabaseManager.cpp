#include "DatabaseManager.h"

#include <QCoreApplication>
#include <QDir>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>

namespace {
QSqlDatabase dbFor(const QString& name)
{
    if (QSqlDatabase::contains(name)) {
        return QSqlDatabase::database(name);
    }
    return QSqlDatabase();
}

QString dbPath()
{
    return QDir(QCoreApplication::applicationDirPath()).filePath("trailo_rentals.sqlite");
}
}

DatabaseManager& DatabaseManager::instance()
{
    static DatabaseManager inst;
    return inst;
}

bool DatabaseManager::initialize()
{
    QSqlDatabase db = dbFor(m_connectionName);
    if (!db.isValid()) {
        db = QSqlDatabase::addDatabase("QSQLITE", m_connectionName);
        db.setDatabaseName(dbPath());
    }

    if (!db.open()) {
        m_lastError = db.lastError().text();
        return false;
    }

    if (!createTables()) return false;
    if (!seedTrailers()) return false;
    return true;
}

QString DatabaseManager::lastError() const
{
    return m_lastError;
}

bool DatabaseManager::createTables()
{
    QSqlDatabase db = dbFor(m_connectionName);
    if (!db.isOpen()) return false;

    QSqlQuery q(db);

    const QString customersSql = R"(
        CREATE TABLE IF NOT EXISTS customers (
            id INTEGER PRIMARY KEY,
            full_name TEXT,
            email TEXT,
            phone TEXT,
            billing_name TEXT,
            country TEXT,
            postal_code TEXT,
            city TEXT,
            street TEXT,
            house_number TEXT,
            company_name TEXT,
            tax_number TEXT,
            id_number TEXT,
            license_number TEXT
        )
    )";

    const QString trailersSql = R"(
        CREATE TABLE IF NOT EXISTS trailers (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            name TEXT NOT NULL UNIQUE,
            price_per_day INTEGER NOT NULL,
            stock INTEGER NOT NULL
        )
    )";

    const QString reservationsSql = R"(
        CREATE TABLE IF NOT EXISTS reservations (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            trailer_id INTEGER NOT NULL,
            customer_id INTEGER NOT NULL,
            start_date TEXT NOT NULL,
            end_date TEXT NOT NULL,
            days INTEGER NOT NULL,
            total_price INTEGER NOT NULL,
            created_at TEXT NOT NULL,
            FOREIGN KEY(trailer_id) REFERENCES trailers(id),
            FOREIGN KEY(customer_id) REFERENCES customers(id)
        )
    )";

    if (!q.exec(customersSql) || !q.exec(trailersSql) || !q.exec(reservationsSql)) {
        m_lastError = q.lastError().text();
        return false;
    }

    return true;
}

bool DatabaseManager::seedTrailers()
{
    QSqlDatabase db = dbFor(m_connectionName);
    QSqlQuery countQuery(db);
    if (!countQuery.exec("SELECT COUNT(*) FROM trailers")) {
        m_lastError = countQuery.lastError().text();
        return false;
    }
    if (countQuery.next() && countQuery.value(0).toInt() > 0) {
        return true;
    }

    const QList<TrailerInfo> seed = {
        {0, QStringLiteral("Nyitott, oldalfalas utánfutó"), 5000, 3},
        {0, QStringLiteral("Ponyvás"), 7000, 3},
        {0, QStringLiteral("Motorszállító"), 8000, 3},
        {0, QStringLiteral("Autószállító"), 12000, 3},
        {0, QStringLiteral("Zárt dobozos / autó- és áruszállító"), 18000, 3},
        {0, QStringLiteral("Billenőplatós utánfutó"), 7500, 3}
    };

    QSqlQuery insert(db);
    insert.prepare("INSERT INTO trailers(name, price_per_day, stock) VALUES (?, ?, ?)");
    for (const auto& trailer : seed) {
        insert.addBindValue(trailer.name);
        insert.addBindValue(trailer.pricePerDay);
        insert.addBindValue(trailer.stock);
        if (!insert.exec()) {
            m_lastError = insert.lastError().text();
            return false;
        }
        insert.clear();
        insert.prepare("INSERT INTO trailers(name, price_per_day, stock) VALUES (?, ?, ?)");
    }

    return true;
}

CustomerProfile DatabaseManager::loadCustomerProfile()
{
    CustomerProfile profile;
    QSqlDatabase db = dbFor(m_connectionName);
    QSqlQuery q(db);
    q.prepare("SELECT id, full_name, email, phone, billing_name, country, postal_code, city, street, house_number, company_name, tax_number, id_number, license_number FROM customers WHERE id = 1");
    if (!q.exec()) {
        m_lastError = q.lastError().text();
        return profile;
    }

    if (q.next()) {
        profile.id = q.value(0).toInt();
        profile.fullName = q.value(1).toString();
        profile.email = q.value(2).toString();
        profile.phone = q.value(3).toString();
        profile.billingName = q.value(4).toString();
        profile.country = q.value(5).toString();
        profile.postalCode = q.value(6).toString();
        profile.city = q.value(7).toString();
        profile.street = q.value(8).toString();
        profile.houseNumber = q.value(9).toString();
        profile.companyName = q.value(10).toString();
        profile.taxNumber = q.value(11).toString();
        profile.idNumber = q.value(12).toString();
        profile.licenseNumber = q.value(13).toString();
    }

    return profile;
}

bool DatabaseManager::saveCustomerProfile(const CustomerProfile& profile)
{
    QSqlDatabase db = dbFor(m_connectionName);
    QSqlQuery q(db);
    q.prepare(R"(
        INSERT INTO customers(id, full_name, email, phone, billing_name, country, postal_code, city, street, house_number, company_name, tax_number, id_number, license_number)
        VALUES(1, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)
        ON CONFLICT(id) DO UPDATE SET
            full_name=excluded.full_name,
            email=excluded.email,
            phone=excluded.phone,
            billing_name=excluded.billing_name,
            country=excluded.country,
            postal_code=excluded.postal_code,
            city=excluded.city,
            street=excluded.street,
            house_number=excluded.house_number,
            company_name=excluded.company_name,
            tax_number=excluded.tax_number,
            id_number=excluded.id_number,
            license_number=excluded.license_number
    )");

    q.addBindValue(profile.fullName);
    q.addBindValue(profile.email);
    q.addBindValue(profile.phone);
    q.addBindValue(profile.billingName);
    q.addBindValue(profile.country);
    q.addBindValue(profile.postalCode);
    q.addBindValue(profile.city);
    q.addBindValue(profile.street);
    q.addBindValue(profile.houseNumber);
    q.addBindValue(profile.companyName);
    q.addBindValue(profile.taxNumber);
    q.addBindValue(profile.idNumber);
    q.addBindValue(profile.licenseNumber);

    if (!q.exec()) {
        m_lastError = q.lastError().text();
        return false;
    }

    return true;
}

QList<TrailerInfo> DatabaseManager::trailers() const
{
    QList<TrailerInfo> list;
    QSqlDatabase db = dbFor(m_connectionName);
    QSqlQuery q(db);
    q.prepare("SELECT id, name, price_per_day, stock FROM trailers ORDER BY id ASC");
    if (!q.exec()) {
        return list;
    }

    while (q.next()) {
        TrailerInfo trailer;
        trailer.id = q.value(0).toInt();
        trailer.name = q.value(1).toString();
        trailer.pricePerDay = q.value(2).toInt();
        trailer.stock = q.value(3).toInt();
        list.append(trailer);
    }

    return list;
}

TrailerInfo DatabaseManager::trailerByName(const QString& name) const
{
    TrailerInfo trailer;
    QSqlDatabase db = dbFor(m_connectionName);
    QSqlQuery q(db);
    q.prepare("SELECT id, name, price_per_day, stock FROM trailers WHERE name = ?");
    q.addBindValue(name);
    if (!q.exec()) {
        return trailer;
    }

    if (q.next()) {
        trailer.id = q.value(0).toInt();
        trailer.name = q.value(1).toString();
        trailer.pricePerDay = q.value(2).toInt();
        trailer.stock = q.value(3).toInt();
    }

    return trailer;
}

int DatabaseManager::overlappingReservationsCount(int trailerId, const QDate& startDate, const QDate& endDate) const
{
    QSqlDatabase db = dbFor(m_connectionName);
    QSqlQuery q(db);
    q.prepare(R"(
        SELECT COUNT(*)
        FROM reservations
        WHERE trailer_id = ?
          AND start_date <= ?
          AND end_date >= ?
    )");
    q.addBindValue(trailerId);
    q.addBindValue(endDate.toString(Qt::ISODate));
    q.addBindValue(startDate.toString(Qt::ISODate));
    if (!q.exec()) {
        return 0;
    }
    if (q.next()) {
        return q.value(0).toInt();
    }
    return 0;
}

bool DatabaseManager::createReservation(int trailerId, int customerId, const QDate& startDate, const QDate& endDate,
                                        int days, int totalPrice, QString* errorMessage)
{
    QSqlDatabase db = dbFor(m_connectionName);
    QSqlQuery stockQuery(db);
    stockQuery.prepare("SELECT stock FROM trailers WHERE id = ?");
    stockQuery.addBindValue(trailerId);
    if (!stockQuery.exec() || !stockQuery.next()) {
        if (errorMessage) *errorMessage = QStringLiteral("Nem sikerült lekérni a készletet.");
        return false;
    }

    const int stock = stockQuery.value(0).toInt();
    const int booked = overlappingReservationsCount(trailerId, startDate, endDate);
    if (booked >= stock) {
        if (errorMessage) *errorMessage = QStringLiteral("Ebben az időszakban már nincs szabad utánfutó ebből a típusból.");
        return false;
    }

    QSqlQuery q(db);
    q.prepare(R"(
        INSERT INTO reservations(trailer_id, customer_id, start_date, end_date, days, total_price, created_at)
        VALUES(?, ?, ?, ?, ?, ?, datetime('now', 'localtime'))
    )");
    q.addBindValue(trailerId);
    q.addBindValue(customerId);
    q.addBindValue(startDate.toString(Qt::ISODate));
    q.addBindValue(endDate.toString(Qt::ISODate));
    q.addBindValue(days);
    q.addBindValue(totalPrice);

    if (!q.exec()) {
        if (errorMessage) *errorMessage = q.lastError().text();
        return false;
    }

    return true;
}

QList<ReservationInfo> DatabaseManager::reservations() const
{
    QList<ReservationInfo> list;
    QSqlDatabase db = dbFor(m_connectionName);
    QSqlQuery q(db);
    q.prepare(R"(
        SELECT r.id, r.trailer_id, r.customer_id, t.name, c.full_name, c.email, c.phone,
               r.start_date, r.end_date, r.days, r.total_price, r.created_at
        FROM reservations r
        JOIN trailers t ON t.id = r.trailer_id
        JOIN customers c ON c.id = r.customer_id
        ORDER BY r.start_date DESC, r.id DESC
    )");
    if (!q.exec()) {
        return list;
    }

    while (q.next()) {
        ReservationInfo item;
        item.id = q.value(0).toInt();
        item.trailerId = q.value(1).toInt();
        item.customerId = q.value(2).toInt();
        item.trailerName = q.value(3).toString();
        item.customerName = q.value(4).toString();
        item.customerEmail = q.value(5).toString();
        item.customerPhone = q.value(6).toString();
        item.startDate = QDate::fromString(q.value(7).toString(), Qt::ISODate);
        item.endDate = QDate::fromString(q.value(8).toString(), Qt::ISODate);
        item.days = q.value(9).toInt();
        item.totalPrice = q.value(10).toInt();
        item.createdAt = q.value(11).toString();
        list.append(item);
    }

    return list;
}

QList<ReservationInfo> DatabaseManager::reservationsForMonth(const QDate& monthDate) const
{
    const QDate first = QDate(monthDate.year(), monthDate.month(), 1);
    const QDate last = first.addMonths(1).addDays(-1);

    QList<ReservationInfo> list;
    QSqlDatabase db = dbFor(m_connectionName);
    QSqlQuery q(db);
    q.prepare(R"(
        SELECT r.id, r.trailer_id, r.customer_id, t.name, c.full_name, c.email, c.phone,
               r.start_date, r.end_date, r.days, r.total_price, r.created_at
        FROM reservations r
        JOIN trailers t ON t.id = r.trailer_id
        JOIN customers c ON c.id = r.customer_id
        WHERE r.start_date <= ? AND r.end_date >= ?
        ORDER BY t.id ASC, r.start_date ASC
    )");
    q.addBindValue(last.toString(Qt::ISODate));
    q.addBindValue(first.toString(Qt::ISODate));
    if (!q.exec()) {
        return list;
    }

    while (q.next()) {
        ReservationInfo item;
        item.id = q.value(0).toInt();
        item.trailerId = q.value(1).toInt();
        item.customerId = q.value(2).toInt();
        item.trailerName = q.value(3).toString();
        item.customerName = q.value(4).toString();
        item.customerEmail = q.value(5).toString();
        item.customerPhone = q.value(6).toString();
        item.startDate = QDate::fromString(q.value(7).toString(), Qt::ISODate);
        item.endDate = QDate::fromString(q.value(8).toString(), Qt::ISODate);
        item.days = q.value(9).toInt();
        item.totalPrice = q.value(10).toInt();
        item.createdAt = q.value(11).toString();
        list.append(item);
    }

    return list;
}

bool DatabaseManager::deleteReservation(int reservationId)
{
    QSqlDatabase db = dbFor(m_connectionName);
    QSqlQuery q(db);
    q.prepare("DELETE FROM reservations WHERE id = ?");
    q.addBindValue(reservationId);
    if (!q.exec()) {
        return false;
    }
    return true;
}
