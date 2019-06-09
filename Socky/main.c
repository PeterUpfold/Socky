/*
 * Socky.
 *
 * Copyright (C) 2019 Peter Upfold.
 *
 * This file is licensed to you under the Apache License, version 2.0 (the "License").
 * You may not use this file except in compliance with the License. You may obtain
 * a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software distributed under
 * the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied. See the License for the specific language governing
 * permissions and limitations under the License.
 *
 */

#include <stdio.h>
#import <CoreFoundation/CoreFoundation.h>
#include <sys/socket.h>
#include <netinet/in.h>


void callback(CFSocketRef socket, CFSocketCallBackType type, CFDataRef address, const void *data, void *info) {
    printf("Got a callback instance");
}

int main(int argc, const char * argv[]) {
    CFSocketRef socket;
    
    socket = CFSocketCreate(NULL,
                            PF_INET,
                            SOCK_STREAM,
                            IPPROTO_TCP,
                            kCFSocketReadCallBack | kCFSocketAcceptCallBack | kCFSocketConnectCallBack,
                            &callback,
                            NULL);
    
    if (socket == NULL) {
        printf("Failed to create socket");
        return 1;
    }
    
    // bind to port
    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(27060);
    inet_aton("127.0.0.1", &address.sin_addr.s_addr);
    CFDataRef addressData = CFDataCreate(NULL, &address, sizeof(address));
    
    CFSocketError bindResult = CFSocketSetAddress(socket, addressData);
    if (bindResult != kCFSocketSuccess) {
        printf("Failed to bind with %d\n", bindResult);
    }
    
    // add to run loop
    CFRunLoopSourceRef runLoopSource = CFSocketCreateRunLoopSource(NULL, socket, 1);
    if (runLoopSource == NULL) {
        printf("Unable to get CFRunLoop source reference");
        return 2;
    }
    CFRunLoopAddSource(CFRunLoopGetCurrent(), runLoopSource, kCFRunLoopDefaultMode);
    
    
    printf("Beginning run loop\n");
    
    
    CFRunLoopRun();
    CFRelease(addressData);
    CFRelease(runLoopSource);
    CFRelease(socket);
    
    return 0;
}

