#include <iostream>
#include <dlfcn.h>
#include <unistd.h>

typedef int32_t MagicCookie;
typedef MagicCookie LVUserEventRef;
typedef int (*PostLVUserEvent_T)(LVUserEventRef ref, void* data);
// Function pointer type for pass_LV_ref_to_shared_mem
typedef int32_t (*PassLVRefToSharedMem_T)(LVUserEventRef*);

int main() {
    // Load the shared library containing pass_LV_ref_to_shared_mem
    void* sharedLib = dlopen("/home/admin/OpenDDS/DevGuideExamples/DCPS/Messenger/build/libpassLVRefToSharedMem.so", RTLD_LAZY);

    if (!sharedLib) {
        std::cerr << "Error loading shared library: " << dlerror() << std::endl;
        return 1;
    }

    // Get a pointer to the pass_LV_ref_to_shared_mem function
    PassLVRefToSharedMem_T passLVRefToSharedMem = reinterpret_cast<PassLVRefToSharedMem_T>(dlsym(sharedLib, "pass_LV_ref_to_shared_mem"));

    if (!passLVRefToSharedMem) {
        std::cerr << "Error resolving function: " << dlerror() << std::endl;
        return 1;
    }

    // Call pass_LV_ref_to_shared_mem
    LVUserEventRef ref;// = nullptr; // Initialize with a valid reference if needed
    int32_t result = passLVRefToSharedMem(&ref);

    if (result != 0) {
        std::cerr << "Error in pass_LV_ref_to_shared_mem: " << result << std::endl;
        return 1;
    }

    // Unload the shared library
    dlclose(sharedLib);

    return 0;
}