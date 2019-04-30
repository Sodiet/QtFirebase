package com.qtfirebase.googlesignin;

import android.app.Activity;
import android.content.Intent;
import android.util.Log;

import com.google.android.gms.auth.api.signin.GoogleSignIn;
import com.google.android.gms.auth.api.signin.GoogleSignInAccount;
import com.google.android.gms.auth.api.signin.GoogleSignInClient;
import com.google.android.gms.auth.api.signin.GoogleSignInOptions;
import com.google.android.gms.common.api.ApiException;
import com.google.android.gms.tasks.Task;

public class GoogleSignInActivity extends org.qtproject.qt5.android.bindings.QtActivity { //AndroidNativeActivity  || Make QtFirebaseActivity, then FlipabitActivity extends QtFirebaseActivity

    private static final String TAG = "GoogleSignInActivity";

    // RC_SIGN_IN 987654321 this is a random number i picked. if you change it
    // do not forget to change it in your BaseActivity
    private static final int RC_SIGN_IN = 987654321;

    Activity m_qtActivity;
    String m_clientId;
	long m_handler;

    public GoogleSignInClient m_googleSignInClient;

    public GoogleSignInActivity(String clientId, long handler, Activity qtActivity)
    {
        m_handler = handler;
        m_qtActivity = qtActivity;
        m_clientId = clientId;
    }

    public void login() //separate file
    {
        // Configure Google Sign In
        GoogleSignInOptions gso = new GoogleSignInOptions.Builder(GoogleSignInOptions.DEFAULT_SIGN_IN)
                .requestIdToken("404440643421-lbasejr6a7qasqist87pr62vh842jlgv.apps.googleusercontent.com")
                .requestEmail()
                .build();

        m_googleSignInClient = GoogleSignIn.getClient(m_qtActivity.getApplicationContext(), gso);

        Intent signInIntent = m_googleSignInClient.getSignInIntent();
        m_qtActivity.startActivityForResult(signInIntent, RC_SIGN_IN);

    }

    public void activityResult(Intent data) {
        Task<GoogleSignInAccount> task = GoogleSignIn.getSignedInAccountFromIntent(data);
        try {
            // Google Sign In was successful, authenticate with Firebase
            GoogleSignInAccount account = task.getResult(ApiException.class);
            firebaseSignIn(account.getIdToken());
        } catch (ApiException e) {
            // Google Sign In failed
            Log.w(TAG, "Google sign in failed", e);
        }
    }

    public static native void firebaseSignIn(String userToken);
}