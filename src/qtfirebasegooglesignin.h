#ifndef QTFIREBASE_GOOGLE_SIGNIN_H
#define QTFIREBASE_GOOGLE_SIGNIN_H

#include "qtfirebaseservice.h"
#include "firebase/app.h"
#include "firebase/auth.h"

#ifdef QTFIREBASE_GOOGLE_SIGNIN_H
#include "src/qtfirebase.h"
#if defined(qFirebaseGoogleSignIn)
#undef qFirebaseGoogleSignIn
#endif
#define qFirebaseGoogleSignIn (static_cast<QtFirebaseGoogleSignIn *>(QtFirebaseGoogleSignIn::instance()))

// TODOS Android https://developers.google.com/+/web/api/rest/oauth
// Firebase console enable logins

namespace auth = ::firebase::auth;

class QtFirebaseGoogleSignIn : public QObject
{
    Q_OBJECT

public:
    static QtFirebaseGoogleSignIn *instance()
    {
        if(self == nullptr)
        {
            self = new QtFirebaseGoogleSignIn(0);
            qDebug() << self << "::instance" << "singleton";
        }
        return self;
    }

    void login();

    void firebaseSignIn(const QString &accessToken);

protected:
    explicit QtFirebaseGoogleSignIn(QObject *parent = 0);

private:

    static QtFirebaseGoogleSignIn *self;
    firebase::auth::Auth* m_auth;
    QString __QTFIREBASE_ID;

    Q_DISABLE_COPY(QtFirebaseGoogleSignIn)
};

#endif //QTFIREBASE_GOOGLE_SIGNIN_H

#endif // QTFIREBASE_GOOGLE_SIGNIN_H
