#pragma once
#include <QMainWindow>
#include <QString>
#include <QPushButton>

class QStackedWidget;
class QToolBar;
class QAction;
class QLineEdit;
class QLabel;
class QListWidget;

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent=nullptr);

private:
    QWidget* buildLoginPage();
    QWidget* buildAppShell();

    QWidget* buildTrailerPage();
    QWidget* buildCalendarPage();
    QWidget* buildOrdersPage();
    QWidget* buildProfilePage();

    void applyGlobalStyle();
    void setTabActive(int tabIndex); // 0=Trailer, 1=Calendar, 2=Orders
    void showLogin();
    void showApp();
    void showProfile();
    bool validateLogin(const QString& user, const QString& pass);

private slots:
    void showTrailer();
    void showCalendar();
    void showOrders();
    void orderTrailer(const QString& title);

    void onLoginClicked();
    void onRegisterClicked();
    void onForgotClicked();
    void onSaveProfileClicked();

private:
    QStackedWidget* m_rootPages {nullptr};
    QStackedWidget* m_pages {nullptr};

    QPushButton* m_tabTrailer {nullptr};
    QPushButton* m_tabCalendar {nullptr};
    QPushButton* m_tabOrders {nullptr};

    QAction* m_profileAction {nullptr};
    QAction* m_settingsAction {nullptr};

    QLineEdit* m_loginUser {nullptr};
    QLineEdit* m_loginPass {nullptr};
    QLabel*    m_loginError {nullptr};

    QListWidget* m_ordersList {nullptr};

    QLineEdit* m_profName {nullptr};
    QLineEdit* m_profEmail {nullptr};
    QLineEdit* m_profPhone {nullptr};
    QLineEdit* m_profBillingName {nullptr};
    QLineEdit* m_profBillingCountry {nullptr};
    QLineEdit* m_profBillingZip {nullptr};
    QLineEdit* m_profBillingCity {nullptr};
    QLineEdit* m_profBillingStreet {nullptr};
    QLineEdit* m_profBillingHouse {nullptr};
    QLineEdit* m_profCompany {nullptr};
    QLineEdit* m_profTaxNumber {nullptr};
    QLineEdit* m_profIdNumber {nullptr};
    QLineEdit* m_profDriverLicense {nullptr};

    QString m_user;
    QString m_email;
    QString m_phone;
};
