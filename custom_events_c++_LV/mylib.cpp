#include <iostream>
#include <map>
#include <memory>
#include <mutex>
#include <cstring>
#include <memory>
#include <dlfcn.h>
#include <unistd.h>
#include <stdio.h>

int32_t test_callback_pointer(unsigned int *ref){
int i = 0;
    // while (1) {
    // usleep(1000000);
    void* libraryHandle = dlopen("/usr/local/natinst/labview/liblvrt.so", RTLD_LAZY);

    if (libraryHandle) {
        // The library is loaded, and libraryHandle contains a handle to it

        // Now, you can initialize the function pointers as you did in your code
        typedef int32_t MagicCookie;
        typedef MagicCookie LVUserEventRef;
        typedef int (*PostLVUserEvent_T)(LVUserEventRef ref, void* data);

        PostLVUserEvent_T PostLVUserEvent = (PostLVUserEvent_T)dlsym(libraryHandle, "PostLVUserEvent");

        if (PostLVUserEvent) {
            // You have successfully obtained the function pointer
            // You can call the function with appropriate arguments
            // int ref = 10;
            int i;
            for( i; i< 10; i++){
                
                uint64_t data = i;
                int result = PostLVUserEvent(*ref, (void*)&i);
                // i++;
                usleep(1000000);
                std::cout << result;
                // Handle the result as needed
                // return result;
            }
            return 0;
            } else {
            return 1;
            // Handle the case where PostLVUserEvent is not found
        }


        // Don't forget to close the library when done
        dlclose(libraryHandle);
    } else {
        return 1;
        // Handle the case where the library couldn't be loaded
    }
}




int myoccurance(unsigned int *ref){
    void* libraryHandle = dlopen("/usr/local/natinst/labview/liblvrt.so", RTLD_LAZY);
    typedef int32_t MagicCookie;
    typedef int (*Occur_T)(MagicCookie occurrence);
    static Occur_T Occur = nullptr;
    Occur = (Occur_T)dlsym(libraryHandle, "Occur");
    
    return Occur(*ref);
}

