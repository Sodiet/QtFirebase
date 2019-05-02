#ifndef QTFIREBASE_AUTH_H
#define QTFIREBASE_AUTH_H

#include "qtfirebaseservice.h"
#include "firebase/auth.h"

#include <QMutex>

#ifdef QTFIREBASE_BUILD_AUTH
#include "src/qtfirebase.h"
#if defined(qFirebaseAuth)
#undef qFirebaseAuth
#endif
#define qFirebaseAuth (static_cast<QtFirebaseAuth *>(QtFirebaseAuth::instance()))

class SmsListener;

class QtFirebaseAuth : public QtFirebaseService
{
    Q_OBJECT
    Q_PROPERTY(bool running READ running NOTIFY runningChanged)
    Q_PROPERTY(bool signedIn READ signedIn NOTIFY signedInChanged)
public:
    static QtFirebaseAuth *instance()
    {
        if(self == nullptr)
        {
            self = new QtFirebaseAuth(0);
            qDebug() << self << "::instance" << "singleton";
        }
        return self;
    }

    ~QtFirebaseAuth();

    enum Error
    {
        ErrorNone = firebase::auth::kAuthErrorNone,
        ErrorUnimplemented = firebase::auth::kAuthErrorUnimplemented,
        ErrorFailure = firebase::auth::kAuthErrorFailure
    };
    Q_ENUM(Error)

    enum Action
    {
        ActionRegister,
        ActionSignIn,
        ActionSignOut,
        ActionDeleteUser
    };
    Q_ENUM(Action)



public slots:
    //Control
    void registerUser(const QString& email, const QString& pass);
    void signIn(const QString& email, const QString& pass);
    void signOut();
    void sendPasswordResetEmail(const QString& email);
    void deleteUser();

    //Sms
    void smsVerification(firebase::auth::Credential credential);
    void smsVerificationError(const QString &errorMsg);
    void smsSignIn(const QString &phone_number);
    void resendSms();
    void codeReceived(const QString &code);

    //Status
    bool signedIn() const;
    bool running() const;
    int errorId() const;
    QString errorMsg() const;

    //Google Sign-In
    void googleSignIn();
    void facebookSignIn();

    //Data
    QString email() const;
    QString displayName() const;
    bool emailVerified() const;
    QString photoUrl() const;
    QString uid() const;
    QString number() const;
    QString id() const;

signals:
    void signedInChanged();
    void runningChanged();
    void completed(bool success, int actionId);
    void passwordResetEmailSent();
    void codeSent();

protected:
    explicit QtFirebaseAuth(QObject *parent = 0);

private:
    void clearError();
    void setComplete(bool complete);
    void setSignIn(bool value);
    void setError(int errId, const QString& errMsg = QString());
    void init() override;
    void onFutureEvent(QString eventId, firebase::FutureBase future) override;
    void preSignIn();

    static QtFirebaseAuth *self;
    firebase::auth::Auth* m_auth;

    bool m_complete;
    bool m_signedIn;
    int m_errId;
    QString m_errMsg;
    int m_action;
    SmsListener *m_listener = nullptr;
    QMutex m_mutex;
    QString m_phoneNumber;

    Q_DISABLE_COPY(QtFirebaseAuth)
};



class SmsListener : public QObject, public firebase::auth::PhoneAuthProvider::Listener
{
    Q_OBJECT

public:
    SmsListener(QObject* parent = nullptr);

    ~SmsListener() override {}

    virtual void OnVerificationCompleted(firebase::auth::Credential credential) override;
    virtual void OnVerificationFailed(const std::string& error) override;
    virtual void OnCodeSent(const std::string& verification_id,
                            const firebase::auth::PhoneAuthProvider::ForceResendingToken &force_resending_token) override;

    QString getPhoneNumber() const;
    firebase::auth::PhoneAuthProvider::ForceResendingToken* getToken();

public slots:
    QString verificationID() const;

signals:
    void verificationCompleted(firebase::auth::Credential credential);
    void verificationFailed(const QString &error);
    void codeSent();

private:
    void setVerificationID(const QString &id);
    void setToken(const firebase::auth::PhoneAuthProvider::ForceResendingToken &m_token);
    void setPhoneNumber(const QString &number);

    firebase::auth::PhoneAuthProvider::ForceResendingToken m_token;
    QString m_verificationID;
    QString m_phoneNumber;
    QMutex m_mutex;
};


#endif //QTFIREBASE_BUILD_AUTH

#endif // QTFIREBASE_AUTH_H
