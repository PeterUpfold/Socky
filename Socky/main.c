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
    
    // get address
    CFRetain(address);
    
    int portNumber = *((int*)info);
    
    struct sockaddr_in nativeAddress;
    CFDataGetBytes(address, CFRangeMake(0, CFDataGetLength(address)), &nativeAddress);
    CFRelease(address);
    
    in_port_t port = nativeAddress.sin_port;
    
    CFStringRef description = CFStringCreateWithFormat(NULL, NULL, CFSTR("Received a connection on port %d."), portNumber);
    
    // post a CFNotificationCenter notification
    CFNotificationCenterPostNotification(CFNotificationCenterGetDistributedCenter(),
                                         CFSTR("uk.org.upfold.Socky.ReceivedConnection"),
                                         description,
                                         NULL,
                                         true);
    printf("Got connection on %d!\n", portNumber);
    CFRelease(description);
}

int main(int argc, const char * argv[]) {
    
    CFSocketRef sockets[argc-1];
    CFRunLoopSourceRef runLoopSources[argc-1];
    
    // socket context — store information about the ports for sending to the notifier agent
    long int ports[argc-1];
    for (int i = 0; i < argc-1; i++) {
        ports[i] = 0;
    }
    
    if (argc < 2) {
        printf("Pass arguments for the ports on which you want Socky to listen.\n");
        return 3;
    }
    
    for (int i = 1; i < argc; i++) {
        
        CFSocketContext context;
        
        ports[i-1] = strtol(argv[i], NULL, 10);
        
        context.copyDescription = NULL;
        context.release = NULL;
        context.retain = NULL;
        context.info = &ports[i-1];
        
        sockets[i-1] = CFSocketCreate(NULL,
                                PF_INET,
                                SOCK_STREAM,
                                IPPROTO_TCP,
                                kCFSocketAcceptCallBack | kCFSocketConnectCallBack,
                                &callback,
                                &context);
        
        
        if (sockets[i-1] == NULL) {
            printf("Failed to create socket");
            return 1;
        }
        
        // bind to port
        struct sockaddr_in address;
        address.sin_family = AF_INET;
        address.sin_port = htons((int)ports[i-1]);
        inet_aton("127.0.0.1", &address.sin_addr.s_addr);
        CFDataRef addressData = CFDataCreate(NULL, &address, sizeof(address));
        
        
        
        CFSocketError bindResult = CFSocketSetAddress(sockets[i-1], addressData);
        if (bindResult != kCFSocketSuccess) {
            printf("Failed to bind to 127.0.0.1:%s with %ld\n", argv[i], (long)bindResult);
            CFRelease(addressData);
            continue;
        }
        
        printf("Listening on 127.0.0.1:%s\n", argv[i]);
        
        // add to run loop
        runLoopSources[i-1] = CFSocketCreateRunLoopSource(NULL, sockets[i-1], 1);
        if (runLoopSources[i-1] == NULL) {
            printf("Unable to get CFRunLoop source reference");
            return 2;
        }
        CFRunLoopAddSource(CFRunLoopGetCurrent(), runLoopSources[i-1], kCFRunLoopDefaultMode);
        CFRelease(addressData);
    }
    
    
    
    printf("Beginning run loop\n");
    
    
    CFRunLoopRun();
    
    
    for (int i = 0; i < argc; i++) {
        CFRelease(sockets[i]);
        CFRelease(runLoopSources[i]);
    }
    
    
    return 0;
}

