#include <jni.h>
#include <string>
#include <android/log.h>

#define LOG_INFO(...) __android_log_print(ANDROID_LOG_INFO, "moneyGrabber_ndk", __VA_ARGS__)

#include <spdlog/spdlog.h>
#include "spdlog/sinks/android_sink.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/des.h"

mbedtls_entropy_context entropy;
mbedtls_ctr_drbg_context ctr_drbg;
char *personalization = "grabber-sample-app";

#define SLOG_INFO(...) android_logger->info( __VA_ARGS__ )
auto android_logger = spdlog::android_logger_mt("android", "fclient_ndk");

extern "C" JNIEXPORT jstring JNICALL
Java_com_millioner_moneygrabber_MainActivity_stringFromJNI(
        JNIEnv* env,
        jobject /* this */) {
    std::string hello = "Hello from C++";
    LOG_INFO("Hello from system log %d", 2021);
    SLOG_INFO("Hello from spdlog log {}", 2021);
    return env->NewStringUTF(hello.c_str());
}

extern "C" JNIEXPORT jint JNICALL
Java_com_millioner_moneygrabber_MainActivity_initRng(
        JNIEnv *env,
        jclass){
    mbedtls_entropy_init( &entropy );
    mbedtls_ctr_drbg_init( &ctr_drbg );

    return mbedtls_ctr_drbg_seed( &ctr_drbg, mbedtls_entropy_func, &entropy, (const unsigned char *) personalization, strlen ( personalization ) );
}

extern "C" JNIEXPORT jbyteArray JNICALL
Java_com_millioner_moneygrabber_MainActivity_randomBytes(JNIEnv *env, jclass, jint no){
    uint8_t * buf = new uint8_t [no];
    mbedtls_ctr_drbg_random(&ctr_drbg, buf, no);
    jbyteArray rnd = env->NewByteArray(no);
    env->SetByteArrayRegion(rnd, 0, no, (jbyte *)buf);
    delete [] buf;
    return rnd;
}

extern "C" JNIEXPORT jbyteArray JNICALL
Java_com_millioner_moneygrabber_MainActivity_encrypt(
        JNIEnv *env,
        jclass clazz,
        jbyteArray key,
        jbyteArray data ) {
    jsize keySize = env->GetArrayLength(key);
    jsize dataSize = env->GetArrayLength(data);
    SLOG_INFO("Encrypt: {} {}", keySize, dataSize);
    if(keySize != 16 || dataSize <= 0) {
        return env->NewByteArray(0);
    }
    mbedtls_des3_context ctx;
    mbedtls_des3_init(&ctx);
    jbyte *pKey = env->GetByteArrayElements(key, 0);
    int rst = dataSize % 8;
    int sz = dataSize + 8 - rst;
    std::unique_ptr<uint8_t[]> buf = std::make_unique<uint8_t[]>(sz);
    for (int i = 7; i > rst; --i) {
        buf[dataSize + i] = rst;
    }
    jbyte *pData = env->GetByteArrayElements(data, 0);
    std::copy(pData, pData + dataSize, buf.get());
    mbedtls_des3_set2key_enc(&ctx, reinterpret_cast<uint8_t*>(pKey));
    int cn = sz / 8;
    for (int i = 0; i < cn; ++i) {
        mbedtls_des3_crypt_ecb(&ctx, buf.get() + i * 8, buf.get() + i * 8);
    }
    jbyteArray dout = env->NewByteArray(sz);
    env->SetByteArrayRegion(dout, 0, sz, reinterpret_cast<jbyte *>(buf.get()));
    env->ReleaseByteArrayElements(key, pKey, 0);
    env->ReleaseByteArrayElements(data, pData, 0);
    return dout;
}

extern "C" JNIEXPORT jbyteArray JNICALL
Java_com_millioner_moneygrabber_MainActivity_decrypt(
        JNIEnv *env,
        jclass clazz,
        jbyteArray key,
        jbyteArray data ) {
    jsize keySize = env->GetArrayLength(key);
    jsize dataSize = env->GetArrayLength(data);
    SLOG_INFO("Decrypt: {} {}", keySize, dataSize);
    if(keySize != 16 || dataSize <= 0 || (dataSize % 8) != 0) {
        return env->NewByteArray(0);
    }
    mbedtls_des3_context ctx;
    mbedtls_des3_init(&ctx);
    jbyte *pKey = env->GetByteArrayElements(key, 0);
    jbyte *pData = env->GetByteArrayElements(data, 0);
    std::unique_ptr<uint8_t[]> buf = std::make_unique<uint8_t[]>(dataSize);
    std::copy(pData, pData + dataSize, buf.get());
    mbedtls_des3_set2key_dec(&ctx, reinterpret_cast<uint8_t*>(pKey));
    int cn = dataSize / 8;
    for (int i = 0; i < cn; ++i) {
        mbedtls_des3_crypt_ecb(&ctx, buf.get() + i * 8, buf.get() + i * 8);
    }
    int sz = dataSize - 8 + buf[dataSize - 1];
    jbyteArray dout = env->NewByteArray(cn * 8);
    env->SetByteArrayRegion(dout, 0, cn * 8, reinterpret_cast<jbyte *>(buf.get()));
    env->ReleaseByteArrayElements(key, pKey, 0);
    env->ReleaseByteArrayElements(data, pData, 0);
    return dout;
}