#include <android/log.h>
#include <dlfcn.h>
#include <jni.h>

#define TAG "PGOHOOK431"
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__)

typedef jint (*jni_onload_t)(JavaVM *, void *);

__attribute__((visibility("default")))
jint JNI_OnLoad(JavaVM *vm, void *reserved) {
    void *orig = dlopen("libmain_orig.so", RTLD_NOW);
    if (!orig) {
        LOGE("dlopen libmain_orig.so failed: %s", dlerror());
        return -1;
    }
    jni_onload_t fn = (jni_onload_t)dlsym(orig, "JNI_OnLoad");
    if (!fn) {
        LOGE("JNI_OnLoad missing in libmain_orig.so: %s", dlerror());
        return -1;
    }
    /* A hook failure must not stop the game from starting. */
    if (!dlopen("libpgo_hook.so", RTLD_NOW))
        LOGE("dlopen libpgo_hook.so failed: %s", dlerror());
    return fn(vm, reserved);
}
