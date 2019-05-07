#include "qtfirebaseauth.h"
#include "qtfirebasesocialsignin.h"

#include <QObject>
#include <QMetaMethod>

namespace auth = ::firebase::auth;

QtFirebaseAuth *QtFirebaseAuth::self = 0;

QtFirebaseAuth::QtFirebaseAuth(QObject *parent) : QtFirebaseService(parent),
    m_auth(nullptr)
    ,m_complete(false)
    ,m_signedIn(false)
    ,m_errId(ErrorNone)
    ,m_action(ActionSignIn)
{
    if(self == 0)
    {
        self = this;
        qDebug() << self << "::QtFirebaseAuth" << "singleton";
        QtFirebaseSocialSignIn::instance()->setFirebaseID(__QTFIREBASE_ID);
    }

    startInit();
}

//Google SHA1, gradle

QtFirebaseAuth::~QtFirebaseAuth()
{
    self = 0;
}

void QtFirebaseAuth::clearError()
{
    setError(ErrorNone);
}

void QtFirebaseAuth::setError(int errId, const QString &errMsg)
{
    m_errId = errId;
    m_errMsg = errMsg;
}

void QtFirebaseAuth::registerUser(const QString &email, const QString &pass)
{
    if(running())
        return;

    clearError();
    setComplete(false);
    m_action = ActionRegister;
    firebase::Future<auth::User*> future =
           m_auth->CreateUserWithEmailAndPassword(email.toUtf8().constData(), pass.toUtf8().constData());
    qFirebase->addFuture(__QTFIREBASE_ID + QStringLiteral(".auth.register"), future);
}

void QtFirebaseAuth::deleteUser()
{
    if(running())
        return;
    if (!signedIn())
        return;

    m_action = ActionDeleteUser;
    clearError();
    setComplete(false);

    firebase::Future<void> future = m_auth->current_user()->Delete();
    qFirebase->addFuture(__QTFIREBASE_ID + QStringLiteral(".auth.deleteUser"), future);
}

void QtFirebaseAuth::sendPasswordResetEmail(const QString &email)
{
    if(running())
        return;

    clearError();
    setComplete(false);
    firebase::Future<void> future =
           m_auth->SendPasswordResetEmail(email.toUtf8().constData());
    qFirebase->addFuture(__QTFIREBASE_ID + QStringLiteral(".auth.resetEmail"), future);
}


bool QtFirebaseAuth::signedIn() const
{
    return m_signedIn;
}

void QtFirebaseAuth::signIn(const QString &email, const QString &pass)
{
    preSignIn();

    firebase::Future<auth::User*> future =
                  m_auth->SignInWithEmailAndPassword(email.toUtf8().constData(), pass.toUtf8().constData());

    qFirebase->addFuture(__QTFIREBASE_ID + QStringLiteral(".auth.signin"), future);
}

bool QtFirebaseAuth::running() const
{
    return !m_complete;
}

void QtFirebaseAuth::signOut()
{
    m_action = ActionSignOut;
    m_auth->SignOut();
    clearError();
    setComplete(false);
    setSignIn(false);
    setComplete(true);
}

int QtFirebaseAuth::errorId() const
{
    return m_errId;
}

QString QtFirebaseAuth::errorMsg() const
{
    return m_errMsg;
}

void QtFirebaseAuth::preSignIn()
{
    if(running())
        return;

    m_action = ActionSignIn;
    clearError();
    setComplete(false);

    if(signedIn())
    {
        signOut();
    }
}

void QtFirebaseAuth::googleSignIn()
{
    preSignIn();

    QtFirebaseSocialSignIn::instance()->googleLogIn();
}

void QtFirebaseAuth::facebookSignIn()
{
    preSignIn();

    QtFirebaseSocialSignIn::instance()->facebookLogIn();
}

QString QtFirebaseAuth::email() const
{
    if(signedIn())
    {
        return QString::fromUtf8(m_auth->current_user()->email().c_str());
    }
    return QString();
}

QString QtFirebaseAuth::displayName() const
{
    if(signedIn())
    {
        return QString::fromUtf8(m_auth->current_user()->display_name().c_str());
    }
    return QString();
}

bool QtFirebaseAuth::emailVerified() const
{
    if(signedIn())
    {
        return m_auth->current_user()->is_email_verified();
    }
    return false;
}

QString QtFirebaseAuth::photoUrl() const
{
    if(signedIn())
    {
        return QString::fromUtf8(m_auth->current_user()->photo_url().c_str());
    }
    return QString();
}

QString QtFirebaseAuth::uid() const
{
    if(signedIn())
    {
        return QString::fromUtf8(m_auth->current_user()->uid().c_str());
    }
    return QString();
}

QString QtFirebaseAuth::id() const
{
    if(signedIn())
    {
        return QString::fromUtf8(m_auth->current_user()->provider_id().c_str());
    }
    return QString();
}

QString QtFirebaseAuth::number() const
{
    if(signedIn())
    {
        return QString::fromUtf8(m_auth->current_user()->phone_number().c_str());
    }
    return QString();
}

void QtFirebaseAuth::setComplete(bool complete)
{
    if(m_complete!=complete)
    {
        m_complete = complete;
        emit runningChanged();
        if(m_complete)
            emit completed(m_errId == ErrorNone, m_action);
    }
}

void QtFirebaseAuth::setSignIn(bool value)
{
    if(m_signedIn!=value)
    {
        m_signedIn = value;
        emit signedInChanged();
    }
}

void QtFirebaseAuth::init()
{
    if(!qFirebase->ready()) {
        // NOTE using "self" pointer with qDebug() sometimes lead to crashes during life-cycle:
        // init -> terminate -> init -> crash
        qDebug() << this << "::init" << "base not ready";
        return;
    }

    if(!ready() && !initializing()) {
        setInitializing(true);
        m_auth = auth::Auth::GetAuth(qFirebase->firebaseApp());
        qDebug() << this << "::init" << "native initialized";
        setInitializing(false);
        setReady(true);

        auth::User* user = m_auth->current_user();
        if(user!=nullptr)
            setSignIn(true);
        else
            setSignIn(false);
        setComplete(true);
    }

    m_listener = new SmsListener();

    connect(m_listener, &SmsListener::verificationCompleted, this, &QtFirebaseAuth::smsVerification);
    connect(m_listener, &SmsListener::verificationFailed, this, &QtFirebaseAuth::smsVerificationError);
    connect(m_listener, &SmsListener::codeSent, this, &QtFirebaseAuth::codeSent);
}

void QtFirebaseAuth::onFutureEvent(QString eventId, firebase::FutureBase future)
{

    if(!eventId.startsWith(__QTFIREBASE_ID + QStringLiteral(".auth")))
        return;

    qDebug() << this << "::onFutureEvent" << eventId;

    if(future.status() != firebase::kFutureStatusComplete)
    {
        qDebug() << this << "::onFutureEvent register user failed." << "ERROR: Action failed with error code and message: " << future.error() << future.error_message();
        setError(ErrorFailure, QStringLiteral("Unknown error"));
    }
    else if(future.error()==auth::kAuthErrorNone)
    {
        clearError();
        if(eventId == __QTFIREBASE_ID + QStringLiteral(".auth.register"))
        {
            if(future.result_void() == nullptr)
            {
                setError(ErrorFailure, QStringLiteral("Registered user is null"));
                qDebug() << "Registered user is null";
            }
            else
            {
                auth::User* user = result<auth::User*>(future.result_void())
                                                 ? *(result<auth::User*>(future.result_void()))
                                                 : nullptr;
                if(user!=nullptr)
                {
                    qFirebase->addFuture(__QTFIREBASE_ID + QStringLiteral(".auth.sendemailverify"), user->SendEmailVerification());
                }
            }
        }
        else if(eventId == __QTFIREBASE_ID + QStringLiteral(".auth.sendemailverify"))
        {
            qDebug() << this << "::onFutureEvent Verification email sent successfully";
        }
        else if(eventId == __QTFIREBASE_ID + QStringLiteral(".auth.deleteUser"))
        {
            qDebug() << this << "::onFutureEvent Delete user successfully";
            setSignIn(false);
        }

        else if(eventId == __QTFIREBASE_ID + QStringLiteral(".auth.resetEmail"))
        {
            emit passwordResetEmailSent();
            qDebug() << this << "::onFutureEvent reset email sent successfully";
        }
        else if(eventId == __QTFIREBASE_ID + QStringLiteral(".auth.signin"))
        {

            qDebug() << this << "::onFutureEvent Sign in successful";
            auth::User* user = result<auth::User*>(future.result_void())
                                             ? *(result<auth::User*>(future.result_void()))
                                             : nullptr;
            if(user!=nullptr)
            {
                setSignIn(true);
                /*qDebug() << "Email:" << user->email().c_str();
                qDebug() << "Display name:" << user->display_name().c_str();
                qDebug() << "Photo url:" << user->photo_url().c_str();
                qDebug() << "provider_id:" << user->provider_id().c_str();
                qDebug() << "is_anonymous:" << user->is_anonymous();
                qDebug() << "is_email_verified:" << user->is_email_verified();*/
            }
        }
        else if(eventId == __QTFIREBASE_ID + QStringLiteral(".auth.smsSignin"))
        {
            qDebug() << this << "::onFutureEvent SMS Sign in successful";
            auth::User* user = result<auth::User*>(future.result_void())
                                             ? *(result<auth::User*>(future.result_void()))
                                             : nullptr;
            if(user!=nullptr)
            {
                setSignIn(true);

                qDebug() << "Phone number: " << user->phone_number().c_str();
                qDebug() << "Phone provider uid: " << user->uid().c_str();
                qDebug() << "Phone provider ID: " << user->provider_id().c_str();
            }
        }
        else if(eventId == __QTFIREBASE_ID + QStringLiteral(".auth.socialsignin"))
        {
            qDebug() << this << "::onFutureEvent social Sign in successful";
            auth::User* user = result<auth::User*>(future.result_void())
                                             ? *(result<auth::User*>(future.result_void()))
                                             : nullptr;
            if(user!=nullptr)
            {
                setSignIn(true);

                qDebug() << "Social Name: " << QString::fromStdString(user->display_name());
                qDebug() << "Social Email: " << QString::fromStdString(user->email());
                qDebug() << "Social Phone Number: " << QString::fromStdString(user->phone_number());
                qDebug() << "Social Photo Url: " << QString::fromStdString(user->photo_url());
                qDebug() << "Social UID: " << QString::fromStdString(user->uid());
                qDebug() << "Social provider ID: " << QString::fromStdString(user->provider_id());
            }
        }
    }
    else
    {
        if(eventId == __QTFIREBASE_ID + QStringLiteral(".auth.register"))
        {
            qDebug() << this << "::onFutureEvent Registering user completed with error:" << future.error() << future.error_message();
        }
        else if(eventId == __QTFIREBASE_ID + QStringLiteral(".auth.sendemailverify"))
        {
            qDebug() << this << "::onFutureEvent Verification email send error:" << future.error() << future.error_message();
        }
        else if(eventId == __QTFIREBASE_ID + QStringLiteral(".auth.signin"))
        {
            setSignIn(false);
            qDebug() << this << "::onFutureEvent Sign in error:" << future.error() << future.error_message();
        }
        else if(eventId == __QTFIREBASE_ID + QStringLiteral(".auth.resetEmail"))
        {
            qDebug() << this << "::onFutureEvent reset email error" << future.error() << future.error_message();
        }
        else if(eventId == __QTFIREBASE_ID + QStringLiteral(".auth.deleteUser"))
        {
            qDebug() << this << "::onFutureEvent Delete user error" << future.error() << future.error_message();
        }
        else if(eventId == __QTFIREBASE_ID + QStringLiteral(".auth.socialsignin"))
        {
            qDebug() << this << "::onFutureEvent Social SignIn error" << future.error() << future.error_message();
        }
        setError(future.error(), QString::fromUtf8(future.error_message()));
    }
    setComplete(true);
}


void QtFirebaseAuth::smsSignIn(const QString &phone_number)
{
    qDebug() << "smsSignIn";

    auth::PhoneAuthProvider& phone_provider = auth::PhoneAuthProvider::GetInstance(m_auth);
    phone_provider.VerifyPhoneNumber(phone_number.toUtf8().constData(), 120, NULL,
                                      m_listener);
}

void QtFirebaseAuth::resendSms()
{
    qDebug() << "resendSms";

    auth::PhoneAuthProvider& phone_provider = auth::PhoneAuthProvider::GetInstance(m_auth);
    phone_provider.VerifyPhoneNumber(m_listener->getPhoneNumber().toUtf8().constData(), 120, m_listener->getToken(),
                                      m_listener);
}

void QtFirebaseAuth::codeReceived(const QString &code)
{
    if(running())
        return;

    if (m_listener->verificationID().isEmpty()) {
        setComplete(false);
        setError(ErrorFailure, QStringLiteral("Enter the phone number first"));
        setComplete(true);
        return;
    }

    m_mutex.lock();
    auth::PhoneAuthProvider& phone_provider = auth::PhoneAuthProvider::GetInstance(m_auth);
    auth::Credential credential = phone_provider.GetCredential(m_listener->verificationID().toUtf8().constData(), code.toUtf8().constData());
    m_mutex.unlock();

    smsVerification(credential);
}

void QtFirebaseAuth::smsVerification(auth::Credential credential)
{
    m_mutex.lock();

    qDebug() << "smsVerification - Credential is: " << credential.is_valid();

    if(running())
        return;

    m_action = ActionSignIn;
    clearError();
    setComplete(false);

    if(signedIn())
    {
        signOut();
    }

    firebase::Future<auth::User*> future = m_auth->SignInWithCredential(credential);
    qFirebase->addFuture(__QTFIREBASE_ID + QStringLiteral(".auth.smsSignin"), future);

    m_mutex.unlock();
}

void QtFirebaseAuth::smsVerificationError(const QString &errorMsg)
{
    setComplete(false);
    setError(ErrorFailure, errorMsg);
    setComplete(true);
}

SmsListener::SmsListener(QObject* parent)
    : QObject(parent)
{
}

void SmsListener::OnVerificationCompleted(auth::Credential credential)
{
    emit verificationCompleted(credential);
}

void SmsListener::OnVerificationFailed(const std::string& error)
{
    qDebug() << "Verification Failed: " << QString::fromStdString(error);
    setPhoneNumber("");
    verificationFailed(QString::fromStdString(error));
}

void SmsListener::OnCodeSent(const std::string& verification_id,
                             const firebase::auth::PhoneAuthProvider::ForceResendingToken &force_resending_token)
{
    m_mutex.lock();
    qDebug() << "OnCodeSent";

    setVerificationID(QString::fromStdString(verification_id));
    setToken(force_resending_token);

    m_mutex.unlock();

    emit codeSent();
}

QString SmsListener::getPhoneNumber() const
{
    return m_phoneNumber;
}

firebase::auth::PhoneAuthProvider::ForceResendingToken* SmsListener::getToken()
{
    return &m_token;
}

void SmsListener::setVerificationID(const QString &id)
{
    m_verificationID = id;
}

void SmsListener::setPhoneNumber(const QString &number)
{
    m_phoneNumber = number;
}

void SmsListener::setToken(const firebase::auth::PhoneAuthProvider::ForceResendingToken &token)
{
    m_token = token;
}

QString SmsListener::verificationID() const
{
    return m_verificationID;
}
