#!/bin/bash

curl --location https://firebase.google.com/download/ios --output "firebase_ios.zip"
curl --location https://origincache.facebook.com/developers/resources/?id=FacebookSDKs-iOS-4.40.0.zip --output "facebook_ios.zip"
unzip firebase_ios.zip
unzip facebook_ios.zip -d $PWD/Firebase/Auth/Facebook
rm firebase_ios.zip
rm facebook_ios.zip
