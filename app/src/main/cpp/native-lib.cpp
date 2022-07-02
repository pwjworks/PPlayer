#include <jni.h>
#include <string>
#include <android/log.h>
#include "FFDemux.h"
#include "XLog.h"
#include "FFDecode.h"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
}

class TestObs : public IObserver {
public:
    void Update(XData d) override {
        XLOGI("TestObs Updata data size is %d", d.size);

    }
};

static double r2d(AVRational r) {
    return r.num == 0 || r.den == 0 ? 0. : (double) r.num / r.den;
}

extern "C" JNIEXPORT jstring JNICALL
Java_com_pwjworks_pplayer_MainActivity_stringFromJNI(
        JNIEnv *env,
        jobject /* this */) {
    std::string hello = "Hello from C++";

    hello += avcodec_configuration();

    auto *tobs = new TestObs();

    auto *ffdemux = new FFDemux();
    // ffdemux->AddObs(tobs);
    ffdemux->Open("/sdcard/20051210-w50s.flv");
    ffdemux->Read();
    IDecode *vdecode = new FFDecode();
    vdecode->Open(ffdemux->GetVPara());

    IDecode *adecode = new FFDecode();
    adecode->Open(ffdemux->GetAPara());

    ffdemux->AddObs(vdecode);
    ffdemux->AddObs(adecode);

    ffdemux->Start();

    adecode->Start();
    vdecode->Start();
//    XSleep(100);
//    ffdemux->Stop();
//    for (int i = 0; i < 30; i++) {
//        XData d = ffdemux->Read();
//        XLOGI("Read data size is %d", d.size);
//    }
    return env->NewStringUTF(hello.c_str());
}
extern "C"
JNIEXPORT jboolean JNICALL
Java_com_pwjworks_pplayer_MainActivity_Open(JNIEnv *env, jobject thiz, jstring url,
                                            jobject handle) {
    const char *url_ = env->GetStringUTFChars(url, nullptr);
    printf("%s", url_);
    FILE *fp = fopen(url_, "rb");
    if (!fp) {
        XLOGW("%s open failed!", url_);
    } else {
        XLOGW("%s open success!", url_);
        fclose(fp);
    }
    env->ReleaseStringUTFChars(url, url_);
    return static_cast<jboolean>(true);
}