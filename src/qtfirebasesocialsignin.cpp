#include "qtfirebasesocialsignin.h"
#include "qtfirebaseauth.h"

#include <QDebug>

#include <QThread>
#ifdef Q_OS_ANDROID
#include <QtAndroid>
#include <QAndroidJniEnvironment>
#endif

namespace auth = ::firebase::auth;

QtFirebaseSocialSignIn *QtFirebaseSocialSignIn::self = 0;

enum SocialType
{
    GOOGLE,
    FACEBOOK
};

QtFirebaseSocialSignIn::QtFirebaseSocialSignIn(QObject *parent) : QObject(parent),
    m_auth(nullptr), __QTFIREBASE_ID("")
{
    if(self == 0)
    {
        self = this;
        qDebug() << self << "::QtFirebaseSocialSignIn" << "singleton " << QThread::currentThreadId();
    }
}

//self vs this

//extra parameters for socials
void QtFirebaseSocialSignIn::googleSignIn(const QString &accessToken)
{
    if(!m_auth)
        m_auth = firebase::auth::Auth::GetAuth(qFirebase->firebaseApp());

    auth::Credential credential = auth::GoogleAuthProvider::GetCredential(accessToken.toUtf8(), nullptr);

    qDebug() << "credential = " << credential.is_valid();

    qDebug() << "FDSA THREAD googlesignin " << QThread::currentThreadId();
    qFirebase->addFuture(__QTFIREBASE_ID  + QStringLiteral(".auth.socialsignin"), m_auth->SignInWithCredential(credential));
}

void QtFirebaseSocialSignIn::facebookSignIn(const QString &accessToken)
{
    if(!m_auth) {
        firebase::App* app = qFirebase->firebaseApp();
        firebase::InitResult* ir = nullptr;
        m_auth = firebase::auth::Auth::GetAuth(app, ir);
    }

    auth::Credential credential = auth::FacebookAuthProvider::GetCredential(accessToken.toUtf8());

    qDebug() << "credential = " << credential.is_valid();

    //socialsignin -> facebook
    qFirebase->addFuture(__QTFIREBASE_ID  + QStringLiteral(".auth.socialsignin"), m_auth->SignInWithCredential(credential));
}
//TODO set signin in auth, m_smThg delete empty comments, rename some variables, nullptr, get number and other things, const QString, check tokens. FirebaseService? oauthid to xml gradle instuctions
#ifdef Q_OS_ANDROID
#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT void JNICALL
Java_com_qtfirebase_auth_QtFirebaseAuthActivity_firebaseSignIn(JNIEnv *env, jobject obj, jstring token, jint socialType)
{
    Q_UNUSED(obj);

    const char *tokenStr = env->GetStringUTFChars(token, nullptr);

    qDebug() << "Qt got LogIn Token: " + QString(tokenStr);

    switch(SocialType(socialType))
    {
    case GOOGLE:
        QtFirebaseSocialSignIn::instance()->googleSignIn(QString(tokenStr));
        break;
    case FACEBOOK:
        QtFirebaseSocialSignIn::instance()->facebookSignIn(QString(tokenStr));
        break;
    }

    env->ReleaseStringUTFChars(token, tokenStr);
    return;
}
#ifdef __cplusplus
}
#endif
#endif
