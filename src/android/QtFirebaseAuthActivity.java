package com.qtfirebase.auth;

import android.app.Activity;
import android.content.Intent;
import android.util.Log;

import java.util.Arrays;
import java.lang.Enum;

import com.facebook.CallbackManager;
import com.facebook.FacebookCallback;
import com.facebook.FacebookException;
import com.facebook.FacebookSdk;
import com.facebook.login.LoginResult;
import com.facebook.login.LoginManager;
import com.facebook.login.LoginBehavior;


import com.google.android.gms.auth.api.signin.GoogleSignIn;
import com.google.android.gms.auth.api.signin.GoogleSignInAccount;
import com.google.android.gms.auth.api.signin.GoogleSignInClient;
import com.google.android.gms.auth.api.signin.GoogleSignInOptions;
import com.google.android.gms.common.api.ApiException;
import com.google.android.gms.tasks.Task;

public class QtFirebaseAuthActivity {
    //Main activity should transfer intents to googlesignin
    private static final String TAG = "QtFirebaseAuthActivity";
    public static final int QFGoogleSignIn = 379056123;

    private CallbackManager m_callbackManager;
    private Activity m_qtActivity;
    private GoogleSignInClient m_googleSignInClient;

    public enum socialType
    {
        GOOGLE,
        FACEBOOK
    }

    public QtFirebaseAuthActivity(Activity qtActivity)
    {
        m_qtActivity = qtActivity;

        m_callbackManager = CallbackManager.Factory.create();

        LoginManager.getInstance().registerCallback(m_callbackManager, new FacebookCallback<LoginResult>() {
            @Override
            public void onSuccess(LoginResult loginResult) {
                Log.d(TAG, "facebook:onSuccess: " + loginResult);
                firebaseSignIn(loginResult.getAccessToken().getToken(), socialType.FACEBOOK.ordinal());
            }
            @Override
            public void onCancel() {
                Log.d(TAG, "facebook:onCancel");
            }

            @Override
            public void onError(FacebookException error) {
                Log.d(TAG, "facebook:onError ", error);
            }
        });
        LoginManager.getInstance().setLoginBehavior(LoginBehavior.NATIVE_WITH_FALLBACK);
    }

    public void googleSignIn()
    {
        GoogleSignInOptions gso = new GoogleSignInOptions.Builder(GoogleSignInOptions.DEFAULT_SIGN_IN)
                .requestIdToken("404440643421-lbasejr6a7qasqist87pr62vh842jlgv.apps.googleusercontent.com") //from json
                .requestEmail()
                .build();

        m_googleSignInClient = GoogleSignIn.getClient(m_qtActivity.getApplicationContext(), gso);

        Intent signInIntent = m_googleSignInClient.getSignInIntent();
        m_qtActivity.startActivityForResult(signInIntent, QFGoogleSignIn);

    }

    public void facebookLogIn()
    {
        LoginManager.getInstance().logInWithReadPermissions(m_qtActivity, Arrays.asList("public_profile", "user_friends"));
    }

    public void passToCallbackManager(int requestCode, int resultCode, Intent data)
    {
        m_callbackManager.onActivityResult(requestCode, resultCode, data);
    }

    public void activityResult(Intent data) {
        Task<GoogleSignInAccount> task = GoogleSignIn.getSignedInAccountFromIntent(data);
        try {
            GoogleSignInAccount account = task.getResult(ApiException.class);
            firebaseSignIn(account.getIdToken(), socialType.GOOGLE.ordinal());
        } catch (ApiException e) {
            Log.w(TAG, "GoogleSignIn failed", e);
        }
    }

    public static native void firebaseSignIn(String userToken, int socialType);
}
