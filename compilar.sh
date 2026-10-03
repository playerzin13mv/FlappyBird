#!/usr/bin/env bash
set -e

echo "=== 1. Limpando compilações antigas ==="
cd ~/AFlappyBird
rm -rf build flappybird_unaligned.apk FlappyBird.apk /storage/emulated/0/Download/FlappyBird.apk

echo "=== 2. Detectando Arquitetura do Processador ==="
ABI=$(getprop ro.product.cpu.abi 2>/dev/null || echo "arm64-v8a")
echo "Arquitetura: $ABI"
mkdir -p build/assets "build/lib/$ABI" build/res/drawable

echo "=== 3. Definindo Framework Android ==="
ANDROID_JAR=$(find $PREFIX -name "android.jar" 2>/dev/null | head -n 1)

if [ -z "$ANDROID_JAR" ] || [ ! -f "$ANDROID_JAR" ]; then
    if [ -f "/system/framework/framework-res.apk" ]; then
        ANDROID_JAR="/system/framework/framework-res.apk"
    else
        echo "A descarregar android.jar..."
        curl -s -L -o ~/AFlappyBird/android.jar https://raw.githubusercontent.com/aapt2/android-jar/master/android-30/android.jar || wget -q -O ~/AFlappyBird/android.jar https://raw.githubusercontent.com/aapt2/android-jar/master/android-30/android.jar
        ANDROID_JAR="android.jar"
    fi
fi
echo "Utilizando framework: $ANDROID_JAR"

echo "=== 4. Copiando Ícone ==="
ICON_FOUND=""
for f in "/storage/emulated/0/Download/NOME_DO_SEU_ICONE.png" \
         "/storage/emulated/0/Download/Flappy-Bird-PNG-Photos.png" \
         $(find /storage/emulated/0/Download -iname "*.png" 2>/dev/null | head -n 1); do
    if [ -n "$f" ] && [ -f "$f" ]; then
        ICON_FOUND="$f"
        break
    fi
done

if [ -n "$ICON_FOUND" ]; then
    echo "Ícone localizado: $ICON_FOUND"
    cp "$ICON_FOUND" build/res/drawable/icon.png
elif [ -f "assets/bird.png" ]; then
    cp assets/bird.png build/res/drawable/icon.png
else
    echo "iVBORw0KGgoAAAANSUhEUgAAAAEAAAABCAYAAAAfFcSJAAAADUlEQVR42mNk+M9QDwADhgGAWjR9awAAAABJRU5ErkJggg==" | base64 -d > build/res/drawable/icon.png
fi

if [ -d "assets" ]; then
    cp -r assets/* build/assets/ 2>/dev/null || true
fi

echo "=== 5. Criando AndroidManifest.xml ==="
cat << 'MANIFEST' > build/AndroidManifest.xml
<?xml version="1.0" encoding="utf-8"?>
<manifest xmlns:android="http://schemas.android.com/apk/res/android"
    package="com.meujogo.flappybird"
    android:versionCode="1"
    android:versionName="1.0">

    <uses-sdk android:minSdkVersion="21" android:targetSdkVersion="33"/>

    <application 
        android:label="Flappy Bird"
        android:icon="@drawable/icon"
        android:hasCode="false">
        
        <activity android:name="android.app.NativeActivity"
            android:label="Flappy Bird"
            android:configChanges="orientation|keyboardHidden|screenSize"
            android:exported="true">
            <meta-data android:name="android.app.lib_name" android:value="main" />
            <intent-filter>
                <action android:name="android.intent.action.MAIN" />
                <category android:name="android.intent.category.LAUNCHER" />
            </intent-filter>
        </activity>
    </application>
</manifest>
MANIFEST

echo "=== 6. Compilando código C++ ==="
g++ -shared -fPIC main.cpp -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -o "build/lib/$ABI/libmain.so"

echo "=== 7. Empacotando APK com AAPT ==="
aapt package -f -M build/AndroidManifest.xml -S build/res -A build/assets -I "$ANDROID_JAR" -F flappybird_unaligned.apk

echo "=== 8. Inserindo biblioteca nativa ==="
cd build
aapt add ../flappybird_unaligned.apk "lib/$ABI/libmain.so"
cd ~/AFlappyBird

echo "=== 9. Chave de Assinatura ==="
if [ ! -f chave.keystore ]; then
    keytool -genkey -v -keystore chave.keystore -alias meujogo -keyalg RSA -keysize 2048 -validity 10000 -storepass 123456 -keypass 123456 -dname "CN=Me, OU=Dev, O=Game, L=City, ST=State, C=BR"
fi

echo "=== 10. Assinando o APK ==="
apksigner sign --ks chave.keystore --ks-pass pass:123456 --ks-key-alias meujogo --out FlappyBird.apk flappybird_unaligned.apk

echo "=== 11. Copiando para Downloads ==="
cp FlappyBird.apk /storage/emulated/0/Download/FlappyBird.apk

echo ""
echo "================================================="
echo " SUCESSO! APK GERADO COM SUCESSO!"
echo " Tamanho do arquivo final:"
ls -lh /storage/emulated/0/Download/FlappyBird.apk
echo "================================================="
