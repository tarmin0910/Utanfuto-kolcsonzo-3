
#pragma once
#include <QDialog>
class QLineEdit;

class RegisterDialog : public QDialog {
    Q_OBJECT
public:
    explicit RegisterDialog(QWidget* parent=nullptr);
private:
    QLineEdit* m_user{nullptr};
    QLineEdit* m_email{nullptr};
    QLineEdit* m_pass1{nullptr};
    QLineEdit* m_pass2{nullptr};
};
