
#pragma once
#include <QDialog>
#include <QString>

class QLineEdit;
class QLabel;

class LoginDialog : public QDialog {
    Q_OBJECT
public:
    explicit LoginDialog(QWidget* parent = nullptr);
    QString username() const;

private slots:
    void onLogin();
    void onRegister();
    void onForgot();

private:
    QWidget* buildPanel();
    QLineEdit* m_user {nullptr};
    QLineEdit* m_pass {nullptr};
    QLabel*    m_error {nullptr};
};
