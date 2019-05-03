#ifndef QTFIREBASE_SOCIAL_SIGNIN_H
#define QTFIREBASE_SOCIAL_SIGNIN_H

#include "qtfirebaseservice.h"
#include "firebase/app.h"
#include "firebase/auth.h"

#include "platformutils.h"

#ifdef QTFIREBASE_SOCIAL_SIGNIN_H
#include "src/qtfirebase.h"
#if defined(qFirebaseSocialSignIn)
#undef qFirebaseSocialSignIn
#endif
#define qFirebaseSocialSignIn (static_cast<QtFirebaseSocialSignIn *>(QtFirebaseSocialSignIn::instance()))

namespace auth = ::firebase::auth;

class QtFirebaseSocialSignIn : public QObject
{
    Q_OBJECT

public:
    static QtFirebaseSocialSignIn *instance()
    {
        if(self == nullptr)
        {
            self = new QtFirebaseSocialSignIn();
            qDebug() << self << "::instance" << "singleton";
        }
        return self;
    }

    inline void setFirebaseID(const QString &id) { __QTFIREBASE_ID = id; }
#ifdef Q_OS_ANDROID
    inline void googleLogIn() { QtAndroid::androidActivity().callMethod<void>("googleSignIn"); }
    inline void facebookLogIn() { QtAndroid::androidActivity().callMethod<void>("facebookLogIn"); }
#endif

    inline void googleLogIn() {  }
    inline void facebookLogIn() { PlatformUtils::facebookLogin();}

    void googleSignIn(const QString &accessToken);
    void facebookSignIn(const QString &accessToken);

protected:
    explicit QtFirebaseSocialSignIn(QObject *parent = 0);

private:

    static QtFirebaseSocialSignIn *self;
    firebase::auth::Auth* m_auth;
    QString __QTFIREBASE_ID;

    Q_DISABLE_COPY(QtFirebaseSocialSignIn)
};

#endif //QTFIREBASE_SOCIAL_SIGNIN_H

#endif // QTFIREBASE_SOCIAL_SIGNIN_H
