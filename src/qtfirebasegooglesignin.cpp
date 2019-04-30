#include "qtfirebasegooglesignin.h"

#include <QDebug>
#include <QtAndroid>
#include <QAndroidJniEnvironment>

namespace auth = ::firebase::auth;

QtFirebaseGoogleSignIn *QtFirebaseGoogleSignIn::self = 0;

QtFirebaseGoogleSignIn::QtFirebaseGoogleSignIn(QObject *parent) : QObject(parent),
    m_auth(nullptr)
{
    if(self == 0)
    {
        self = this;
        __QTFIREBASE_ID = QString().sprintf("%8p", this);
        qDebug() << self << "::QtFirebaseGoogleSignIn" << "singleton";
    }
}

//self vs this

void QtFirebaseGoogleSignIn::login()
{
    QtAndroid::androidActivity().callMethod<void>("googleSignIn");
}


void QtFirebaseGoogleSignIn::firebaseSignIn(const QString &accessToken)
{
    if(!m_auth) {
        firebase::App* app = qFirebase->firebaseApp();
        firebase::InitResult* ir = nullptr;
        m_auth = firebase::auth::Auth::GetAuth(app, ir);
    }

    auth::Credential credential = auth::GoogleAuthProvider::GetCredential(accessToken.toUtf8(), nullptr);

    qDebug() << "credential = " << credential.is_valid();

    //if (resultCode == Activity.RESULT_OK)

    qFirebase->addFuture(__QTFIREBASE_ID  + QStringLiteral(".auth.googlesignin"), m_auth->SignInWithCredential(credential));
}

//TODO m_smThg delete empty comments, rename some variables, nullptr, get number and other things, const QString, check tokens. FirebaseService? oauthid to xml gradle instuctions delete clientid

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT void JNICALL
Java_com_qtfirebase_auth_QtFirebaseAuthActivity_firebaseSignIn(JNIEnv *env, jobject obj, jstring token)
{
    Q_UNUSED(obj);

       const char *tokenStr = env->GetStringUTFChars(token, nullptr);

       qDebug() << "Qt got Google Sign-In Token: " + QString(tokenStr);

       QtFirebaseGoogleSignIn::instance()->firebaseSignIn(QString(tokenStr));

       env->ReleaseStringUTFChars(token, tokenStr);
       return;
}
#ifdef __cplusplus
}
#endif
