/**
 *  injector.c
 *  Programmed by Ju-yeong Park
 */
#define BINARY_VERSION 1

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <mach/mach_init.h>
#include <mach/mach_vm.h>
#include <sys/types.h>
#include <mach/mach.h>
#include <stdbool.h>

int replaceRemoteProcessMemory(pid_t pid, 
                void *data1, unsigned int size1, // To search
                void *data2, unsigned int size2  // Replacement
            ) {
    int result = 0;
#ifdef __APPLE__
    mach_port_name_t task;
    if(task_for_pid(mach_task_self(), pid, &task) != KERN_SUCCESS) {
        printf("task_for_pid() failed.\n");
        return -1;
    }
    vm_address_t iter;
    
    iter = 0;
    while (1) {
        vm_address_t addr = iter;
        vm_size_t size;
        vm_region_submap_info_data_64_t info;
        mach_msg_type_number_t info_count;
        unsigned int depth;
        kern_return_t kr;
    

        depth = 0;
        info_count = VM_REGION_SUBMAP_INFO_COUNT_64;
        kr = vm_region_recurse_64(task, &addr, &size, &depth, 
                                  (vm_region_info_t)&info, &info_count);
        if (kr) break;

        void *copied;
        mach_msg_type_number_t copied_size = 0;
        if(vm_read(task, addr, size, (vm_offset_t *)&copied, &copied_size) == KERN_SUCCESS) {
            char *ptr1 = (char*)copied;
            char *ptr2 = (char*)data1;
            char *ptr3 = (char*)data2;

            for(int i = 0; i < size-size1; i ++){
                bool isMatched = true;

                for(int j = 0; j < size1; j++){
                    if(*(ptr1+j) != *(ptr2+j) ){
                        isMatched = false;
                        break;
                    }
                }

                if(isMatched) {

                    printf("|  |___ Matched at 0x%llx\n", (long long)ptr1);
                    if(vm_write(task, addr+i, (vm_offset_t)ptr3, size2) != KERN_SUCCESS) {
                        printf("|  |___ Unable to write on the region\n");
                    }
                    result ++;
                }
                ptr1++;
            }
        } else { 
            //printf("|  |___ Unable to read the region\n");
        }
 
        iter = addr + size;
    }
    return result;
#else 
    #warning This platform is not supported.
    return -1;
#endif
}

#define RULE(name, data1, data2) printf("[%s] Patching...\n", #name); char name##1[] = data1;char name ## 2[] = data2;int name ## 3 = replaceRemoteProcessMemory(pid, name ## 1, sizeof(name ## 1), name ## 2, sizeof(name ## 2));if(name ## 3 > 0) { printf("[%s] Successfully patched (%d).\n", #name, name ## 3); }
#define CODE(args...) {args}


int main(int argc, char** argv) {
    pid_t pid = atoi(argv[1]); 
    printf("[+] the Binding of Isaac Hack Binary V%d By Ju-yeong Park <interruptz@gmail.com>\n", BINARY_VERSION);
    printf("* this hack provides: Infinite item, Infinite health\n");
    printf("PID: %d\n", pid);

    // the Binding of Isaac hack
    // Add your own hack here.
    //

    // RULE(INFINITE_HEALTH, CODE(0x96, 0x02, 0x00, 0x08, 0x0f, 0x1C, 0x96, 0x04, 0x00, 0x08, 0x4f, 0x08, 0x0f, 0x1C, 0x96, 0x02, 0x00, 0x08, 0x4f, 0x4E, 0x96, 0x02, 0x00, 0x04, 0x02, 0x0B, 0x4F), CODE(0x96, 0x02, 0x00, 0x08, 0x0f, 0x1C, 0x96, 0x04, 0x00, 0x08, 0x4f, 0x08, 0x0f, 0x1C, 0x96, 0x02, 0x00, 0x08, 0x4f, 0x4E, 0x96, 0x02, 0x00, 0x04, 0x02, 0x47, 0x4F));
    RULE(INFINITE_ITEM_USE, CODE(0x96, 0x0e, 0x00, 0x04, 0x02, 0x09, 0xda, 0x01, 0x06, 0x00, 0x00, 0x00, 0x00), CODE(0x96, 0x0e, 0x00, 0x04, 0x02, 0x09, 0xda, 0x01, 0x07, 0x01, 0x00, 0x00, 0x00));
    
    
    RULE(MAX_ITEMS, CODE(0x96, 0x15, 0x00, 0x04, 0x01, 0x08, 0x13, 0x04, 0x01, 0x08, 0x4b, 0x04, 0x01, 0x08, 0x4c, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00), CODE(0x96, 0x15, 0x00, 0x04, 0x01, 0x08, 0x13, 0x04, 0x01, 0x08, 0x4b, 0x04, 0x01, 0x08, 0x4c, 0x06, 0x00, 0xC0, 0x58, 0x40, 0x00, 0x00, 0x00, 0x00));
    RULE(INFINITE_COIN1, CODE(0x96, 0x08, 0x00, 0x04, 0x01, 0x08, 0x13, 0x04, 0x01, 0x08, 0x13, 0x4e, 0x51, 0x4f), CODE(0x96, 0x08, 0x00, 0x04, 0x01, 0x08, 0x13, 0x04, 0x01, 0x08, 0x13, 0x4e, 0x02, 0x4f));
    RULE(INFINITE_COIN2, CODE(0x96, 0x08, 0x00, 0x04, 0x01, 0x08, 0x13, 0x04, 0x01, 0x08, 0x13, 0x4e, 0x96, 0x02, 0x00, 0x08, 0x72, 0x1c, 0x0b, 0x4f), CODE(0x96, 0x08, 0x00, 0x04, 0x01, 0x08, 0x13, 0x04, 0x01, 0x08, 0x13, 0x4e, 0x96, 0x02, 0x00, 0x08, 0x72, 0x1c, 0x47, 0x4f));
    RULE(INFINITE_BOMBS, CODE(0x96, 0x08, 0x00, 0x04, 0x01, 0x08, 0x4b, 0x04, 0x01, 0x08, 0x4b, 0x4e, 0x51, 0x4f), CODE(0x96, 0x08, 0x00, 0x04, 0x01, 0x08, 0x4b, 0x04, 0x01, 0x08, 0x4b, 0x4e, 0x50, 0x4f));
    RULE(INFINITE_KEYS1, CODE(0x96, 0x08, 0x00, 0x04, 0x01, 0x08, 0x4c, 0x04, 0x01, 0x08, 0x4c, 0x4e, 0x51, 0x4f, 0x96, 0x06), CODE(0x96, 0x08, 0x00, 0x04, 0x01, 0x08, 0x4c, 0x04, 0x01, 0x08, 0x4c, 0x4e, 0x02, 0x4f, 0x96, 0x06));
    RULE(INFINITE_KEYS2, CODE(0x96, 0x08, 0x00, 0x04, 0x01, 0x08, 0x4c, 0x04, 0x01, 0x08, 0x4c, 0x4e, 0x51, 0x4f, 0x96, 0x02), CODE(0x96, 0x08, 0x00, 0x04, 0x01, 0x08, 0x4c, 0x04, 0x01, 0x08, 0x4c, 0x4e, 0x02, 0x4f, 0x96, 0x02));
    RULE(INFINITE_KEYS3, CODE(0x96, 0x08, 0x00, 0x04, 0x02, 0x08, 0x4c, 0x04, 0x02, 0x08, 0x4c, 0x4e, 0x51, 0x4f, 0x96, 0x07, 0x00, 0x04, 0x02, 0x09, 0xdc), CODE(0x96, 0x08, 0x00, 0x04, 0x02, 0x08, 0x4c, 0x04, 0x02, 0x08, 0x4c, 0x4e, 0x02, 0x4f, 0x96, 0x07, 0x00, 0x04, 0x02, 0x09, 0xdc));
    RULE(INFINITE_KEYS4, CODE(0x96, 0x08, 0x00, 0x04, 0x02, 0x08, 0x4c, 0x04, 0x02, 0x08, 0x4c, 0x4e, 0x51, 0x4f, 0x96, 0x07, 0x00, 0x04, 0x02, 0x09, 0x34), CODE(0x96, 0x08, 0x00, 0x04, 0x02, 0x08, 0x4c, 0x04, 0x02, 0x08, 0x4c, 0x4e, 0x02, 0x4f, 0x96, 0x07, 0x00, 0x04, 0x02, 0x09, 0x34));
    RULE(INFINITE_KEYS5, CODE(0x96, 0x08, 0x00, 0x04, 0x02, 0x08, 0x4c, 0x04, 0x02, 0x08, 0x4c, 0x4e, 0x51, 0x4f, 0x96, 0x07, 0x00, 0x04, 0x02, 0x09, 0x55), CODE(0x96, 0x08, 0x00, 0x04, 0x02, 0x08, 0x4c, 0x04, 0x02, 0x08, 0x4c, 0x4e, 0x02, 0x4f, 0x96, 0x07, 0x00, 0x04, 0x02, 0x09, 0x55));
    
    // RULE(DAMAGE_250, CODE(0x96, 0x09, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x87, 0x01, 0x00, 0x02, 0x17, 0x96, 0x02), CODE(0x96, 0x09, 0x00, 0x06, 0x00, 0x40, 0x6F, 0x40, 0x00, 0x00, 0x00, 0x00, 0x87, 0x01, 0x00, 0x02, 0x17, 0x96, 0x02));
    RULE(MAX_FIRE_RATE, CODE(0x96, 0x0c, 0x00, 0x04, 0x02, 0x09, 0x3a, 0x01, 0x07, 0x10, 0x00, 0x00, 0x00, 0x08, 0x72, 0x1c, 0x96, 0x05, 0x00, 0x07, 0x06, 0x00, 0x00, 0x00), CODE(0x96, 0x0c, 0x00, 0x04, 0x02, 0x09, 0x3a, 0x01, 0x07, 0x10, 0x00, 0x00, 0x00, 0x08, 0x72, 0x1c, 0x96, 0x05, 0x00, 0x07, 0xff, 0x00, 0x00, 0x00));

    // RULE(ALWAYS_SHOW_CURSE_ROOM, CODE(0x96, 0x02, 0x00, 0x08, 0x64, 0x1c, 0x96, 0x02, 0x00, 0x08, 0x30, 0x4e, 0x96, 0x09, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x67, 0x4c, 0x12, 0x9d, 0x02, 0x00, 0x44, 0x00, 0x17, 0x96, 0x05, 0x00, 0x07, 0x07, 0x00, 0x00, 0x00, 0x30, 0x96, 0x09, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x49, 0x96, 0x05, 0x00, 0x07, 0x04, 0x00, 0x00, 0x00, 0x30, 0x96, 0x09, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x49, 0x4c, 0x12, 0x9d, 0x02, 0x00, 0x0f, 0x00, 0x17, 0x96, 0x03, 0x00, 0x09, 0x9b, 0x05, 0x1c, 0x96, 0x03, 0x00, 0x09, 0x52, 0x05, 0x4e, 0x11, 0x12, 0x9d, 0x02, 0x00, 0x1a, 0x00, 0x96, 0x03, 0x00, 0x09, 0x9b, 0x05, 0x1c, 0x96, 0x0e, 0x00, 0x08, 0xb8, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x09, 0xb7, 0x05, 0x3d, 0x4f, 0x96, 0x02, 0x00, 0x08, 0x64, 0x1c, 0x96, 0x02, 0x00, 0x08, 0x30, 0x4e, 0x96, 0x09, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x67, 0x4c, 0x12, 0x9d, 0x02, 0x00, 0x43, 0x00, 0x17, 0x96, 0x05, 0x00, 0x07, 0x14, 0x00, 0x00, 0x00, 0x30, 0x96, 0x09, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x49, 0x96, 0x03, 0x00, 0x09, 0x9b, 0x05, 0x1c, 0x96, 0x02, 0x00, 0x08, 0x48, 0x4e, 0x4c, 0x12, 0x9d, 0x02, 0x00, 0x17, 0x00, 0x17, 0x96, 0x05, 0x00, 0x07, 0x04, 0x00, 0x00, 0x00, 0x30, 0x96, 0x09, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x49, 0x11, 0x12, 0x9d, 0x02, 0x00, 0x1a, 0x00, 0x96, 0x03, 0x00, 0x09, 0x9b, 0x05, 0x1c, 0x96, 0x0e, 0x00, 0x08, 0xb6, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x09, 0xb7, 0x05, 0x3d, 0x4f, 0x96, 0x02, 0x00, 0x08, 0x64, 0x1c, 0x96, 0x02, 0x00, 0x08, 0x30, 0x4e, 0x96, 0x09, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x67, 0x4c, 0x12, 0x9d, 0x02, 0x00, 0x44, 0x00, 0x17, 0x96, 0x05, 0x00, 0x07, 0x02, 0x00, 0x00, 0x00, 0x30, 0x96, 0x09, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x49, 0x96, 0x03, 0x00, 0x09, 0x9b, 0x05, 0x1c, 0x96, 0x03, 0x00, 0x09, 0x57, 0x05, 0x4e, 0x4c, 0x12, 0x9d, 0x02, 0x00, 0x17, 0x00, 0x17, 0x96, 0x05, 0x00, 0x07, 0x04, 0x00, 0x00, 0x00, 0x30, 0x96, 0x09, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x49, 0x11, 0x12, 0x9d, 0x02, 0x00, 0x1a, 0x00, 0x96, 0x03, 0x00, 0x09, 0x9b, 0x05, 0x1c, 0x96, 0x0e, 0x00, 0x08, 0xb2, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x09, 0xb7, 0x05, 0x3d, 0x4f, 0x96, 0x02, 0x00, 0x08, 0x64, 0x1c, 0x96, 0x02, 0x00, 0x08, 0x30, 0x4e, 0x96, 0x09, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x67, 0x4c, 0x12, 0x9d, 0x02, 0x00, 0x4c, 0x00, 0x17, 0x96, 0x05, 0x00, 0x07, 0x04, 0x00, 0x00, 0x00, 0x30, 0x96, 0x09, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x49, 0x96, 0x03, 0x00, 0x09, 0x9b, 0x05, 0x1c, 0x96, 0x02, 0x00, 0x08, 0x41, 0x4e, 0x96, 0x05, 0x00, 0x07, 0x01, 0x00, 0x00, 0x00, 0x49, 0x4c, 0x12, 0x9d, 0x02, 0x00, 0x17, 0x00, 0x17, 0x96, 0x05, 0x00, 0x07, 0x03, 0x00, 0x00, 0x00, 0x30, 0x96, 0x09, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x49, 0x11, 0x4c, 0x12, 0x9d, 0x02, 0x00, 0x21, 0x00, 0x17, 0x96, 0x03, 0x00, 0x09, 0x9b, 0x05, 0x1c, 0x96, 0x02, 0x00, 0x08, 0x4d, 0x4e, 0x96, 0x02, 0x00, 0x08, 0x30, 0x4e, 0x96, 0x09, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x67, 0x12, 0x9d, 0x02, 0x00, 0x1a, 0x00, 0x96, 0x03, 0x00, 0x09, 0x9b, 0x05, 0x1c, 0x96, 0x0e, 0x00, 0x08, 0xae, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x09, 0xb7, 0x05, 0x3d, 0x4f), CODE(0x96, 0x02, 0x00, 0x08, 0x64, 0x1c, 0x96, 0x02, 0x00, 0x08, 0x30, 0x4e, 0x96, 0x09, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x67, 0x4c, 0x12, 0x9d, 0x02, 0x00, 0x44, 0x00, 0x17, 0x96, 0x05, 0x00, 0x07, 0x07, 0x00, 0x00, 0x00, 0x30, 0x96, 0x09, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x49, 0x96, 0x05, 0x00, 0x07, 0x04, 0x00, 0x00, 0x00, 0x30, 0x96, 0x09, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x49, 0x4c, 0x12, 0x9d, 0x02, 0x00, 0x0f, 0x00, 0x17, 0x96, 0x03, 0x00, 0x09, 0x9b, 0x05, 0x1c, 0x96, 0x03, 0x00, 0x09, 0x52, 0x05, 0x4e, 0x11, 0x12, 0x02, 0x02, 0x02, 0x02, 0x02, 0x96, 0x03, 0x00, 0x09, 0x9b, 0x05, 0x1c, 0x96, 0x0e, 0x00, 0x08, 0xb8, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x09, 0xb7, 0x05, 0x3d, 0x4f, 0x96, 0x02, 0x00, 0x08, 0x64, 0x1c, 0x96, 0x02, 0x00, 0x08, 0x30, 0x4e, 0x96, 0x09, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x67, 0x4c, 0x12, 0x9d, 0x02, 0x00, 0x43, 0x00, 0x17, 0x96, 0x05, 0x00, 0x07, 0x14, 0x00, 0x00, 0x00, 0x30, 0x96, 0x09, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x49, 0x96, 0x03, 0x00, 0x09, 0x9b, 0x05, 0x1c, 0x96, 0x02, 0x00, 0x08, 0x48, 0x4e, 0x4c, 0x12, 0x9d, 0x02, 0x00, 0x17, 0x00, 0x17, 0x96, 0x05, 0x00, 0x07, 0x04, 0x00, 0x00, 0x00, 0x30, 0x96, 0x09, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x49, 0x11, 0x12, 0x9d, 0x02, 0x00, 0x1a, 0x00, 0x96, 0x03, 0x00, 0x09, 0x9b, 0x05, 0x1c, 0x96, 0x0e, 0x00, 0x08, 0xb6, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x09, 0xb7, 0x05, 0x3d, 0x4f, 0x96, 0x02, 0x00, 0x08, 0x64, 0x1c, 0x96, 0x02, 0x00, 0x08, 0x30, 0x4e, 0x96, 0x09, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x67, 0x4c, 0x12, 0x9d, 0x02, 0x00, 0x44, 0x00, 0x17, 0x96, 0x05, 0x00, 0x07, 0x02, 0x00, 0x00, 0x00, 0x30, 0x96, 0x09, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x49, 0x96, 0x03, 0x00, 0x09, 0x9b, 0x05, 0x1c, 0x96, 0x03, 0x00, 0x09, 0x57, 0x05, 0x4e, 0x4c, 0x12, 0x9d, 0x02, 0x00, 0x17, 0x00, 0x17, 0x96, 0x05, 0x00, 0x07, 0x04, 0x00, 0x00, 0x00, 0x30, 0x96, 0x09, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x49, 0x11, 0x12, 0x9d, 0x02, 0x00, 0x1a, 0x00, 0x96, 0x03, 0x00, 0x09, 0x9b, 0x05, 0x1c, 0x96, 0x0e, 0x00, 0x08, 0xb2, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x09, 0xb7, 0x05, 0x3d, 0x4f, 0x96, 0x02, 0x00, 0x08, 0x64, 0x1c, 0x96, 0x02, 0x00, 0x08, 0x30, 0x4e, 0x96, 0x09, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x67, 0x4c, 0x12, 0x9d, 0x02, 0x00, 0x4c, 0x00, 0x17, 0x96, 0x05, 0x00, 0x07, 0x04, 0x00, 0x00, 0x00, 0x30, 0x96, 0x09, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x49, 0x96, 0x03, 0x00, 0x09, 0x9b, 0x05, 0x1c, 0x96, 0x02, 0x00, 0x08, 0x41, 0x4e, 0x96, 0x05, 0x00, 0x07, 0x01, 0x00, 0x00, 0x00, 0x49, 0x4c, 0x12, 0x9d, 0x02, 0x00, 0x17, 0x00, 0x17, 0x96, 0x05, 0x00, 0x07, 0x03, 0x00, 0x00, 0x00, 0x30, 0x96, 0x09, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x49, 0x11, 0x4c, 0x12, 0x9d, 0x02, 0x00, 0x21, 0x00, 0x17, 0x96, 0x03, 0x00, 0x09, 0x9b, 0x05, 0x1c, 0x96, 0x02, 0x00, 0x08, 0x4d, 0x4e, 0x96, 0x02, 0x00, 0x08, 0x30, 0x4e, 0x96, 0x09, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x67, 0x12, 0x9d, 0x02, 0x00, 0x1a, 0x00, 0x96, 0x03, 0x00, 0x09, 0x9b, 0x05, 0x1c, 0x96, 0x0e, 0x00, 0x08, 0xae, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x09, 0xb7, 0x05, 0x3d, 0x4f));

    RULE(TWO_TREASURE_ROOMS, CODE(0x96, 0x03, 0x00, 0x09, 0x9b, 0x05, 0x1c, 0x96, 0x02, 0x00, 0x08, 0x83, 0x4e, 0x4c, 0x12, 0x9d, 0x02, 0x00, 0x17, 0x00, 0x17, 0x96, 0x03, 0x00, 0x09, 0x9b, 0x05, 0x1c, 0x96, 0x02, 0x00, 0x08, 0x5c, 0x4e, 0x96, 0x05, 0x00, 0x07, 0x05, 0x00, 0x00, 0x00, 0x48, 0x12, 0x9d, 0x02, 0x00, 0x1a, 0x00, 0x96, 0x03, 0x00, 0x09, 0x9b, 0x05, 0x1c, 0x96, 0x0e, 0x00, 0x08, 0x82, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x09, 0xb7, 0x05, 0x3d, 0x4f, 0x96, 0x02, 0x00, 0x08, 0x64, 0x1c, 0x96, 0x02, 0x00, 0x08, 0x30, 0x4e, 0x96, 0x09, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x67, 0x4c, 0x12, 0x9d, 0x02, 0x00, 0x44, 0x00, 0x17, 0x96, 0x05, 0x00, 0x07, 0x07, 0x00, 0x00, 0x00, 0x30, 0x96, 0x09, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x49, 0x96, 0x05, 0x00, 0x07, 0x04, 0x00, 0x00, 0x00, 0x30, 0x96, 0x09, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x49, 0x4c, 0x12, 0x9d, 0x02, 0x00, 0x0f, 0x00, 0x17, 0x96, 0x03, 0x00, 0x09, 0x9b, 0x05, 0x1c, 0x96, 0x03, 0x00, 0x09, 0x52, 0x05, 0x4e, 0x11, 0x12, 0x9d, 0x02, 0x00, 0x1a, 0x00, 0x96, 0x03, 0x00, 0x09, 0x9b, 0x05, 0x1c, 0x96, 0x0e, 0x00, 0x08, 0xb8, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x09, 0xb7, 0x05, 0x3d, 0x4f, 0x96, 0x02, 0x00, 0x08, 0x64, 0x1c, 0x96, 0x02, 0x00, 0x08, 0x30, 0x4e, 0x96, 0x09, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x67, 0x4c, 0x12, 0x9d, 0x02, 0x00, 0x43, 0x00, 0x17, 0x96, 0x05, 0x00, 0x07, 0x14, 0x00, 0x00, 0x00, 0x30, 0x96, 0x09, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x49, 0x96, 0x03, 0x00, 0x09, 0x9b, 0x05, 0x1c, 0x96, 0x02, 0x00, 0x08, 0x48, 0x4e, 0x4c, 0x12, 0x9d, 0x02, 0x00, 0x17, 0x00, 0x17, 0x96, 0x05, 0x00, 0x07, 0x04, 0x00, 0x00, 0x00, 0x30, 0x96, 0x09, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x49, 0x11, 0x12, 0x9d, 0x02, 0x00, 0x1a, 0x00, 0x96, 0x03, 0x00, 0x09, 0x9b, 0x05, 0x1c, 0x96, 0x0e, 0x00, 0x08, 0xb6, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x09, 0xb7, 0x05, 0x3d, 0x4f, 0x96, 0x02, 0x00, 0x08, 0x64, 0x1c, 0x96, 0x02, 0x00, 0x08, 0x30, 0x4e, 0x96, 0x09, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x67, 0x4c, 0x12, 0x9d, 0x02, 0x00, 0x44, 0x00, 0x17, 0x96, 0x05, 0x00, 0x07, 0x02, 0x00, 0x00, 0x00, 0x30, 0x96, 0x09, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x49, 0x96, 0x03, 0x00, 0x09, 0x9b, 0x05, 0x1c, 0x96, 0x03, 0x00, 0x09, 0x57, 0x05, 0x4e, 0x4c, 0x12, 0x9d, 0x02, 0x00, 0x17, 0x00, 0x17, 0x96, 0x05, 0x00, 0x07, 0x04, 0x00, 0x00, 0x00, 0x30, 0x96, 0x09, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x49, 0x11, 0x12, 0x9d, 0x02, 0x00, 0x1a, 0x00, 0x96, 0x03, 0x00, 0x09, 0x9b, 0x05, 0x1c, 0x96, 0x0e, 0x00, 0x08, 0xb2, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x09, 0xb7, 0x05, 0x3d, 0x4f, 0x96, 0x02, 0x00, 0x08, 0x64, 0x1c, 0x96, 0x02, 0x00, 0x08, 0x30, 0x4e, 0x96, 0x09, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x67, 0x4c, 0x12, 0x9d, 0x02, 0x00, 0x4c, 0x00, 0x17, 0x96, 0x05, 0x00, 0x07, 0x04, 0x00, 0x00, 0x00, 0x30, 0x96, 0x09, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x49, 0x96, 0x03, 0x00, 0x09, 0x9b, 0x05, 0x1c, 0x96, 0x02, 0x00, 0x08, 0x41, 0x4e, 0x96, 0x05, 0x00, 0x07, 0x01, 0x00, 0x00, 0x00, 0x49, 0x4c, 0x12, 0x9d, 0x02, 0x00, 0x17, 0x00, 0x17, 0x96, 0x05, 0x00, 0x07, 0x03, 0x00, 0x00, 0x00, 0x30, 0x96, 0x09, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x49, 0x11, 0x4c, 0x12, 0x9d, 0x02, 0x00, 0x21, 0x00, 0x17, 0x96, 0x03, 0x00, 0x09, 0x9b, 0x05, 0x1c, 0x96, 0x02, 0x00, 0x08, 0x4d, 0x4e, 0x96, 0x02, 0x00, 0x08, 0x30, 0x4e, 0x96, 0x09, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x67, 0x12, 0x9d, 0x02, 0x00, 0x1a, 0x00, 0x96, 0x03, 0x00, 0x09, 0x9b, 0x05, 0x1c, 0x96, 0x0e, 0x00, 0x08, 0xae, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x09, 0xb7, 0x05, 0x3d, 0x4f), CODE(0x96, 0x03, 0x00, 0x09, 0x9b, 0x05, 0x1c, 0x96, 0x02, 0x00, 0x08, 0x83, 0x4e, 0x4c, 0x12, 0x02, 0x02, 0x02, 0x02, 0x02, 0x17, 0x96, 0x03, 0x00, 0x09, 0x9b, 0x05, 0x1c, 0x96, 0x02, 0x00, 0x08, 0x5c, 0x4e, 0x96, 0x05, 0x00, 0x07, 0x05, 0x00, 0x00, 0x00, 0x48, 0x12, 0x02, 0x02, 0x02, 0x02, 0x02, 0x96, 0x03, 0x00, 0x09, 0x9b, 0x05, 0x1c, 0x96, 0x0e, 0x00, 0x08, 0x82, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x09, 0xb7, 0x05, 0x3d, 0x4f, 0x96, 0x02, 0x00, 0x08, 0x64, 0x1c, 0x96, 0x02, 0x00, 0x08, 0x30, 0x4e, 0x96, 0x09, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x67, 0x4c, 0x12, 0x9d, 0x02, 0x00, 0x44, 0x00, 0x17, 0x96, 0x05, 0x00, 0x07, 0x07, 0x00, 0x00, 0x00, 0x30, 0x96, 0x09, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x49, 0x96, 0x05, 0x00, 0x07, 0x04, 0x00, 0x00, 0x00, 0x30, 0x96, 0x09, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x49, 0x4c, 0x12, 0x9d, 0x02, 0x00, 0x0f, 0x00, 0x17, 0x96, 0x03, 0x00, 0x09, 0x9b, 0x05, 0x1c, 0x96, 0x03, 0x00, 0x09, 0x52, 0x05, 0x4e, 0x11, 0x12, 0x9d, 0x02, 0x00, 0x1a, 0x00, 0x96, 0x03, 0x00, 0x09, 0x9b, 0x05, 0x1c, 0x96, 0x0e, 0x00, 0x08, 0xb8, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x09, 0xb7, 0x05, 0x3d, 0x4f, 0x96, 0x02, 0x00, 0x08, 0x64, 0x1c, 0x96, 0x02, 0x00, 0x08, 0x30, 0x4e, 0x96, 0x09, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x67, 0x4c, 0x12, 0x9d, 0x02, 0x00, 0x43, 0x00, 0x17, 0x96, 0x05, 0x00, 0x07, 0x14, 0x00, 0x00, 0x00, 0x30, 0x96, 0x09, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x49, 0x96, 0x03, 0x00, 0x09, 0x9b, 0x05, 0x1c, 0x96, 0x02, 0x00, 0x08, 0x48, 0x4e, 0x4c, 0x12, 0x9d, 0x02, 0x00, 0x17, 0x00, 0x17, 0x96, 0x05, 0x00, 0x07, 0x04, 0x00, 0x00, 0x00, 0x30, 0x96, 0x09, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x49, 0x11, 0x12, 0x9d, 0x02, 0x00, 0x1a, 0x00, 0x96, 0x03, 0x00, 0x09, 0x9b, 0x05, 0x1c, 0x96, 0x0e, 0x00, 0x08, 0xb6, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x09, 0xb7, 0x05, 0x3d, 0x4f, 0x96, 0x02, 0x00, 0x08, 0x64, 0x1c, 0x96, 0x02, 0x00, 0x08, 0x30, 0x4e, 0x96, 0x09, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x67, 0x4c, 0x12, 0x9d, 0x02, 0x00, 0x44, 0x00, 0x17, 0x96, 0x05, 0x00, 0x07, 0x02, 0x00, 0x00, 0x00, 0x30, 0x96, 0x09, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x49, 0x96, 0x03, 0x00, 0x09, 0x9b, 0x05, 0x1c, 0x96, 0x03, 0x00, 0x09, 0x57, 0x05, 0x4e, 0x4c, 0x12, 0x9d, 0x02, 0x00, 0x17, 0x00, 0x17, 0x96, 0x05, 0x00, 0x07, 0x04, 0x00, 0x00, 0x00, 0x30, 0x96, 0x09, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x49, 0x11, 0x12, 0x9d, 0x02, 0x00, 0x1a, 0x00, 0x96, 0x03, 0x00, 0x09, 0x9b, 0x05, 0x1c, 0x96, 0x0e, 0x00, 0x08, 0xb2, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x09, 0xb7, 0x05, 0x3d, 0x4f, 0x96, 0x02, 0x00, 0x08, 0x64, 0x1c, 0x96, 0x02, 0x00, 0x08, 0x30, 0x4e, 0x96, 0x09, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x67, 0x4c, 0x12, 0x9d, 0x02, 0x00, 0x4c, 0x00, 0x17, 0x96, 0x05, 0x00, 0x07, 0x04, 0x00, 0x00, 0x00, 0x30, 0x96, 0x09, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x49, 0x96, 0x03, 0x00, 0x09, 0x9b, 0x05, 0x1c, 0x96, 0x02, 0x00, 0x08, 0x41, 0x4e, 0x96, 0x05, 0x00, 0x07, 0x01, 0x00, 0x00, 0x00, 0x49, 0x4c, 0x12, 0x9d, 0x02, 0x00, 0x17, 0x00, 0x17, 0x96, 0x05, 0x00, 0x07, 0x03, 0x00, 0x00, 0x00, 0x30, 0x96, 0x09, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x49, 0x11, 0x4c, 0x12, 0x9d, 0x02, 0x00, 0x21, 0x00, 0x17, 0x96, 0x03, 0x00, 0x09, 0x9b, 0x05, 0x1c, 0x96, 0x02, 0x00, 0x08, 0x4d, 0x4e, 0x96, 0x02, 0x00, 0x08, 0x30, 0x4e, 0x96, 0x09, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x67, 0x12, 0x9d, 0x02, 0x00, 0x1a, 0x00, 0x96, 0x03, 0x00, 0x09, 0x9b, 0x05, 0x1c, 0x96, 0x0e, 0x00, 0x08, 0xae, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x09, 0xb7, 0x05, 0x3d, 0x4f));
    RULE(TWO_TREASURE_ROOMS2, CODE(0x9d, 0x02, 0x00, 0x2a, 0x00, 0x96, 0x03, 0x00, 0x09, 0x9b, 0x05, 0x1c, 0x96, 0x03, 0x00, 0x08, 0x81, 0x03, 0x4f, 0x96, 0x03, 0x00, 0x09, 0x9b, 0x05, 0x1c, 0x96, 0x03, 0x00, 0x08, 0x82, 0x03, 0x4f), CODE(0x9d, 0x02, 0x00, 0x2a, 0x00, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02));

    RULE(ROBO_BABY, CODE(0x96, 0x2f, 0x00, 0x09, 0x00, 0x02, 0x07, 0x9d, 0x00, 0x00, 0x00, 0x07, 0x75, 0x00, 0x00, 0x00, 0x07, 0x24, 0x00, 0x00, 0x00, 0x07, 0x22, 0x00, 0x00, 0x00, 0x07, 0x2e, 0x00, 0x00, 0x00, 0x07, 0x2d, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x07, 0x00, 0x00, 0x00), CODE(0x96, 0x2f, 0x00, 0x09, 0x00, 0x02, 0x07, 0x5f, 0x00, 0x00, 0x00, 0x07, 0x5F, 0x00, 0x00, 0x00, 0x07, 0x5F, 0x00, 0x00, 0x00, 0x07, 0x5F, 0x00, 0x00, 0x00, 0x07, 0x5F, 0x00, 0x00, 0x00, 0x07, 0x5F, 0x00, 0x00, 0x00, 0x07, 0x5F, 0x00, 0x00, 0x00, 0x07, 0x07, 0x00, 0x00, 0x00, 0x90, 0x90, 0x90, 0x90));

    RULE(OBTAINED_ITEM_REAPPEAR , CODE(0x96, 0x10, 0x00, 0x07, 0x01, 0x00, 0x00, 0x00, 0x04, 0x03, 0x07, 0x02, 0x00, 0x00, 0x00, 0x04, 0x01, 0x08, 0x22, 0x4E, 0x96, 0x02, 0x00, 0x08, 0x31, 0x52, 0x17, 0x96), CODE(0x96, 0x10, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x04, 0x03, 0x07, 0x02, 0x00, 0x00, 0x00, 0x04, 0x01, 0x08, 0x22, 0x4E, 0x96, 0x02, 0x00, 0x08, 0x31, 0x52, 0x17, 0x96));
    RULE(OBTAINED_ITEM_REAPPEAR1, CODE(0x96, 0x10, 0x00, 0x07, 0x01, 0x00, 0x00, 0x00, 0x04, 0x03, 0x07, 0x02, 0x00, 0x00, 0x00, 0x04, 0x01, 0x08, 0x2a), CODE(0x96, 0x10, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x04, 0x03, 0x07, 0x02, 0x00, 0x00, 0x00, 0x04, 0x01, 0x08, 0x2a));
    RULE(OBTAINED_ITEM_REAPPEAR2, CODE(0x96, 0x10, 0x00, 0x07, 0x01, 0x00, 0x00, 0x00, 0x04, 0x03, 0x07, 0x02, 0x00, 0x00, 0x00, 0x04, 0x01, 0x08, 0x23), CODE(0x96, 0x10, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x04, 0x03, 0x07, 0x02, 0x00, 0x00, 0x00, 0x04, 0x01, 0x08, 0x23));
    RULE(OBTAINED_ITEM_REAPPEAR3, CODE(0x96, 0x10, 0x00, 0x07, 0x01, 0x00, 0x00, 0x00, 0x04, 0x03, 0x07, 0x02, 0x00, 0x00, 0x00, 0x04, 0x01, 0x08, 0x29), CODE(0x96, 0x10, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x04, 0x03, 0x07, 0x02, 0x00, 0x00, 0x00, 0x04, 0x01, 0x08, 0x29));
    RULE(OBTAINED_ITEM_REAPPEAR4, CODE(0x96, 0x10, 0x00, 0x07, 0x01, 0x00, 0x00, 0x00, 0x04, 0x03, 0x07, 0x02, 0x00, 0x00, 0x00, 0x04, 0x01, 0x08, 0x28), CODE(0x96, 0x10, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x04, 0x03, 0x07, 0x02, 0x00, 0x00, 0x00, 0x04, 0x01, 0x08, 0x28));
    RULE(OBTAINED_ITEM_REAPPEAR5, CODE(0x96, 0x10, 0x00, 0x07, 0x01, 0x00, 0x00, 0x00, 0x04, 0x03, 0x07, 0x02, 0x00, 0x00, 0x00, 0x04, 0x01, 0x08, 0xda), CODE(0x96, 0x10, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x04, 0x03, 0x07, 0x02, 0x00, 0x00, 0x00, 0x04, 0x01, 0x08, 0xda));
    RULE(OBTAINED_ITEM_REAPPEAR6, CODE(0x96, 0x10, 0x00, 0x07, 0x01, 0x00, 0x00, 0x00, 0x04, 0x03, 0x07, 0x02, 0x00, 0x00, 0x00, 0x04, 0x01, 0x08, 0x27), CODE(0x96, 0x10, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x04, 0x03, 0x07, 0x02, 0x00, 0x00, 0x00, 0x04, 0x01, 0x08, 0x27));
    RULE(OBTAINED_ITEM_REAPPEAR7, CODE(0x96, 0x10, 0x00, 0x07, 0x01, 0x00, 0x00, 0x00, 0x04, 0x03, 0x07, 0x02, 0x00, 0x00, 0x00, 0x04, 0x01, 0x08, 0x26), CODE(0x96, 0x10, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x04, 0x03, 0x07, 0x02, 0x00, 0x00, 0x00, 0x04, 0x01, 0x08, 0x26));
    RULE(OBTAINED_ITEM_REAPPEAR8, CODE(0x96, 0x10, 0x00, 0x07, 0x01, 0x00, 0x00, 0x00, 0x04, 0x03, 0x07, 0x02, 0x00, 0x00, 0x00, 0x04, 0x01, 0x08, 0x22, 0x4E, 0x96, 0x02, 0x00, 0x08, 0x31, 0x52, 0x17, 0x99), CODE(0x96, 0x10, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x04, 0x03, 0x07, 0x02, 0x00, 0x00, 0x00, 0x04, 0x01, 0x08, 0x22, 0x4E, 0x96, 0x02, 0x00, 0x08, 0x31, 0x52, 0x17, 0x99));

    return 0;

}
